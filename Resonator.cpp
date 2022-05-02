// Resonator.cpp: file for implementing a 2nd Order resonating IIR filter
#include "Resonator.h"

#include <libraries/math_neon/math_neon.h>

#include <cmath>
#include <Bela.h>

// Constructor taking arguments for sample rate and table data
Resonator::Resonator(float sampleRate, float frequencyHz, float decay) {
  setup(sampleRate, frequencyHz, decay);
}

void Resonator::setup(float sampleRate, float frequencyHz, float decay) {
  // It's faster to multiply than to divide on most platforms,
  // therefore the inverse samplerate is stored
  inverseSampleRate_ = 1.0 / sampleRate;  // Audio sample rate
  frequencyHz_ = frequencyHz;             // Frequency of the resonator
  decay_ = decay;                         // Frequency of the resonator

  lastOutputSample_ = 0.0;
  lastlastOutputSample_ = 0.0;

  // Calculate the coefficients
  calculate_coefficients_();
}

// Set the resonator frequency
void Resonator::setFrequencyHz(float f) {
  frequencyHz_ = f;
  calculate_coefficients_();
}

// Get the resonator frequency
float Resonator::getFrequencyHz() { return frequencyHz_; }

// Set the resonator frequency
void Resonator::setDecay(float r) {
  decay_ = r;
  calculate_coefficients_();
}

// Get the resonator frequency
float Resonator::getDecay() { return decay_; }

// Calculate resonator coefficients
void Resonator::calculate_coefficients_() {
  float freqRel2 = 2.0 * frequencyHz_ * inverseSampleRate_;
  // rt_printf("freqRel2: %f \n", freqRel2);
  // if (freqRel2 < 1.0) {
  //   b0_ = 1.0;
  //   a1_ = -2.0 * decay_ * cosf(M_PI * freqRel2);
  //   a2_ = decay_ * decay_;
  // } else {
  //   b0_ = 0.0;
  //   a1_ = 0.0;
  //   a2_ = 0.0;
  // }

    b0_ = 1.0;
    a1_ = -2.0 * decay_ * cosf(M_PI * freqRel2);
    a2_ = decay_ * decay_;

}

// Process next sample and update stored previous values
float Resonator::process(float in) {
  // Calculate output sample
  float out = b0_ * in - a1_ * lastOutputSample_ - a2_ * lastlastOutputSample_;

  // Save last output
  lastlastOutputSample_ = lastOutputSample_;
  lastOutputSample_ = out;
  return out;
}