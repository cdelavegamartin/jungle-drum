// BiquadResonatorBank.h: header file for bank of Resonator objects
#pragma once

#include <vector>

#include "BiquadFloat.h"

class BiquadResonatorBank {
 public:
  BiquadResonatorBank() {}  // Default constructor
  BiquadResonatorBank(const float sampleRate,
                      const std::vector<float>& frequenciesHz,
                      const std::vector<float>& resonances,
                      const std::vector<float>& gainsDb);
  // Minimal constructor
  BiquadResonatorBank(const float sampleRate,
                      const std::vector<float>& frequenciesHz);

  // Overloaded setup function
  void setup(const float sampleRate, const std::vector<float>& frequencies,
             const std::vector<float>& resonances,
             const std::vector<float>& gainsDb);

  void setup(const float sampleRate, const std::vector<float>& frequencies,
             const float resonance, const std::vector<float>& gainsDb);

  void setup(const float sampleRate, const std::vector<float>& frequencies,
             const float resonance = 100.0, const float gainDb = 50.0);

  // Accesors
  void setFrequenciesHz(const std::vector<float>& fs);
  std::vector<float> getFrequenciesHz();

  void setResonances(const std::vector<float>& qs);
  void setResonances(const float q);
  std::vector<float> getResonances();

  void setGainsDb(const std::vector<float>& gs);
  void setGainsDb(const float g);
  std::vector<float> getGainsDb();

  std::vector<Biquad>::size_type getSize();

  float process(float in);  // Process sample

  ~BiquadResonatorBank() {}  // Destructor

 protected:
  float sampleRate_;  // audio sample rate

  // Not private in order to modify them directly in child classes for
  // efficiency
  std::vector<Biquad>::size_type nResonators_;  // number of resonators
  std::vector<float> frequenciesHz_;  // Frequencies of the resonantors
  std::vector<float> resonances_;     // Q factor of the resonators
  std::vector<float> gainsDb_;        // Gains of the resonators

 private:
  std::vector<Biquad> resonators_; // Vector of resonators
  Biquad::Settings settings_; // Settings for the setup of resonators
};
