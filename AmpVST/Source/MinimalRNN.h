/***** MinimalRNN.h *****/
#pragma once
#include <cmath>
#include <cstring>
#include <cassert>

class MinimalRNN {
public:
    MinimalRNN(int input_size,
               int hidden_size,
               const float* W_ih_,   // (H, I) row-major
               const float* W_hh_,   // (H, H) row-major
               const float* b_ih_,   // (H)
               const float* b_hh_,   // (H)
               const float* W_out_,  // (H)   (since output size = 1)
               const float* b_out_)  // (1)
    : I(input_size), H(hidden_size),
      W_ih(W_ih_), W_hh(W_hh_), b_ih(b_ih_), b_hh(b_hh_), W_out(W_out_), b_out(b_out_)
    {
        assert(I > 0 && H > 0);
        assert(W_ih && W_hh && b_ih && b_hh && W_out && b_out);

        h = new float[H];
        tmp = new float[H];      // tmp pre-activation vector
        resetState();
    }

    ~MinimalRNN() {
        delete[] h;
        delete[] tmp;
    }

    void resetState() {
        std::memset(h, 0, sizeof(float) * H);
    }

    // x is a scalar input if I==1; otherwise pass a pointer to I floats.
    // Returns scalar output (post linear layer).
    inline float processSample(const float* x_ptr) {
        // tmp = W_ih * x + W_hh * h + b_ih + b_hh
        for (int r = 0; r < H; ++r) {
            float sum = b_ih[r] + b_hh[r];

            // input contribution
            const float* wih = &W_ih[r * I];
            float acc_in = 0.0f;
            for (int i = 0; i < I; ++i) acc_in += wih[i] * x_ptr[i];

            // recurrent contribution
            const float* whh = &W_hh[r * H];
            float acc_h = 0.0f;
            for (int j = 0; j < H; ++j) acc_h += whh[j] * h[j];

            tmp[r] = sum + acc_in + acc_h;
        }

        // h = tanh(tmp)
        for (int r = 0; r < H; ++r) {
            h[r] = std::tanh(tmp[r]);
        }

        // y = W_out * h + b_out
        float y = b_out[0];
        for (int j = 0; j < H; ++j) {
            y += W_out[j] * h[j];
        }
        return y;
    }

    // Convenience for I==1
    inline float processSampleScalar(float x) {
        scalar_in[0] = x;
        return processSample(scalar_in);
    }

private:
    int I, H;
    const float* W_ih;
    const float* W_hh;
    const float* b_ih;
    const float* b_hh;
    const float* W_out;  // (H)
    const float* b_out;  // (1)

    float* h;    // hidden state (owned)
    float* tmp;  // pre-activation (owned)

    float scalar_in_storage[1];
    float* scalar_in = scalar_in_storage; // used when I==1
};
