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
class AmpVSTAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    
    AmpVSTAudioProcessorEditor (AmpVSTAudioProcessor&);
    ~AmpVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AmpVSTAudioProcessor& audioProcessor;
    
    juce::Rectangle<float> preBox, postBox, centerBox;
    
    CircleToggleLookAndFeel circleLAF;
    
    juce::Slider preHiPSlider, preLoPSlider, gainSlider, postHiPSlider, postLoPSlider;
    juce::ToggleButton preHiP_onoff, preLoP_onoff, dist_onoff, postHiP_onoff, postLoP_onoff;
    
    std::unique_ptr<SliderAttachment> preHiPAttach;
    std::unique_ptr<ButtonAttachment> preHiPOnAttach;
    
    std::unique_ptr<SliderAttachment> preLoPAttach;
    std::unique_ptr<ButtonAttachment> preLoPOnAttach;
    
    std::unique_ptr<SliderAttachment> gainAttach;
    std::unique_ptr<ButtonAttachment> distOnAttach;
    
    std::unique_ptr<SliderAttachment> postHiPAttach;
    std::unique_ptr<ButtonAttachment> postHiPOnAttach;
    
    std::unique_ptr<SliderAttachment> postLoPAttach;
    std::unique_ptr<ButtonAttachment> postLoPOnAttach;
    
    void configureSlider(juce::Slider& s, const juce::String& suffix);
    void configureToggle(juce::ToggleButton& b);
    
    void setCircleLAFforToggles(juce::LookAndFeel* laf);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpVSTAudioProcessorEditor)
};
