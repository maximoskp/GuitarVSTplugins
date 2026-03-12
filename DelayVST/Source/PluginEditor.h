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
class DelayVSTAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    DelayVSTAudioProcessorEditor (DelayVSTAudioProcessor&);
    ~DelayVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayVSTAudioProcessor& audioProcessor;
    
    juce::Rectangle<float> preBox, postBox, centerBox;
    
    juce::Slider hiPSlider, loPSlider, wetSlider, timeSlider, feedbackSlider;
    
    // --- Attachments (ORDER MATTERS: after UI members) ---
    std::unique_ptr<SliderAttachment> hiPAttach;
    std::unique_ptr<SliderAttachment> loPAttach;

    std::unique_ptr<SliderAttachment> wetAttach;
    std::unique_ptr<SliderAttachment> timeAttach;
    std::unique_ptr<SliderAttachment> feedbackAttach;
    
    void configureSlider (juce::Slider& s, const juce::String& suffix);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayVSTAudioProcessorEditor)
};
