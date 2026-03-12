/*
  ==============================================================================

    DistortionClasses.h
    Created: 9 Nov 2021 11:36:19pm
    Author:  Max

  ==============================================================================
*/

#pragma once

#include <cmath>

class ClippingDistortion
{
public:
    ClippingDistortion();
    ClippingDistortion(float d);
    ~ClippingDistortion();
    
    void setDistortionRate(float d);
    
    float process_sample(float s);
private:
    float dist_rate = 0.5;
    float clipping_limit = 0.5;
    float amp;
};

/*
TODO: make TanhDistortion class
 */

class TanhDistortion
{
public:
    TanhDistortion();
    TanhDistortion(float d);
    ~TanhDistortion();
    
    void setDistortionRate(float d);
    
    float process_sample(float s);
private:
    float max_preamp = 50.;
    float min_preamp = 2.;
    float pre_amp = 2.;
};
