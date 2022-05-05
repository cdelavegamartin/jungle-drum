// BiquadResonatorBank.h: header file for bank of Resonator objects
#pragma once

#include <libraries/Biquad/Biquad.h>

#include <vector>

class BiquadResonatorBank {
 public:
  BiquadResonatorBank() {}  // Default constructor
  BiquadResonatorBank(float sampleRate, const std::vector<float>& frequenciesHz,
                      const std::vector<float>& resonances,
                      const std::vector<float>& gainsDb);

  void setup(float sampleRate, const std::vector<float>& frequencies,
             const std::vector<float>& resonances, const std::vector<float>& gainsDb);

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

 private:
  std::vector<Biquad>::size_type nResonators_;  // number of resonators
  std::vector<float> frequenciesHz_;  // Frequency of the BiquadResonatorBank
  std::vector<float> resonances_;     // Decay of the BiquadResonatorBank
  std::vector<float> gainsDb_;        // Decay of the BiquadResonatorBank
  std::vector<Biquad> resonators_;
  Biquad::Settings settings_;
};
