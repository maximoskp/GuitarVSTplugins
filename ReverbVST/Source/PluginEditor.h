/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VisualStyle.h"

//==============================================================================
/**
*/
class ReverbVSTAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    ReverbVSTAudioProcessorEditor (ReverbVSTAudioProcessor&);
    ~ReverbVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ReverbVSTAudioProcessor& audioProcessor;
    
    juce::Slider reverbRoomSlider, reverbDampSlider, reverbWetSlider, reverbDrySlider;
    
    // --- Attachments (ORDER MATTERS: after UI members) ---
    std::unique_ptr<SliderAttachment> roomsizeAttach;
    std::unique_ptr<SliderAttachment> dampingAttach;
    std::unique_ptr<SliderAttachment> wetAttach;
    std::unique_ptr<SliderAttachment> dryAttach;
    
    juce::Rectangle<float> revBox;
    
    void configureSlider (juce::Slider& s, const juce::String& suffix);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbVSTAudioProcessorEditor)
};
