/*
  ==============================================================================

    AmpClasses.cpp
    Created: 25 Oct 2025 8:22:58am
    Author:  Maximos Kaliakatsos-Papakostas

  ==============================================================================
*/

#include "AmpClasses.h"


PrePostAmp::PrePostAmp(){
    sample_rate = 44100;
    pre_highf = 80;
    pre_lowf = 5000;
    post_highf = 20;
    post_lowf = 5000;
    gain = 0.1;
    
    pre_highf_on = true;
    pre_lowf_on = false;
    dist_onoff = false;
    post_highf_on = false;
    post_lowf_on = false;
    
    tanhDist.setDistortionRate(gain);
    preHiP.prepare(sample_rate, pre_highf, 1.);
    preLoP.prepare(sample_rate, pre_lowf, 1.);
    postHiP.prepare(sample_rate, post_highf, 1.);
    postLoP.prepare(sample_rate, post_lowf, 1.);
    
    gRnn = new MinimalRNN(
        RNN_INPUT_SIZE,
        RNN_HIDDEN_SIZE,
        rnn_W_ih,
        rnn_W_hh,
        rnn_b_ih,
        rnn_b_hh,
        rnn_W_out,
        rnn_b_out
    );
    gRnn->resetState();
}

void PrePostAmp::set_sample_rate(float sr){
    sample_rate = sr;
    preHiP.set_sample_rate(sample_rate);
    preLoP.set_sample_rate(sample_rate);
    postHiP.set_sample_rate(sample_rate);
    postLoP.set_sample_rate(sample_rate);
}
void PrePostAmp::set_pre_highf(float f){
    pre_highf = f;
    preHiP.set_frequency(pre_highf);
}
void PrePostAmp::set_pre_lowf(float f){
    pre_lowf = f;
    preLoP.set_frequency(post_lowf);
}
void PrePostAmp::set_post_highf(float f){
    post_highf = f;
    postHiP.set_frequency(post_highf);
}
void PrePostAmp::set_post_lowf(float f){
    post_lowf = f;
    postLoP.set_frequency(post_lowf);
}
void PrePostAmp::set_gain(float g){
    gain = g;
    tanhDist.setDistortionRate(gain);
}

void PrePostAmp::toggle_pre_high(bool b){
    pre_highf_on = b;
}
void PrePostAmp::toggle_pre_low(bool b){
    pre_lowf_on = b;
}
void PrePostAmp::toggle_dist(bool b){
    dist_onoff = b;
}
void PrePostAmp::toggle_post_high(bool b){
    post_highf_on = b;
}
void PrePostAmp::toggle_post_low(bool b){
    post_lowf_on = b;
}

void PrePostAmp::prepare_values(float sr, float pre_hf, float pre_lf, float post_hf, float post_lf, float g){
    sample_rate = sr;
    
    pre_highf = pre_hf;
    preHiP.set_sample_rate(sample_rate);
    preHiP.set_frequency(pre_highf);
    
    pre_lowf = pre_lf;
    preLoP.set_sample_rate(sample_rate);
    preLoP.set_frequency(pre_lowf);
    
    post_highf = post_hf;
    postHiP.set_sample_rate(sample_rate);
    postHiP.set_frequency(post_highf);
    
    post_lowf = post_lf;
    postLoP.set_sample_rate(sample_rate);
    postLoP.set_frequency(post_lowf);
    
    gain = g;
    tanhDist.setDistortionRate(gain);
}
void PrePostAmp::prepare_toggles(bool pre_hf, bool pre_lf, bool post_hf, bool post_lf){
    pre_highf_on = pre_hf;
    pre_lowf_on = pre_lf;
    post_highf_on = post_hf;
    post_lowf_on = post_lf;
}

float PrePostAmp::process_sample(float s){
    s = pre_highf_on ? preHiP.process_sample(s) : s;
    s = pre_lowf_on ? preLoP.process_sample(s) : s;
    s = tanhDist.process_sample(s);
    s = dist_onoff ? 0.5*gRnn->processSampleScalar(s) : s;
    s = post_highf_on ? postHiP.process_sample(s) : s;
    s = post_lowf_on ? postLoP.process_sample(s) : s;
    return s;
}


float PrePostAmp::get_sample_rate(){
    return sample_rate;
}
float PrePostAmp::get_pre_highf(){
    return pre_highf;
}
float PrePostAmp::get_pre_lowf(){
    return pre_lowf;
}
float PrePostAmp::get_post_highf(){
    return post_highf;
}
float PrePostAmp::get_post_lowf(){
    return post_lowf;
}
float PrePostAmp::get_gain(){
    return gain;
}

bool PrePostAmp::get_toggle_pre_high(){
    return pre_highf_on;
}
bool PrePostAmp::get_toggle_pre_low(){
    return pre_lowf_on;
}
bool PrePostAmp::get_toggle_dist(){
    return dist_onoff;
}
bool PrePostAmp::get_toggle_post_high(){
    return post_highf_on;
}
bool PrePostAmp::get_toggle_post_low(){
    return post_lowf_on;
}
