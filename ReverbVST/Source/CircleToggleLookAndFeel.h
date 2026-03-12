/*
  ==============================================================================

    CircleToggleLookAndFeel.h
    Created: 5 Dec 2025 5:33:12pm
    Author:  Maximos Kaliakatsos-Papakostas

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

struct CircleToggleLookAndFeel : juce::LookAndFeel_V4
{
    enum ColourIds
    {
        circleFillColourId      = 0x2000100, // for ON fill
        circleBorderColourId    = 0x2000101, // border
        backgroundColourId      = 0x2000102  // OFF background
    };
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& b,
                           bool, bool) override
    {
        auto r = b.getLocalBounds().toFloat().reduced(2.0f);
        auto diameter = juce::jmin(r.getWidth(), r.getHeight());
        auto circle = r.withSizeKeepingCentre(diameter, diameter);
        
        auto bg     = b.findColour(backgroundColourId);
        auto border = b.findColour(circleBorderColourId);
        auto fill   = b.findColour(circleFillColourId);
        // Fallbacks if colour not set
        if (bg.isTransparent())     bg     = juce::Colours::darkgrey;
        if (border.isTransparent()) border = juce::Colours::black.withAlpha(0.35f);
        if (fill.isTransparent())   fill   = juce::Colours::lightgreen;
        
        // off background
        g.setColour(bg);
        g.fillEllipse (circle);
        
        // Border
        g.setColour(border);
        g.drawEllipse (circle, 1.0f);
        
        // If ON, draw inner circle
        if (b.getToggleState())
        {
            auto inner = circle.reduced(diameter * 0.1f);
            g.setColour(fill);
            g.fillEllipse(inner);
        }
    }
};
