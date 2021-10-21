//
// Created by Jacob Wolfe
// 
// Simple Direct Form I 5-coefficient Biquadratic Parametric Peaking EQ
// 
// Refs: "The Equivalence of Various Methods of Computing Biquad Coefficients for Audio Parametric Equalizers" by Robert Bristow-Johnson, 
// and https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html

#ifndef ParametricEQ_h
#define ParametricEQ_h

#define _USE_MATH_DEFINES
#include "math.h"

#pragma once

class Parametric
{
public:

    Parametric() 
    { 
        reset();
    }

    ~Parametric() {}

    void reset()
    {
        xnL1 = xnL2 = ynL1 = ynL2 = xnR1 = xnR2 = ynR1 = ynR2 = b0 = b1 = b2 = a0 = a1 = a2 = gain = q = frequency = 0.0f;
    }

    void processBlock(float* block, int numFrames, bool isLeft)
    {
        // Support interleaved data.
        for (int n = 0; n < numFrames; ++n)
        {
            float sample = 0;
            if (isLeft)
            {
                sample = filterLeftSample(block[n]);
            }
            else
            {
                sample = filterRightSample(block[n]);
            }
            block[n] = sample;
        }
    }

    void calcCoeffs(float g, float freq, float quality, float sampRate)
    {
        q = quality;
        gain = g;
        frequency = freq;

        float A = powf(10.0f, (gain / 40.0f));
        float omega = 2.0f * M_PI * frequency / sampRate;
        float tsin = sinf(omega);
        float tcos = cosf(omega);
        float beta = sqrtf(A + A);
        float alpha = 0.0f;

        alpha = tsin / (2.0f * q);

        a0 = (1.0f + alpha / A);
        a1 = (-2.0f * tcos) / a0;
        a2 = (1.0f - alpha / A) / a0;

        b0 = (1.0f + alpha * A) / a0;
        b1 = (-2.0f * tcos) / a0;
        b2 = (1.0f - alpha * A) / a0;
    }

    float q;
    float gain;
    float frequency;

private:

    float filterLeftSample(float xL)
    {
        float yL0 = (b0 * xL + b1 * xnL1 + b2 * xnL2 - a1 * ynL1 - a2 * ynL2);

        xnL2 = xnL1;
        xnL1 = xL;

        ynL2 = ynL1;
        ynL1 = yL0;

        return yL0;
    }

    float filterRightSample(float xR)
    {
        float yR0 = (b0 * xR + b1 * xnR1 + b2 * xnR2 - a1 * ynR1 - a2 * ynR2);

        xnR2 = xnR1;
        xnR1 = xR;

        ynR2 = ynR1;
        ynR1 = yR0;

        return yR0;
    }


    float b0, b1, b2, a0, a1, a2, xnR1, xnR2, ynR1, ynR2, xnL1, xnL2, ynL1, ynL2, sampRate;
};

#endif /* ParametricEQ_h */