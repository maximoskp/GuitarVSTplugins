/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbVSTAudioProcessor::ReverbVSTAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

ReverbVSTAudioProcessor::~ReverbVSTAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
ReverbVSTAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"room_size", 1},
        "Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"damping", 1},
        "Damp",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"wet", 1},
        "Wet",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"dry", 1},
        "Dry",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String ReverbVSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverbVSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReverbVSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReverbVSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReverbVSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReverbVSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ReverbVSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReverbVSTAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ReverbVSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void ReverbVSTAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ReverbVSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    reverbs.resize(getTotalNumInputChannels());
        
    for (auto& r : reverbs)
        r.prepare(sampleRate);   // if you have prepare()
    
    float room_size = *parameters.getRawParameterValue("room_size");
    float damping = *parameters.getRawParameterValue("damping");
    float wet = *parameters.getRawParameterValue("wet");
    float dry = *parameters.getRawParameterValue("dry");
    
    for (auto& reverb : reverbs){
        reverb.setRoomSize(room_size);
        reverb.setDamping(damping);
        reverb.setWet(wet);
        reverb.setDry(dry);
    }
    
    parameters.addParameterListener("room_size", this);
    parameters.addParameterListener("damping", this);
    parameters.addParameterListener("wet", this);
    parameters.addParameterListener("dry", this);
}
void ReverbVSTAudioProcessor::parameterChanged(const juce::String& id, float newValue)
{
    if (id == "room_size"){
        for (auto& reverb : reverbs) reverb.setRoomSize(newValue);
    }else if(id == "damping"){
        for (auto& reverb : reverbs) reverb.setDamping(newValue);
    }else if(id == "wet"){
        for (auto& reverb : reverbs) reverb.setWet(newValue);
    }else if(id == "dry"){
        for (auto& reverb : reverbs) reverb.setDry(newValue);
    }
}

void ReverbVSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReverbVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ReverbVSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto& reverbChannel = reverbs[channel];

        // ..do something to the data...
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = reverbChannel.process_sample(channelData[sample]);
        }
    }
}

//==============================================================================
bool ReverbVSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReverbVSTAudioProcessor::createEditor()
{
    return new ReverbVSTAudioProcessorEditor (*this);
}

//==============================================================================
void ReverbVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ReverbVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbVSTAudioProcessor();
}
