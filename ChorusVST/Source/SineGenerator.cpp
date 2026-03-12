/*
  ==============================================================================

    SineGenerator.cpp
    Created: 9 Dec 2020 5:10:56pm
    Author:  user

  ==============================================================================
*/

#include "SineGenerator.h"

// constructor and destructor
SineGenerator::SineGenerator() {
    // default constructor
    sampleRate = 44100.;
    frequency = 440.;
    amplitude = 0.7;
    phase = 0.f;
    current_angle = 0.;
}
SineGenerator::SineGenerator(float sr) {
    // default constructor
    sampleRate = sr;
    frequency = 440.;
    amplitude = 0.7;
    phase = 0.f;
    current_angle = 0.;
}
SineGenerator::SineGenerator(float sr, float freq, float amp) {
    // default constructor
    sampleRate = sr;
    frequency = freq;
    amplitude = amp;
    phase = 0.f;
    current_angle = 0.;
}
SineGenerator::SineGenerator(float sr, float freq, float amp, float phs) {
    // default constructor
    sampleRate = sr;
    frequency = freq;
    amplitude = amp;
    phase = phs;
    current_angle = phase;
}
SineGenerator::~SineGenerator() {
    // do nothing...
}

// public functions for setting private variables
void SineGenerator::setFrequency(float f) {
    frequency = f;
}
void SineGenerator::setAmplitude(float a) {
    amplitude = a;
}
void SineGenerator::setPhase(float p) {
    phase = p;
    current_angle = phase;
}
float SineGenerator::getNextSample() {
    current_sample_value = amplitude * std::sin(current_angle);
    current_angle += 2.0 * pi * frequency / sampleRate;
    // since we don't have infinite precision in pi, we need to keep 
    // the angle value within one turn of the unit circle
    while (current_angle > 2. * pi) {
        current_angle -= 2. * pi;
    }
    return current_sample_value;
}
void SineGenerator::setSampleRate(float s) {
    sampleRate = s;
}

void SineGenerator::prepare(float sr) {
    // default constructor
    sampleRate = sr;
    frequency = 440.;
    amplitude = 0.7;
    phase = 0.f;
    current_angle = 0.;
}
void SineGenerator::prepare(float sr, float freq, float amp) {
    // default constructor
    sampleRate = sr;
    frequency = freq;
    amplitude = amp;
    phase = 0.f;
    current_angle = 0.;
}
void SineGenerator::prepare(float sr, float freq, float amp, float phs) {
    // default constructor
    sampleRate = sr;
    frequency = freq;
    amplitude = amp;
    phase = phs;
    current_angle = phase;
}
