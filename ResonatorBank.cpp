// ResonatorBank.cpp: file for implementing a 2nd Order resonating IIR filter

#include "ResonatorBank.h"

#include <cmath>
// #include <iostream>
#include <vector>

// Constructor taking arguments for sample rate and table data
ResonatorBank::ResonatorBank(float sampleRate,
                             std::vector<float>& frequenciesHz,
                             std::vector<float>& decays) {
  setup(sampleRate, frequenciesHz, decays);
}

void ResonatorBank::setup(float sampleRate, std::vector<float>& frequenciesHz,
                          std::vector<float>& decays) {
  sampleRate_ = sampleRate;
  nResonators_ = frequenciesHz.size();
  frequenciesHz_ = frequenciesHz;  // Frequencies of the resonators
  decays_ = decays;                // Frequencies of the resonators
  resonators_.resize(nResonators_);
//   std::cout << "In ResonatorBank::setup after resize \n" << std::endl;
//   std::cout << "Freqs size:  " << frequenciesHz_.size() << "\n" << std::endl;
  setFrequenciesHz(frequenciesHz_);
//   std::cout
//       << "In ResonatorBank::setup after setFrequenciesHz(frequenciesHz_) \n"
//       << std::endl;
//   std::cout << "Decays size:  " << decays_.size() << "\n" << std::endl;
  //   std::cout << "Decays size"<< decays_<< "\n" << std::endl;
  setDecays(decays_);
//   std::cout << "In ResonatorBank::setup after setDecays(decays_) \n"
//             << std::endl;
  for (int i = 0; i < nResonators_; i++) {
    resonators_[i].setup(sampleRate_, frequenciesHz_[i], decays_[i]);
  }
//   std::cout << "In ResonatorBank::setup after loop \n" << std::endl;
}

// Set the ResonatorBank frequencies
void ResonatorBank::setFrequenciesHz(std::vector<float>& frequenciesHz) {
  frequenciesHz_ = frequenciesHz;
  for (int i = 0; i < nResonators_; i++) {
    resonators_[i].setFrequencyHz(frequenciesHz_[i]);
  }
}

// Get the ResonatorBank frequencies
std::vector<float> ResonatorBank::getFrequenciesHz() { return frequenciesHz_; }

// Set the ResonatorBank decays
void ResonatorBank::setDecays(const std::vector<float>& decays) {
  decays_ = decays;
  for (int i = 0; i < nResonators_; i++) {
    resonators_[i].setDecay(decays_[i]);
  }
}

// Overloaded function with just a float
void ResonatorBank::setDecays(const float decay) {
  for (int i = 0; i < nResonators_; i++) {
    decays_[i] = decay;
    resonators_[i].setDecay(decay);
  }
}

// Get the ResonatorBank decays
std::vector<float> ResonatorBank::getDecays() { return decays_; }

std::vector<Resonator>::size_type ResonatorBank::getSize() {
  return nResonators_;
}

// Process next sample
float ResonatorBank::process(const float in) {
  // Calculate output sample
  float out = 0.0;
  for (int i = 0; i < nResonators_; i++) {
    out += resonators_[i].process(in);
  }
  out /= (float)nResonators_;
  return out;
}
