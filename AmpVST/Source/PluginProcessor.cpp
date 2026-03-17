/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmpVSTAudioProcessor::AmpVSTAudioProcessor()
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

AmpVSTAudioProcessor::~AmpVSTAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
AmpVSTAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"pre_highf", 1},
        "Pre_HF",
        juce::NormalisableRange<float>(50.0f, 1000.0f, 1.0f),
        120.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"pre_lowf",1},
        "Pre_LF",
        juce::NormalisableRange<float>(1000.0f, 8000.0f, 1.0f),
        5000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"post_highf",1},
        "Post_HF",
        juce::NormalisableRange<float>(50.0f, 1000.0f, 1.0f),
        120.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"post_lowf",1},
        "Post_LF",
        juce::NormalisableRange<float>(1000.0f, 8000.0f, 1.0f),
        5000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"gain",1},
        "Gain",
        juce::NormalisableRange<float>(0.1f, 5.0f, 0.01),
        1.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"pre_highf_on",1},
        "Pre_HF_ON",
        true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"pre_lowf_on",1},
        "Pre_LF_ON",
        false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"post_highf_on",1},
        "Post_HF_ON",
        false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"post_lowf_on",1},
        "Post_LF_ON",
        false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"dist_onoff",1},
        "Dist_ON",
        false));
    
    return { params.begin(), params.end() };
}


//==============================================================================
const juce::String AmpVSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AmpVSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AmpVSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AmpVSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AmpVSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AmpVSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AmpVSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AmpVSTAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AmpVSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void AmpVSTAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AmpVSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
//    amp.set_sample_rate(sampleRate);
    amps.resize(getTotalNumInputChannels());
    
    for (auto& a : amps)
        a.set_sample_rate(sampleRate);   // if you have prepare()
    
    float pre_highf = *parameters.getRawParameterValue("pre_highf");
    float pre_lowf = *parameters.getRawParameterValue("pre_lowf");
    float post_highf = *parameters.getRawParameterValue("post_highf");
    float post_lowf = *parameters.getRawParameterValue("post_lowf");
    float gain = *parameters.getRawParameterValue("gain");
    
    float pre_highf_on = *parameters.getRawParameterValue("pre_highf_on");
    float pre_lowf_on = *parameters.getRawParameterValue("pre_lowf_on");
    float post_highf_on = *parameters.getRawParameterValue("post_highf_on");
    float post_lowf_on = *parameters.getRawParameterValue("post_lowf_on");
    float dist_onoff = *parameters.getRawParameterValue("dist_onoff");
    
    for (auto& amp : amps){
        amp.set_pre_highf(pre_highf);
        amp.set_pre_lowf(pre_lowf);
        amp.set_post_highf(post_highf);
        amp.set_post_lowf(post_lowf);
        amp.set_gain(gain);
        
        amp.toggle_pre_high(pre_highf_on);
        amp.toggle_pre_low(pre_lowf_on);
        amp.toggle_dist(dist_onoff);
        amp.toggle_post_high(post_highf_on);
        amp.toggle_post_low(post_lowf_on);
    }
    
    parameters.addParameterListener("pre_highf", this);
    parameters.addParameterListener("pre_lowf", this);
    parameters.addParameterListener("post_highf", this);
    parameters.addParameterListener("post_lowf", this);
    parameters.addParameterListener("gain", this);
    
    parameters.addParameterListener("pre_highf_on", this);
    parameters.addParameterListener("pre_lowf_on", this);
    parameters.addParameterListener("post_highf_on", this);
    parameters.addParameterListener("post_lowf_on", this);
    parameters.addParameterListener("dist_onoff", this);
}

void AmpVSTAudioProcessor::parameterChanged(const juce::String& id, float newValue)
{
    if (id == "pre_highf"){
        for (auto& amp : amps) amp.set_pre_highf(newValue);
    }else if(id == "pre_lowf"){
        for (auto& amp : amps) amp.set_pre_lowf(newValue);
    }else if(id == "post_highf"){
        for (auto& amp : amps) amp.set_post_highf(newValue);
    }else if(id == "post_lowf"){
        for (auto& amp : amps) amp.set_post_lowf(newValue);
    }else if(id == "gain"){
        for (auto& amp : amps) amp.set_gain(newValue);
    }else if(id == "pre_highf_on"){
        for (auto& amp : amps) amp.toggle_pre_high(newValue);
    }else if(id == "pre_lowf_on"){
        for (auto& amp : amps) amp.toggle_pre_low(newValue);
    }else if(id == "post_highf_on"){
        for (auto& amp : amps) amp.toggle_post_high(newValue);
    }else if(id == "post_lowf_on"){
        for (auto& amp : amps) amp.toggle_post_low(newValue);
    }else if(id == "dist_onoff"){
        for (auto& amp : amps) amp.toggle_dist(newValue);
    }
}

void AmpVSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AmpVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AmpVSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
        auto& ampChannel = amps[channel];
        
        // ..do something to the data...
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
//            channelData[sample] = amp.process_sample(channelData[sample]);//channelData[sample];
            channelData[sample] = ampChannel.process_sample(channelData[sample]);
        }
    }
}

//==============================================================================
bool AmpVSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AmpVSTAudioProcessor::createEditor()
{
    return new AmpVSTAudioProcessorEditor (*this);
}

//==============================================================================
void AmpVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AmpVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new AmpVSTAudioProcessor();
}

