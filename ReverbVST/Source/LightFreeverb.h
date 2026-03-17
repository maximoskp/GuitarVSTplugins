/*
  ==============================================================================

    LightFreeverb.h
    Created: 28 Oct 2025 6:28:31pm
    Author:  Maximos Kaliakatsos-Papakostas

  ==============================================================================
*/

#pragma once
#include <array>
#include <vector>
#include <cmath>

class LightFreeverb
{
public:
    void prepare (double sampleRate)
    {
        const int combDelays[] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
        const int allpassDelays[] = {556, 441, 341, 225};

        for (size_t i = 0; i < combs.size(); ++i)
            combs[i].setDelay (combDelays[i]);

        for (size_t i = 0; i < allpasses.size(); ++i)
            allpasses[i].setDelay (allpassDelays[i]);
    }

    void setParameters (float roomSize, float damping, float wetLevel = 0.33f)
    {
        // Map roughly to Freeverb ranges
        for (auto& c : combs)
        {
            c.feedback = 0.7f + roomSize * 0.28f;
            c.setDamping (0.2f + damping * 0.7f);
        }
        wet = wetLevel;
    }
    
    void setRoomSize(float value){
        // Map roughly to Freeverb ranges
        for (auto& c : combs)
        {
            c.feedback = 0.7f + value * 0.28f;
        }
    }
    void setDamping(float value){
        // Map roughly to Freeverb ranges
        for (auto& c : combs)
        {
            c.setDamping (0.2f + value * 0.7f);
        }
    }
    void setWet(float value){
        wet = value;
    }
    void setDry(float value){
        dry = value;
    }
    void set_sampleRate(float value){
        float sample = value;
    }

    inline float process_sample (float input)
    {
        float input_copy = input;
        float output = 0.0f;
        for (auto& c : combs)
            output += c.process (input);
        for (auto& a : allpasses)
            output = a.process (output);
        return 0.3*output * wet*wet + dry*input_copy;
    }

    void reset()
    {
        for (auto& c : combs) c.reset();
        for (auto& a : allpasses) a.reset();
    }

private:
    struct Comb
    {
        std::vector<float> buffer;
        int idx = 0;
        float feedback = 0.8f;
        float damping = 0.5f;
        float filterstore = 0.0f;

        void setDelay (int samples)
        {
            buffer.assign (samples, 0.0f);
            idx = 0;
        }

        void setDamping (float d)
        {
            damping = std::clamp (d, 0.0f, 1.0f);
        }

        inline float process (float input)
        {
            if (buffer.empty()) // safety guard
                return 0.0f;
            float output = buffer[idx];
            filterstore = output * (1.0f - damping) + filterstore * damping;
            float v = input + filterstore * feedback;
            buffer[idx] = v;
            if (++idx >= (int)buffer.size()) idx = 0;
            return output;
        }

        void reset()
        {
            std::fill (buffer.begin(), buffer.end(), 0.0f);
            filterstore = 0.0f;
            idx = 0;
        }
    };

    struct Allpass
    {
        std::vector<float> buffer;
        int idx = 0;
        float feedback = 0.5f;

        void setDelay (int samples)
        {
            buffer.assign (samples, 0.0f);
            idx = 0;
        }

        inline float process (float input)
        {
            if (buffer.empty()) // safety guard
                return 0.0f;
            float bufout = buffer[idx];
            float output = -input + bufout;
            buffer[idx] = input + bufout * feedback;
            if (++idx >= (int)buffer.size()) idx = 0;
            return output;
        }

        void reset()
        {
            std::fill (buffer.begin(), buffer.end(), 0.0f);
            idx = 0;
        }
    };

    std::array<Comb, 8> combs;
    std::array<Allpass, 4> allpasses;
    float wet = 0.33f;
    float dry = 1.f;
};
