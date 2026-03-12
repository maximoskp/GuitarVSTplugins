/*
  ==============================================================================

    FilterClasses.h
    Created: 12 Nov 2021 10:11:21pm
    Author:  Max

  ==============================================================================
*/

#pragma once
#include <cmath>
#include <iostream>

class LowPassFilter
{
public:
    LowPassFilter();
    LowPassFilter(float sample_rate, float f, float q_in);
    ~LowPassFilter();
    
    float process_sample(float s);
    
    void set_frequency(float f);
    void set_q(float q_in);
    void set_sample_rate(float sr);
    void prepare(float sample_rate, float f, float q_in);
    
    float get_frequency();
    float get_q();
    float get_sample_rate();
private:
    float frequency = 500;
    float q = 1;
    float sample_rate;
    
    // filter states
    float x1, x2, y1, y2;
    // filter coefficients
//    float b0, b1, b2, a1, a2;
    
    float b0_target, b1_target, b2_target;
    float a1_target, a2_target;

    float b0_current, b1_current, b2_current;
    float a1_current, a2_current;
    
    float interp = 0.001f;  // small smoothing factor
    
    void calculate_coefficients();
};

class HighPassFilter
{
public:
    HighPassFilter();
    HighPassFilter(float sample_rate, float f, float q_in);
    ~HighPassFilter();
    
    float process_sample(float s);
    
    void set_frequency(float f);
    void set_q(float q_in);
    void set_sample_rate(float sr);
    void prepare(float sample_rate, float f, float q_in);
    
    float get_frequency();
    float get_q();
    float get_sample_rate();
private:
    float frequency = 500;
    float q = 1;
    float sample_rate;
    
    // filter states
    float x1, x2, y1, y2;
    // filter coefficients
//    float b0, b1, b2, a1, a2;
    
    float b0_target, b1_target, b2_target;
    float a1_target, a2_target;

    float b0_current, b1_current, b2_current;
    float a1_current, a2_current;
    
    float interp = 0.001f;  // small smoothing factor
    
    void calculate_coefficients();
};
