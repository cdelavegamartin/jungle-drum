// BiquadResonatorBank.cpp: file for implementing a 2nd Order resonating IIR
// filter bank. Uses biquad filters in bandpass configuration. The number of
// resonators in the filter bank is determined by the number of frequencies
// givento it. The other parameters are adjusted to that size

#include "BiquadResonatorBank.h"

#include <libraries/math_neon/math_neon.h>

#include <vector>

// Constructor taking arguments for sample rate and table data
BiquadResonatorBank::BiquadResonatorBank(
    const float sampleRate, const std::vector<float>& frequenciesHz,
    const std::vector<float>& resonances, const std::vector<float>& gainsDb) {
  setup(sampleRate, frequenciesHz, resonances, gainsDb);
}

BiquadResonatorBank::BiquadResonatorBank(
    const float sampleRate, const std::vector<float>& frequenciesHz) {
  setup(sampleRate, frequenciesHz);
}

void BiquadResonatorBank::setup(const float sampleRate,
                                const std::vector<float>& frequenciesHz,
                                const std::vector<float>& resonances,
                                const std::vector<float>& gainsDb) {
  sampleRate_ = sampleRate;

  // Common settings
  settings_.fs = sampleRate_;
  settings_.type = Biquad::peak;

  // Set parameters
  setFrequenciesHz(frequenciesHz);
  setResonances(resonances);
  setGainsDb(gainsDb);
}

void BiquadResonatorBank::setup(const float sampleRate,
                                const std::vector<float>& frequenciesHz,
                                const float resonance,
                                const std::vector<float>& gainsDb) {
  sampleRate_ = sampleRate;

  // Common settings
  settings_.fs = sampleRate_;
  settings_.type = Biquad::peak;

  // Set parameters
  setFrequenciesHz(frequenciesHz);
  setResonances(resonance);
  setGainsDb(gainsDb);
}

void BiquadResonatorBank::setup(const float sampleRate,
                                const std::vector<float>& frequenciesHz,
                                const float resonance, const float gainDb) {
  sampleRate_ = sampleRate;

  // Common settings
  settings_.fs = sampleRate_;
  settings_.type = Biquad::peak;

  // Set parameters
  setFrequenciesHz(frequenciesHz);
  setResonances(resonance);
  setGainsDb(gainDb);
}

// Set the BiquadResonatorBank frequencies
void BiquadResonatorBank::setFrequenciesHz(
    const std::vector<float>& frequenciesHz) {
  frequenciesHz_ = frequenciesHz;
  nResonators_ = frequenciesHz_.size();
  resonators_.resize(nResonators_, Biquad(settings_));
  for (int i = 0; i < nResonators_; i++) {
    resonators_[i].setFc(frequenciesHz_[i]);
  }
}

// Get the BiquadResonatorBank frequencies
std::vector<float> BiquadResonatorBank::getFrequenciesHz() {
  return frequenciesHz_;
}

// Set the BiquadResonatorBank Q factors
void BiquadResonatorBank::setResonances(const std::vector<float>& resonances) {
  resonances_ = resonances;
  // Use the last resonance given to fill missing ones
  if (resonances_.size() != nResonators_) {
    resonances_.resize(nResonators_, resonances_.back());
  }
  for (int i = 0; i < nResonators_; i++) {
    resonators_[i].setQ(resonances_[i]);
  }
}

// Overload function with just a single Q factor
void BiquadResonatorBank::setResonances(const float resonance) {
  if (resonances_.size() != nResonators_) {
    resonances_.resize(nResonators_, resonance);
  }
  for (int i = 0; i < nResonators_; i++) {
    resonances_[i] = resonance;
    resonators_[i].setQ(resonances_[i]);
  }
}

// Get the BiquadResonatorBank Q factors
std::vector<float> BiquadResonatorBank::getResonances() { return resonances_; }

// Set the BiquadResonatorBank gains
void BiquadResonatorBank::setGainsDb(const std::vector<float>& gainsDb) {
  gainsDb_ = gainsDb;
  if (gainsDb_.size() != nResonators_) {
    gainsDb_.resize(nResonators_, 0.0);
  }
  for (int i = 0; i < nResonators_; i++) {
    resonators_[i].setPeakGain(gainsDb_[i]);
  }
}

// Set the BiquadResonatorBank gains with asingle value
void BiquadResonatorBank::setGainsDb(const float gainDb) {
  if (gainsDb_.size() != nResonators_) {
    gainsDb_.resize(nResonators_, gainDb);
  }
  for (int i = 0; i < nResonators_; i++) {
    gainsDb_[i] = gainDb;
    resonators_[i].setPeakGain(gainsDb_[i]);
  }
}

// Get the BiquadResonatorBank gains
std::vector<float> BiquadResonatorBank::getGainsDb() { return gainsDb_; }

// Get size of filter bank
std::vector<Biquad>::size_type BiquadResonatorBank::getSize() {
  return nResonators_;
}

// Process next sample
float BiquadResonatorBank::process(const float in) {
  // Calculate output sample
  float out = 0.0;
  for (int i = 0; i < nResonators_; i++) {
    out += resonators_[i].process(in);
  }
  // Normalize output using the fast sqrtf_c for performance
  // Lower precision means there is some variability, not completely
  // undesirable
  out /= sqrtf_c((float)nResonators_);
  return out;
}
