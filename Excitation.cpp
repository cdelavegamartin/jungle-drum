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

void Excitation::setLengthMs(float l) { lengthMsNew_ = l; }

float Excitation::getLengthMs() { return lengthMs_; }

void Excitation::setAmplitude(float amp) { amplitude_ = amp; }

float Excitation::getAmplitude() { return amplitude_; }

void Excitation::calculate_internal_parameters_() {
  lengthSamples_ = (int)(0.001 * lengthMs_ * sampleRate_);
  step_ = 1.0f / (float)(lengthSamples_);
  readPosition_ = 0.0;
}

void Excitation::trigger() {
  lengthMs_ = lengthMsNew_;
  calculate_internal_parameters_();
  reset_read_pos_();
}

void Excitation::reset_read_pos_() { readPosition_ = 0.0; }

float Excitation::process() {
  float out = 0.0;
  if (readPosition_ < 1.0) {
    out = amplitude_ * 0.5f *
          (1.0f + cosf((float)M_PI * (2.0f * readPosition_ - 1.0f)));

    readPosition_ += step_;
  }

  return out;
}