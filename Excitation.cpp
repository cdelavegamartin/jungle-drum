#include "Excitation.h"

#include <Bela.h>

#include <algorithm>
#include <cmath>

Excitation::Excitation(float sampleRate, float lengthMs, float expCoeff) {
  setup(sampleRate, lengthMs, expCoeff);
}

void Excitation::setup(float sampleRate, float lengthMs, float expCoeff) {
  sampleRate_ = sampleRate;
  lengthMsNew_ = lengthMs;
  expCoeffNew_ = expCoeff;
  trigger();
}

void Excitation::setLengthMs(float l) { lengthMsNew_ = l; }

float Excitation::getLengthMs() { return lengthMs_; }

void Excitation::setExponentialCoefficient(float coeff) {
  expCoeffNew_ = coeff;
}

float Excitation::getExponentialCoefficient() { return expCoeff_; }

void Excitation::setAmplitude(float amp) { amplitude_ = amp; }

float Excitation::getAmplitude() { return amplitude_; }

void Excitation::calculate_internal_parameters_() {
  lengthSamples_ = (int)(0.001 * lengthMs_ * sampleRate_);
  step_ = 1.0f / (float)(lengthSamples_);
  readPosition_ = 0.0;
}

void Excitation::trigger() {
  lengthMs_ = lengthMsNew_;
  expCoeff_ = expCoeffNew_;
  calculate_internal_parameters_();
  reset_read_pos_();
}

void Excitation::reset_read_pos_() { readPosition_ = 0.0; }

float Excitation::process() {
  float out = 0.0;
  if (readPosition_ < 1.0) {
    // out = amplitude_ * expf(-expCoeff_ * (readPosition_));
    out = amplitude_ *0.5f * (1.0f + cosf(M_PI *(2.0f*readPosition_ -1.0f)));
  
    readPosition_ += step_;
  }

  return out;
}