/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CircleToggleLookAndFeel.h"
#include "VisualStyle.h"

//==============================================================================
/**
*/
class ChorusVSTAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    
    ChorusVSTAudioProcessorEditor (ChorusVSTAudioProcessor&);
    ~ChorusVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ChorusVSTAudioProcessor& audioProcessor;
    
    juce::Rectangle<float> revBox;
    
    CircleToggleLookAndFeel circleLAF;
    
    juce::Slider slowSlider, fastSlider, depthSlider;
    juce::ToggleButton slow_fast_toggle, stereo_toggle;
    
    juce::Label slow_fastLabel, stereoLabel;
    
    std::unique_ptr<SliderAttachment> slowAttach;
    std::unique_ptr<SliderAttachment> fastAttach;
    std::unique_ptr<SliderAttachment> depthAttach;
    
    std::unique_ptr<ButtonAttachment> slow_fastAttach;
    std::unique_ptr<ButtonAttachment> stereoAttach;
    
    void configureSlider (juce::Slider& s, const juce::String& suffix);
    void configureToggle(juce::ToggleButton& b, bool is_open);
    
    void setCircleLAFforToggles(juce::LookAndFeel* laf);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusVSTAudioProcessorEditor)
};
