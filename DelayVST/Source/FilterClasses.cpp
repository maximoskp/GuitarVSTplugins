/*
  ==============================================================================

    FilterClasses.cpp
    Created: 12 Nov 2021 10:11:21pm
    Author:  Max

  ==============================================================================
*/

#include "FilterClasses.h"

LowPassFilter::LowPassFilter(){
    x1 = 0.;
    x2 = 0.;
    y1 = 0.;
    y2 = 0.;
    a1_current = 0.;
    a2_current = 0.;
    b0_current = 1.;
    b1_current = 0.;
    b2_current = 0.;
    a1_target = 0.;
    a2_target = 0.;
    b0_target = 1.;
    b1_target = 0.;
    b2_target = 0.;
    frequency = 500.f;
    q = 1.f;
    sample_rate = 44100.f;
    calculate_coefficients();
}
LowPassFilter::LowPassFilter(float sr, float f, float q_in){
    x1 = 0.;
    x2 = 0.;
    y1 = 0.;
    y2 = 0.;
    a1_current = 0.;
    a2_current = 0.;
    b0_current = 1.;
    b1_current = 0.;
    b2_current = 0.;
    a1_target = 0.;
    a2_target = 0.;
    b0_target = 1.;
    b1_target = 0.;
    b2_target = 0.;
    frequency = f;
    q = q_in;
    sample_rate = sr;
    calculate_coefficients();
}
LowPassFilter::~LowPassFilter(){
    
}
void LowPassFilter::calculate_coefficients()
{
    float k = tanf(M_PI * frequency / sample_rate);
    float norm = 1.0 / (1 + k / q + k * k);
    
    b0_target = k * k * norm;
    b1_target = 2.0 * b0_target;
    b2_target = b0_target;
    a1_target = 2 * (k * k - 1) * norm;
    a2_target = (1 - k / q + k * k) * norm;
}

float LowPassFilter::process_sample(float s){
    b0_current += interp * (b0_target - b0_current);
    b1_current += interp * (b1_target - b1_current);
    b2_current += interp * (b2_target - b2_current);
    a1_current += interp * (a1_target - a1_current);
    a2_current += interp * (a2_target - a2_current);
    float out = b0_current*s + b1_current*x1 + b2_current*x2 - a1_current*y1 - a2_current*y2;
    x2 = x1;
    x1 = s;
    y2 = y1;
    y1 = out;
    return out;
}

void LowPassFilter::set_frequency(float f){
    frequency = f;
    calculate_coefficients();
}
void LowPassFilter::set_q(float q_in){
    q = q_in;
    calculate_coefficients();
}
void LowPassFilter::set_sample_rate(float sr){
    sample_rate = sr;
    calculate_coefficients();
}
void LowPassFilter::prepare(float sr, float f, float q_in){
    x1 = 0.;
    x2 = 0.;
    y1 = 0.;
    y2 = 0.;
    a1_current = 0.;
    a2_current = 0.;
    b0_current = 1.;
    b1_current = 0.;
    b2_current = 0.;
    a1_target = 0.;
    a2_target = 0.;
    b0_target = 1.;
    b1_target = 0.;
    b2_target = 0.;
    frequency = f;
    q = q_in;
    sample_rate = sr;
    calculate_coefficients();
}
float LowPassFilter::get_frequency(){
    return frequency;
}
float LowPassFilter::get_q(){
    return q;
}
float LowPassFilter::get_sample_rate(){
    return sample_rate;
}


HighPassFilter::HighPassFilter(){
    x1 = 0.;
    x2 = 0.;
    y1 = 0.;
    y2 = 0.;
    a1_current = 0.;
    a2_current = 0.;
    b0_current = 1.;
    b1_current = 0.;
    b2_current = 0.;
    a1_target = 0.;
    a2_target = 0.;
    b0_target = 1.;
    b1_target = 0.;
    b2_target = 0.;
    frequency = 500.f;
    q = 1.f;
    sample_rate = 44100.f;
    calculate_coefficients();
}
HighPassFilter::HighPassFilter(float sr, float f, float q_in){
    x1 = 0.;
    x2 = 0.;
    y1 = 0.;
    y2 = 0.;
    a1_current = 0.;
    a2_current = 0.;
    b0_current = 1.;
    b1_current = 0.;
    b2_current = 0.;
    a1_target = 0.;
    a2_target = 0.;
    b0_target = 1.;
    b1_target = 0.;
    b2_target = 0.;
    frequency = f;
    q = q_in;
    sample_rate = sr;
    calculate_coefficients();
}
HighPassFilter::~HighPassFilter(){
    
}
void HighPassFilter::calculate_coefficients()
{
    float omega = 2.0f * M_PI * frequency / sample_rate;
    float alpha = sinf(omega) / (2.0f * q);
    float cosw = cosf(omega);

    b0_target = (1.0f + cosw) / 2.0f;
    b1_target = -(1.0f + cosw);
    b2_target = (1.0f + cosw) / 2.0f;

    float a0 = 1.0f + alpha;
    a1_target = -2.0f * cosw;
    a2_target = 1.0f - alpha;

    // Normalize coefficients
    b0_target /= a0;
    b1_target /= a0;
    b2_target /= a0;
    a1_target /= a0;
    a2_target /= a0;
}

float HighPassFilter::process_sample(float s){
    b0_current += interp * (b0_target - b0_current);
    b1_current += interp * (b1_target - b1_current);
    b2_current += interp * (b2_target - b2_current);
    a1_current += interp * (a1_target - a1_current);
    a2_current += interp * (a2_target - a2_current);
    float out = b0_current * s + b1_current * x1 + b2_current * x2
                       - a1_current * y1 - a2_current * y2;
        // Shift states
        x2 = x1;
        x1 = s;
        y2 = y1;
        y1 = out;
    return out;
}

void HighPassFilter::set_frequency(float f){
    frequency = f;
    calculate_coefficients();
}
void HighPassFilter::set_q(float q_in){
    q = q_in;
    calculate_coefficients();
}
void HighPassFilter::set_sample_rate(float sr){
    sample_rate = sr;
    calculate_coefficients();
}
void HighPassFilter::prepare(float sr, float f, float q_in){
    x1 = 0.;
    x2 = 0.;
    y1 = 0.;
    y2 = 0.;
    a1_current = 0.;
    a2_current = 0.;
    b0_current = 1.;
    b1_current = 0.;
    b2_current = 0.;
    a1_target = 0.;
    a2_target = 0.;
    b0_target = 1.;
    b1_target = 0.;
    b2_target = 0.;
    frequency = f;
    q = q_in;
    sample_rate = sr;
    calculate_coefficients();
}

float HighPassFilter::get_frequency(){
    return frequency;
}
float HighPassFilter::get_q(){
    return q;
}
float HighPassFilter::get_sample_rate(){
    return sample_rate;
}
