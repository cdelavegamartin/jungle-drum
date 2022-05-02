
#include "RectangularMembrane.h"

// #include <Bela.h>

#include <algorithm>
#include <cmath>
#include <vector>


// Constructor taking arguments
RectangularMembrane::RectangularMembrane(float sampleRate,
                                         float fundamentalFrequencyHz,
                                         float decayMax, float decaySlope,
                                         int numPartials, float lengthRatio) {
  setup(sampleRate, fundamentalFrequencyHz, decayMax, decaySlope, numPartials,
        lengthRatio);
}

void RectangularMembrane::setup(float sampleRate, float fundamentalFrequencyHz,
                                float decayMax, float decaySlope,
                                int numPartials, float lengthRatio) {
  sampleRate_ = sampleRate;
  fundamentalFrequencyHz_ = fundamentalFrequencyHz;
  decayMax_ = decayMax;
  decaySlope_ = decaySlope;
  numPartials_ = numPartials;
  lengthRatio_ = lengthRatio;

  std::vector<float> frequencies =
      calculate_frequencies_(numPartials_, numPartials_);
  std::vector<float> decays = calculate_decays_(frequencies);

  ResonatorBank::setup(sampleRate_, frequencies, decays);

}

std::vector<float> RectangularMembrane::calculate_frequencies_(int n, int m) {
  std::vector<float> freqs(n * m, 0.0);
  // freqs.reserve(nHarmonics);
  
  float r2 = powf(lengthRatio_, 2);
  float B2 = ((r2 + 1) / r2);
  float A2 = r2 + 1;
  // float fPrev = 0.0;
  // for (int i = 0; i < n; i++) {
  //   for (int j = 0; j < m; j++) {
  //     float coeff =
  //         sqrtf(powf((float)(i + 1), 2) / A2 + powf((float)(j + 1), 2) / B2);
  //     float f = coeff * fundamentalFrequencyHz_;
  //     if (fabs(f - fPrev) > 0.01) {
  //       freqs[i * m + j] = f;
  //     } else {
  //       freqs[i * m + j] = sampleRate_;
  //     }
  //     fPrev = f;
  //   }
  // }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      float coeff =
          sqrtf(powf((float)(i + 1), 2) / A2 + powf((float)(j + 1), 2) / B2);
      float f = coeff * fundamentalFrequencyHz_;
      freqs[i * m + j] = f;
    }
  }
  return freqs;
}

std::vector<float> RectangularMembrane::calculate_decays_(
    std::vector<float>& frequencies) {
  auto nRes = frequencies.size();
  std::vector<float> decays(nRes, 0.0);
  for (int i = 0; i < nRes; i++) {
    decays[i] = decayMax_ - decaySlope_ * frequencies[i];
  }
  return decays;
}

// float RectangularMembrane::process(float in) {
//   return ResonatorBank::process(in);
// }

// Helper functions
void RectangularMembrane::update_decays() {
  std::vector<float> freqs = getFrequenciesHz();
  std::vector<float> decays = calculate_decays_(freqs);
  setDecays(decays);
}

void RectangularMembrane::update_freqs_and_decays() {
  std::vector<float> frequencies =
      calculate_frequencies_(numPartials_, numPartials_);
  std::vector<float> decays = calculate_decays_(frequencies);
  setFrequenciesHz(frequencies);
  setDecays(decays);
}

// Accesors
void RectangularMembrane::setFundamentalFrequencyHz(float f) {
  fundamentalFrequencyHz_ = f;
  update_freqs_and_decays();
}

void RectangularMembrane::setDecayMax(float r) {
  decayMax_ = r;
  update_decays();
}

void RectangularMembrane::setDecaySlope(float s) {
  decaySlope_ = s;
  update_decays();
}

void RectangularMembrane::setLengthRatio(float l) {
  lengthRatio_ = l;
  update_freqs_and_decays();
}

void RectangularMembrane::setNumPartials(int num) {
  numPartials_ = num;
  update_freqs_and_decays();
}

// Get the resonator frequency
float RectangularMembrane::getFundamentalFrequencyHz() {
  return fundamentalFrequencyHz_;
}
float RectangularMembrane::getLengthRatio() { return lengthRatio_; }
int RectangularMembrane::getNumPartials() { return numPartials_; }
float RectangularMembrane::getDecayMax() { return decayMax_; }
float RectangularMembrane::getDecaySlope() { return decaySlope_; }