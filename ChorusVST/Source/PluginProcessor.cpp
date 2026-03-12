/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusVSTAudioProcessor::ChorusVSTAudioProcessor()
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

ChorusVSTAudioProcessor::~ChorusVSTAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
ChorusVSTAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"depth", 1},
        "Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"speed", 1},
        "Speed",
        juce::NormalisableRange<float>(0.01f, 10.0f, 0.01f),
        0.5f));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"slow_fast",1},
        "Fast",
        false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"stereo",1},
        "Stereo",
        false));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String ChorusVSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChorusVSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChorusVSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChorusVSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChorusVSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChorusVSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChorusVSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChorusVSTAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ChorusVSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChorusVSTAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ChorusVSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    choruses.resize(getTotalNumInputChannels());
    
    for (auto& c : choruses)
        c.prepare(sampleRate);   // if you have prepare()
    
    float speed = *parameters.getRawParameterValue("speed");
    float depth = *parameters.getRawParameterValue("depth");
    
    bool slow_fast = *parameters.getRawParameterValue("slow_fast");
    bool stereo = *parameters.getRawParameterValue("stereo");
    
    float i = 0.;
    float phase = 0.;
    for (auto& chorus : choruses){
        chorus.setSpeed(speed);
        chorus.setDepth(depth);
        phase = i*stereo/2;
        chorus.setPhase(phase);
        i++;
        // TODO: stereo and speed
    }
    
    parameters.addParameterListener("speed", this);
    parameters.addParameterListener("depth", this);
    
    parameters.addParameterListener("stereo", this);
    parameters.addParameterListener("slow_fast", this);
}

void ChorusVSTAudioProcessor::parameterChanged(const juce::String& id, float newValue)
{
    if (id == "speed"){
        for (auto& chorus : choruses){
            chorus.setSpeed(newValue);
        }
    }else if(id == "depth"){
        for (auto& chorus : choruses){
            chorus.setDepth(newValue);
        }
    }else if(id == "stereo"){
        float i = 0.;
        float phase = 0.;
        for (auto& chorus : choruses){
            phase = i*newValue/2;
            chorus.setPhase(phase);
            i++;
        }
    }else if(id == "slow_fast"){
//        float i = -0.1f;
        for (auto& chorus : choruses){
            chorus.setFeedback(newValue*0.5);
//            float speed = *parameters.getRawParameterValue("speed");
//            chorus.setSpeed(speed*(1.+ newValue*i));
//            i += 0.2;
        }
    }
}

void ChorusVSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChorusVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ChorusVSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
        auto& chorusChannel = choruses[channel];

        // ..do something to the data...
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = chorusChannel.process_sample(channelData[sample]);
        }
    }
}

//==============================================================================
bool ChorusVSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChorusVSTAudioProcessor::createEditor()
{
    return new ChorusVSTAudioProcessorEditor (*this);
}

//==============================================================================
void ChorusVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ChorusVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new ChorusVSTAudioProcessor();
}
