/*
  ==============================================================================

    AmpClasses.h
    Created: 25 Oct 2025 8:22:58am
    Author:  Maximos Kaliakatsos-Papakostas

  ==============================================================================
*/

#pragma once
#include "DistortionClasses.h"
#include "FilterClasses.h"
#include <iostream>
#include "MinimalRNN.h"
#include "dist_rnn_weights.h"

class PrePostAmp
{
public:
    PrePostAmp();
    
    void set_sample_rate(float sr);
    void set_pre_highf(float f);
    void set_pre_lowf(float f);
    void set_post_highf(float f);
    void set_post_lowf(float f);
    void set_gain(float g);
    
    void toggle_pre_high(bool b);
    void toggle_pre_low(bool b);
    void toggle_dist(bool b);
    void toggle_post_high(bool b);
    void toggle_post_low(bool b);
    
    void prepare_values(float sr, float pre_hf, float pre_lf, float post_hf, float post_lf, float g);
    void prepare_toggles(bool pre_hf, bool pre_lf, bool post_hf, bool post_lf);
    
    float process_sample(float s);
    
    float get_sample_rate();
    float get_pre_highf();
    float get_pre_lowf();
    float get_post_highf();
    float get_post_lowf();
    float get_gain();
    
    bool get_toggle_pre_high();
    bool get_toggle_pre_low();
    bool get_toggle_dist();
    bool get_toggle_post_high();
    bool get_toggle_post_low();
    

private:
    float sample_rate;
    float pre_highf;
    float pre_lowf;
    float post_highf;
    float post_lowf;
    float gain;
    
    bool pre_highf_on;
    bool pre_lowf_on;
    bool dist_onoff;
    bool post_highf_on;
    bool post_lowf_on;
    
    TanhDistortion tanhDist;
    HighPassFilter preHiP;
    LowPassFilter preLoP;
    HighPassFilter postHiP;
    LowPassFilter postLoP;
    MinimalRNN* gRnn = nullptr;
};
