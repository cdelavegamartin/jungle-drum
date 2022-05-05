
#include "RectangularMembraneBq.h"

// #include <Bela.h>

#include <algorithm>
#include <cmath>
#include <vector>

// Constructor taking arguments
RectangularMembraneBq::RectangularMembraneBq(float sampleRate,
                                             float fundamentalFrequencyHz,
                                             float gainDbMax, float gainDbSlope,
                                             float resonance,
                                             int numPartialsPerDim,
                                             float lengthRatio) {
  setup(sampleRate, fundamentalFrequencyHz, gainDbMax, gainDbSlope,
        numPartialsPerDim, lengthRatio);
}

void RectangularMembraneBq::setup(float sampleRate,
                                  float fundamentalFrequencyHz, float gainDbMax,
                                  float gainDbSlope, float resonance,
                                  int numPartialsPerDim, float lengthRatio) {
  sampleRate_ = sampleRate;
  fundamentalFrequencyHz_ = fundamentalFrequencyHz;
  gainDbMax_ = gainDbMax;
  gainDbSlope_ = gainDbSlope;
  resonance_ = resonance;
  numPartialsPerDim_ = numPartialsPerDim;
  lengthRatio_ = lengthRatio;

  std::vector<float> frequencies =
      calculate_frequencies_(numPartialsPerDim_, numPartialsPerDim_);
  std::vector<float> gainsDb = calculate_gainsDb_(frequencies);

  BiquadResonatorBank::setup(sampleRate_, frequencies, resonance_, gainsDb);
}

std::vector<float> RectangularMembraneBq::calculate_frequencies_(int n, int m) {
  std::vector<float> freqs(n * m, 0.0);

  float r2 = powf(lengthRatio_, 2);
  float B2 = ((r2 + 1) / r2);
  float A2 = r2 + 1;
  // float fPrev = 0.0;
  // for (int i = 0; i < n; i++) {
  //   for (int j = 0; j < m; j++) {
  //     float coeff =
  //         sqrtf(powf((float)(i + 1), 2) / A2 + powf((float)(j + 1), 2) / B2);
  //     float f = coeff * fundamentalFrequencyHz_;
  //     if (fabs(f - fPrev) > 0.0000001) {
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

std::vector<float> RectangularMembraneBq::calculate_gainsDb_(
    const std::vector<float>& frequencies) {
  auto nRes = frequencies.size();

  std::vector<float> gainsDb(nRes, 0.0);
  for (int i = 0; i < nRes; i++) {
    gainsDb[i] =
        gainDbMax_ -
        gainDbSlope_ * (frequencies[i] / fundamentalFrequencyHz_ - 1.0f);
  }
  return gainsDb;
}

// float RectangularMembraneBq::process(float in) {
//   return BiquadResonatorBank::process(in);
// }

// Helper functions
void RectangularMembraneBq::update_gainsDb() {
  std::vector<float> freqs = getFrequenciesHz();
  std::vector<float> gainsDb = calculate_gainsDb_(freqs);
  setGainsDb(gainsDb);
}

void RectangularMembraneBq::update_freqs_and_gainsDb() {
  std::vector<float> frequencies =
      calculate_frequencies_(numPartialsPerDim_, numPartialsPerDim_);
  std::vector<float> gainsDb = calculate_gainsDb_(frequencies);
  setFrequenciesHz(frequencies);
  setGainsDb(gainsDb);
  setResonances(resonance_);
}

// Accesors
void RectangularMembraneBq::setFundamentalFrequencyHz(const float f) {
  fundamentalFrequencyHz_ = f;
  update_freqs_and_gainsDb();
}

void RectangularMembraneBq::setLengthRatio(const float l) {
  lengthRatio_ = l;
  update_freqs_and_gainsDb();
}

void RectangularMembraneBq::setNumPartialsPerDim(const int num) {
  numPartialsPerDim_ = num;
  update_freqs_and_gainsDb();
}

void RectangularMembraneBq::setGainDbMax(const float g) {
  gainDbMax_ = g;
  update_gainsDb();
}

void RectangularMembraneBq::setGainDbSlope(const float s) {
  gainDbSlope_ = s;
  update_gainsDb();
}

void RectangularMembraneBq::setResonance(const float r) {
  resonance_ = r;
  setResonances(resonance_);
}

// Get the resonator frequency
float RectangularMembraneBq::getFundamentalFrequencyHz() {
  return fundamentalFrequencyHz_;
}
float RectangularMembraneBq::getLengthRatio() { return lengthRatio_; }
int RectangularMembraneBq::getNumPartialsPerDim() { return numPartialsPerDim_; }
float RectangularMembraneBq::getGainDbMax() { return gainDbMax_; }
float RectangularMembraneBq::getGainDbSlope() { return gainDbSlope_; }
float RectangularMembraneBq::getResonance() { return resonance_; }