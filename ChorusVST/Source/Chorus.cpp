/*
  ==============================================================================

    Chorus.cpp
    Created: 18 Dec 2021 10:27:58pm
    Author:  Max

  ==============================================================================
*/

#include "Chorus.h"

Chorus::Chorus(){
    sample_rate = 44100;
    depth = 0.003;
    speed = 3.;
    speed_target = speed;
    speed_delta = 2./sample_rate;
    phase = 0.f;
    feedback = 0.f;
    onoff = true;
    depth_multiplier = 0.003;
    
    center_delay_time =  2.*depth + 0.001;
    
    delay = new MonoDelay( sample_rate );
    delay->setDelayTime( center_delay_time );
    delay->setFeedback(0.);
    delay->setWetDry(1., 0.);
    sine = new SineGenerator( sample_rate, speed, depth );
}
Chorus::Chorus(float sr){
    sample_rate = sr;
    depth = 0.003;
    speed = 3.;
    speed_target = speed;
    speed_delta = 2./sample_rate;
    phase = 0.f;
    feedback = 0.f;
    onoff = true;
    depth_multiplier = 0.003;
    
    center_delay_time =  2.*depth + 0.001;
    
    delay = new MonoDelay( sample_rate );
    delay->setDelayTime( center_delay_time );
    delay->setFeedback(0.);
    delay->setWetDry(1., 0.);
    sine = new SineGenerator( sample_rate, speed, depth );
}
Chorus::Chorus(float sr, float d, float s){
    sample_rate = sr;
    depth = d;
    speed = s;
    speed_target = speed;
    speed_delta = 2./sample_rate;
    phase = 0.f;
    feedback = 0.f;
    onoff = true;
    depth_multiplier = 0.003;
    
    center_delay_time =  2.*depth + 0.001;
    
    delay = new MonoDelay( sample_rate );
    delay->setDelayTime( center_delay_time );
    delay->setFeedback(0.);
    delay->setWetDry(1., 0.);
    sine = new SineGenerator( sample_rate, speed, depth );
}
Chorus::Chorus(float sr, float d, float s, float p){
    sample_rate = sr;
    depth = d;
    speed = s;
    speed_target = speed;
    speed_delta = 2./sample_rate;
    phase = p;
    feedback = 0.f;
    onoff = true;
    depth_multiplier = 0.003;
    
    center_delay_time =  2.*depth + 0.001;
    
    delay = new MonoDelay( sample_rate );
    delay->setDelayTime( center_delay_time );
    delay->setFeedback(0.);
    delay->setWetDry(1., 0.);
    sine = new SineGenerator( sample_rate, speed, depth, phase );
}
Chorus::~Chorus(){
    
}

void Chorus::setSampleRate(int sr){
    sample_rate = sr;
    speed_delta = 2./sample_rate;
}
void Chorus::setDepth(float d){
    depth = depth_multiplier*d;
    center_delay_time =  2.*depth + 0.001;
    sine->setAmplitude(depth);
}
void Chorus::setSpeed(float s){
    speed_target = s;
//    speed = s;
//    sine->setFrequency(s);
}
void Chorus::setPhase(float p){
    phase = p;
    sine->setPhase(p);
}
void Chorus::setFeedback(float f){
    feedback = f;
    delay->setFeedback(f);
}
void Chorus::setOnOff(bool on){
    onoff = on;
}
float Chorus::getSpeed(){
    return speed;
}
float Chorus::process_sample(float s){
    if (abs(speed_target - speed) > 2*speed_delta){
        float speed_step = abs(speed_target - speed) > 1.0 ? abs(speed_target - speed)/sample_rate : speed_delta;
        speed += speed_target > speed ? speed_step : -speed_step;
    }
    sine->setFrequency(speed);
    float sine_sample = sine->getNextSample();
    delay->setDelayTime( center_delay_time + sine_sample );
    return s + float(onoff)*(1 - depth + s)*delay->process_sample(s);
}

void Chorus::prepare(float sr){
    sample_rate = sr;
    depth = 0.003;
    speed = 3.;
    speed_target = speed;
    speed_delta = 2./sample_rate;
    phase = 0.f;
    feedback = 0.f;
    onoff = true;
    
    center_delay_time =  2.*depth + 0.001;
    
    delay->setSampleRate(sr);
    delay->setDelayTime( center_delay_time );
    delay->setFeedback(0.);
    delay->setWetDry(1., 0.);
    sine->prepare( sample_rate, speed, depth );
}
void Chorus::prepare(float sr, float d, float s, bool on){
    sample_rate = sr;
    depth = d*depth_multiplier;
    speed = s;
    speed_target = speed;
    speed_delta = 2./sample_rate;
    phase = 0.f;
    feedback = 0.f;
    onoff = on;
    
    center_delay_time =  2.*depth + 0.001;
    
    delay->setSampleRate(sr);
    delay->setDelayTime( center_delay_time );
    delay->setFeedback(0.);
    delay->setWetDry(1., 0.);
    sine->prepare( sample_rate, speed, depth );
}
void Chorus::prepare(float sr, float d, float s, float p, bool on){
    sample_rate = sr;
    depth = d*depth_multiplier;
    speed = s;
    speed_target = speed;
    speed_delta = 2./sample_rate;
    phase = p;
    feedback = 0.f;
    onoff = on;
    
    center_delay_time =  2.*depth + 0.001;
    
    delay->setSampleRate(sr);
    delay->setDelayTime( center_delay_time );
    delay->setFeedback(0.);
    delay->setWetDry(1., 0.);
    sine->prepare( sample_rate, speed, depth, phase );
}
