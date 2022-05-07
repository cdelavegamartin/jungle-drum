#include "Excitation.h"

#include <cmath>

Excitation::Excitation(float sampleRate, float amplitude, float lengthMs) {
  setup(sampleRate, amplitude, lengthMs);
}

void Excitation::setup(float sampleRate, float amplitude, float lengthMs) {
  sampleRate_ = sampleRate;
  lengthMsNew_ = lengthMs;
  amplitude_ = amplitude;
  trigger();
}
// Accesors
void Excitation::setLengthMs(float l) { lengthMsNew_ = l; }

float Excitation::getLengthMs() { return lengthMs_; }

void Excitation::setAmplitude(float amp) { amplitude_ = amp; }

float Excitation::getAmplitude() { return amplitude_; }

// Helper function
void Excitation::calculate_internal_parameters_() {
  lengthSamples_ = (int)(0.001 * lengthMs_ * sampleRate_);
  step_ = 1.0f / (float)(lengthSamples_);
  readPosition_ = 0.0;
}
// Triggers the object, reading the new parameters and resetting the index
void Excitation::trigger() {
  lengthMs_ = lengthMsNew_;
  calculate_internal_parameters_();
  readPosition_ = 0.0;
}

// Output is a raised cosine, the length of the impulse can be used to change
// the frequency content
float Excitation::process() {
  float out = 0.0;
  if (readPosition_ < 1.0) {
    out = amplitude_ * 0.5f *
          (1.0f + cosf((float)M_PI * (2.0f * readPosition_ - 1.0f)));

    readPosition_ += step_;
  }

  return out;
}