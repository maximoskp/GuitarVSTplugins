/*
  ==============================================================================

    Chorus.h
    Created: 18 Dec 2021 10:27:58pm
    Author:  Max

  ==============================================================================
*/

#pragma once
#include <iostream>
#include <cmath>
#include "DelayClasses.h"
#include "SineGenerator.h"
// #include <JuceHeader.h>

using namespace std;

class Chorus{
public:
    Chorus();
    Chorus(float sr);
    Chorus(float sr, float d, float s);
    Chorus(float sr, float d, float s, float p);
    ~Chorus();
    
    void setSampleRate(int sr);
    void setDepth(float d);
    void setSpeed(float s);
    void setPhase(float p);
    void setFeedback(float f);
    void setOnOff(bool on);
    float process_sample(float s);
    void prepare(float sr);
    void prepare(float sr, float d, float s, bool on);
    void prepare(float sr, float d, float s, float p, bool on);
    
private:
    float sample_rate;
    float depth;
    float speed;
    float phase;
    float feedback;
    bool onoff;
    float depth_multiplier;
    
    float center_delay_time;
    
    MonoDelay *delay;
    SineGenerator *sine;
};
