#include "Excitation.h"

#include <Bela.h>

#include <algorithm>
#include <cmath>

Excitation::Excitation(float sampleRate, float lengthMs) {
  setup(sampleRate, lengthMs);
}

void Excitation::setup(float sampleRate, float lengthMs) {
  sampleRate_ = sampleRate;
  lengthMs_ = lengthMs;
  rt_printf("Length ms: %f \n", lengthMs_);

  Filter_.setup(200, sampleRate_, OnePole::HIGHPASS);
  allocate_buffer_();
  fill_buffer_();
}

void Excitation::setLengthMs(float l) {
  lengthMs_ = l;
  allocate_buffer_();
  fill_buffer_();
}

float Excitation::getLengthMs() { return lengthMs_; }

void Excitation::setExponentialCoefficient(float coeff) {
  expCoeff_ = coeff;
  fill_buffer_();
}

float Excitation::getExponentialCoefficient() { return expCoeff_; }

void Excitation::setAmplitude(float amp) { amplitude_ = amp; }

float Excitation::getAmplitude() { return amplitude_; }

void Excitation::allocate_buffer_() {
  lengthSamples_ = (int)(0.001 * lengthMs_ * sampleRate_);
  buffer_.resize(lengthSamples_);
  // rt_printf("Length samples: %i \n", lengthSamples_);
  // rt_printf("Length of buffer: %i \n", buffer_.size());

  bufferReadPointer_ = 0;
}

void Excitation::fill_buffer_() {
  float normInd = 0.0;
  for (int i = 0; i < buffer_.size(); i++) {
    normInd = (float)i / (float)(lengthSamples_);
    buffer_[i] = (1.0 + 0.5 * (1.0 + cosf(M_PI * normInd))) *
                 (expf(-expCoeff_ * (normInd)));
  }
}

void Excitation::trigger() { bufferReadPointer_ = 0; }

float Excitation::process() {
  float out = 0.0;
  if (bufferReadPointer_ < (int)buffer_.size()) {
    out = amplitude_ * buffer_[bufferReadPointer_];
    bufferReadPointer_++;
  }

  return out;
}