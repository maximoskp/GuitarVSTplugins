/*
  ==============================================================================

    DelayClass.cpp
    Created: 9 Nov 2021 7:33:50am
    Author:  Max

  ==============================================================================
*/

#include "DelayClasses.h"

# pragma mark - MonoDelay
MonoDelay::MonoDelay()
{
    sample_rate = 44100;
    // initialise circular buffer to zeros
    for(int i=0; i<max_delay_samples; i++){
        circular_buffer[i] = 0.;
    }
}
MonoDelay::MonoDelay(float sr)
{
    sample_rate = sr;
    // initialise circular buffer to zeros
    for(int i=0; i<max_delay_samples; i++){
        circular_buffer[i] = 0.;
    }
}
MonoDelay::MonoDelay(const MonoDelay &d){
    max_delay_samples = d.get_max_delay_samples();
    for(int i=0; i<max_delay_samples; i++){
        circular_buffer[i] = 0.;
    }
    read_pointer = d.get_read_pointer();
    write_pointer = d.get_write_pointer();
    delay_time = d.get_delay_time();
    delay_samples_target = d.get_delay_samples_target();
    delay_samples_current = d.get_delay_samples_current();
    delay_samples_current_int = d.get_delay_samples_current_int();
    sample_rate = d.get_sample_rate();
    feedback = d.get_feedback();
    dry = d.get_dry();
    wet = d.get_wet();
}
MonoDelay::~MonoDelay()
{
}

int MonoDelay::get_max_delay_samples() const{
    return max_delay_samples;
}
int MonoDelay::get_read_pointer() const{
    return read_pointer;
}
int MonoDelay::get_write_pointer() const{
    return write_pointer;
}
float MonoDelay::get_delay_time() const{
    return delay_time;
}
int MonoDelay::get_delay_samples_target() const{
    return delay_samples_target;
}
float MonoDelay::get_delay_samples_current() const{
    return delay_samples_current;
}
int MonoDelay::get_delay_samples_current_int() const{
    return delay_samples_current_int;
}
float MonoDelay::get_sample_rate() const{
    return sample_rate;
}
float MonoDelay::get_feedback() const{
    return feedback;
}
float MonoDelay::get_dry() const{
    return dry;
}
float MonoDelay::get_wet() const{
    return wet;
}
float MonoDelay::get_dtime_change_speed() const{
    return dtime_change_speed;
}

MonoDelay& MonoDelay::operator=(const MonoDelay* d){
    max_delay_samples = d->get_max_delay_samples();
    for(int i=0; i<max_delay_samples; i++){
        circular_buffer[i] = 0.;
    }
    read_pointer = d->get_read_pointer();
    write_pointer = d->get_write_pointer();
    delay_time = d->get_delay_time();
    delay_samples_target = d->get_delay_samples_target();
    delay_samples_current = d->get_delay_samples_current();
    delay_samples_current_int = d->get_delay_samples_current_int();
    sample_rate = d->get_sample_rate();
    feedback = d->get_feedback();
    dry = d->get_dry();
    wet = d->get_wet();
    return *this;
}

float MonoDelay::process_sample(float s){
    // adjust delay time
    if(delay_samples_current < (float)delay_samples_target-1){
        delay_samples_current = delay_samples_current + dtime_change_speed;
    }else if(delay_samples_current > (float)delay_samples_target+1){
        delay_samples_current = delay_samples_current - dtime_change_speed;
    }
    delay_samples_current_int = (int)delay_samples_current;
    // write to circular buffer
    read_pointer = write_pointer - delay_samples_current_int < 0 ? max_delay_samples + write_pointer - delay_samples_current_int : write_pointer - delay_samples_current_int ;
    circular_buffer[ write_pointer ] = s + feedback*circular_buffer[ read_pointer ];
    write_pointer++;
    write_pointer = write_pointer%max_delay_samples;
    return dry*s + wet*circular_buffer[ read_pointer ];
}

void MonoDelay::setSampleRate(float sr){
    sample_rate = sr;
}
void MonoDelay::setDelayTime(float d){
    delay_time = d;
    delay_samples_target = (int)(delay_time*(float)sample_rate);
}
void MonoDelay::setFeedback(float f){
    feedback = f;
}
void MonoDelay::setWetDry(float w, float d){
    wet = w;
    dry = d;
}
void MonoDelay::set_dtime_change_speed(float s){
    dtime_change_speed = s;
}

# pragma mark - MonoLPFDelay

MonoLPFDelay::MonoLPFDelay()
{
    sample_rate = 44100;
    // initialise circular buffer to zeros
    for(int i=0; i<max_delay_samples; i++){
        circular_buffer[i] = 0.;
    }
    lpf = new LowPassFilter(sample_rate, frequency, q);
}
MonoLPFDelay::MonoLPFDelay(float sr)
{
    sample_rate = sr;
    // initialise circular buffer to zeros
    for(int i=0; i<max_delay_samples; i++){
        circular_buffer[i] = 0.;
    }
    lpf = new LowPassFilter(sample_rate, frequency, q);
}
MonoLPFDelay::~MonoLPFDelay()
{
    lpf -> ~LowPassFilter();
}

float MonoLPFDelay::process_sample(float s){
    // adjust delay time
    if(delay_samples_current < (float)delay_samples_target-1){
        delay_samples_current = delay_samples_current + dtime_change_speed;
    }else if(delay_samples_current > (float)delay_samples_target+1){
        delay_samples_current = delay_samples_current - dtime_change_speed;
    }
    delay_samples_current_int = (int)delay_samples_current;
    // write to circular buffer
    read_pointer = write_pointer - delay_samples_current_int < 0 ? max_delay_samples + write_pointer - delay_samples_current_int : write_pointer - delay_samples_current_int ;
    circular_buffer[ write_pointer ] = lpf->process_sample( s + feedback*circular_buffer[ read_pointer ] );
    write_pointer++;
    write_pointer = write_pointer%max_delay_samples;
    return dry*s + wet*circular_buffer[ read_pointer ];
}

void MonoLPFDelay::setSampleRate(float sr){
    sample_rate = sr;
}
void MonoLPFDelay::setDelayTime(float d){
    delay_time = d;
    delay_samples_target = (int)(delay_time*(float)sample_rate);
}
void MonoLPFDelay::setFeedback(float f){
    feedback = f;
}
void MonoLPFDelay::setWetDry(float w, float d){
    wet = w;
    dry = d;
}
void MonoLPFDelay::setLPFfrequency(float f){
    frequency = f;
    lpf->set_frequency(frequency);
}
void MonoLPFDelay::setLPFq(float q_in){
    q = q_in;
    lpf->set_q(q);
}
void MonoLPFDelay::set_dtime_change_speed(float s){
    dtime_change_speed = s;
}


# pragma mark - MonoHLPFDelay

MonoHLPFDelay::MonoHLPFDelay()
{
    sample_rate = 44100;
    low_frequency = 500.f;
    low_q = 1.f;
    high_frequency = 500.f;
    high_q = 1.f;
    // initialise circular buffer to zeros
    for(int i=0; i<max_delay_samples; i++){
        circular_buffer[i] = 0.;
    }
//    lpf = std::make_unique<LowPassFilter>();
//    hpf = std::make_unique<HighPassFilter>();
    lpf.prepare(sample_rate, low_frequency, low_q);
    hpf.prepare(sample_rate, high_frequency, high_q);
}
MonoHLPFDelay::MonoHLPFDelay(float sr)
{
    sample_rate = sr;
    // initialise circular buffer to zeros
    for(int i=0; i<max_delay_samples; i++){
        circular_buffer[i] = 0.;
    }
//    lpf = std::make_unique<LowPassFilter>();
//    hpf = std::make_unique<HighPassFilter>();
    lpf.prepare(sample_rate, low_frequency, low_q);
    hpf.prepare(sample_rate, high_frequency, high_q);
}
MonoHLPFDelay::~MonoHLPFDelay()
{
    lpf.~LowPassFilter();
    hpf.~HighPassFilter();
}

float MonoHLPFDelay::process_sample(float s){
    // adjust delay time
    if(delay_samples_current < (float)delay_samples_target-1){
        delay_samples_current = delay_samples_current + dtime_change_speed;
    }else if(delay_samples_current > (float)delay_samples_target+1){
        delay_samples_current = delay_samples_current - dtime_change_speed;
    }
    delay_samples_current_int = (int)delay_samples_current;
    // write to circular buffer
    read_pointer = write_pointer - delay_samples_current_int < 0 ? max_delay_samples + write_pointer - delay_samples_current_int : write_pointer - delay_samples_current_int ;
    circular_buffer[ write_pointer ] = hpf.process_sample( lpf.process_sample( s + feedback*circular_buffer[ read_pointer ] ) );
    write_pointer++;
    write_pointer = write_pointer%max_delay_samples;
    return dry*s + wet*circular_buffer[ read_pointer ];
}

void MonoHLPFDelay::setSampleRate(float sr){
    sample_rate = sr;
    lpf.prepare(sample_rate, low_frequency, low_q);
    hpf.prepare(sample_rate, high_frequency, high_q);
}
void MonoHLPFDelay::setDelayTime(float d){
    delay_time = d;
    delay_samples_target = (int)(delay_time*(float)sample_rate);
}
void MonoHLPFDelay::setFeedback(float f){
    feedback = f;
}
void MonoHLPFDelay::setWetDry(float w, float d){
    wet = w;
    dry = d;
}
void MonoHLPFDelay::setLPFfrequency(float f){
    low_frequency = f;
    lpf.set_frequency(low_frequency);
}
void MonoHLPFDelay::setLPFq(float q_in){
    low_q = q_in;
    lpf.set_q(low_q);
}
void MonoHLPFDelay::setHPFfrequency(float f){
    high_frequency = f;
    hpf.set_frequency(high_frequency);
}
void MonoHLPFDelay::setHPFq(float q_in){
    high_q = q_in;
    hpf.set_q(high_q);
}
void MonoHLPFDelay::set_dtime_change_speed(float s){
    dtime_change_speed = s;
}
