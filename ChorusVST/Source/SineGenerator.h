/*
  ==============================================================================

    SineGenerator.h
    Created: 9 Dec 2020 5:10:56pm
    Author:  user

  ==============================================================================
*/

#pragma once
#include <iostream>
#include <cmath>

using namespace std;

class SineGenerator {
public:
    // constructor and destructor
    SineGenerator();
    SineGenerator(float sr);
    SineGenerator(float sr, float freq, float amp);
    SineGenerator(float sr, float freq, float amp, float phs);
    ~SineGenerator();

    void setFrequency(float f);
    void setAmplitude(float a);
    void setPhase(float p);
    float getNextSample();
    void setSampleRate(float s);
    void prepare(float sr);
    void prepare(float sr, float freq, float amp);
    void prepare(float sr, float freq, float amp, float phs);
private:
    float sampleRate;
    float frequency;
    float amplitude;
    float phase;

    float current_angle;
    float current_sample_value;

    float pi = 3.14159265358979323846;
};
