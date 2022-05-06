
#include "RectangularMembraneBq.h"

#include <libraries/math_neon/math_neon.h>

// #include <algorithm>
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

  normFrequencies_ =
      calculate_normalized_frequencies_(numPartialsPerDim_, lengthRatio_);
  update_log2frequencies_();

  BiquadResonatorBank::setup(
      sampleRate_,
      calculate_frequencies_(fundamentalFrequencyHz_, normFrequencies_),
      resonance_, calculate_gainsDb_());
}

// Helper Functions

// Calculate the normalized frequencies. Most computationally intensive task
std::vector<float> RectangularMembraneBq::calculate_normalized_frequencies_(
    int numParDim, float lengthRatio) {
  std::vector<float> normFreqs(numParDim * numParDim, 0.0);

  float r2 = lengthRatio * lengthRatio;
  float C = 1.0f / (r2 + 1.0f);

  for (int i = 0; i < numParDim; i++) {
    for (int j = 0; j < numParDim; j++) {
      normFreqs[i * numParDim + j] = sqrtf_neon(
          C * ((float)((i + 1) * (i + 1)) + r2 * (float)((j + 1) * (j + 1))));
    }
  }
  return normFreqs;
}

// Update auxiliary vector log2(normFreq) 
void RectangularMembraneBq::update_log2frequencies_() {
  normFrequenciesLog2_.resize(normFrequencies_.size());
  for (int i = 0; i < normFrequencies_.size(); i++) {
    normFrequenciesLog2_[i] = log2f(normFrequencies_[i]);
  }
}

// Calculate the actual partial frequencies
std::vector<float> RectangularMembraneBq::calculate_frequencies_(
    const float fundamental, const std::vector<float>& normFrequencies) {
  std::vector<float> frequencies = normFrequencies;

  for (int i = 0; i < frequencies.size(); i++) {
    frequencies[i] = fundamental * frequencies[i];
  }
  return frequencies;
}

// Calculate ResonatorBank gains
std::vector<float> RectangularMembraneBq::calculate_gainsDb_() {
  std::vector<float> gainsDb(normFrequenciesLog2_.size());
  for (int i = 0; i < normFrequenciesLog2_.size(); i++) {
    gainsDb[i] = gainDbMax_ - gainDbSlope_ * normFrequenciesLog2_[i];
  }
  return gainsDb;
}

// Update the auxiliary internal vectors and the ResonatorsBank frequencies and
// gains
void RectangularMembraneBq::update_freqs_and_gainsDb() {
  normFrequencies_ =
      calculate_normalized_frequencies_(numPartialsPerDim_, lengthRatio_);
  update_log2frequencies_();
  setFrequenciesHz(
      calculate_frequencies_(fundamentalFrequencyHz_, normFrequencies_));
  setGainsDb(calculate_gainsDb_());
  setResonances(resonance_);
}

// Setters
// They all use checks to make sure the calculations are only done if the values
// have changed
void RectangularMembraneBq::setFundamentalFrequencyHz(const float f) {
  if (fundamentalFrequencyHz_ != f) {
    fundamentalFrequencyHz_ = f;
    setFrequenciesHz(
        calculate_frequencies_(fundamentalFrequencyHz_, normFrequencies_));
  }
}

void RectangularMembraneBq::setLengthRatio(const float l) {
  if (lengthRatio_ != l) {
    lengthRatio_ = l;
    update_freqs_and_gainsDb();
  }
}

void RectangularMembraneBq::setNumPartialsPerDim(const int num) {
  if (numPartialsPerDim_ != num) {
    numPartialsPerDim_ = num;
    update_freqs_and_gainsDb();
  }
}

void RectangularMembraneBq::setGainDbMax(const float g) {
  if (gainDbMax_ != g) {
    float gainDiff = g - gainDbMax_;
    gainDbMax_ = g;
    for (int i = 0; i < gainsDb_.size(); i++) {
      gainsDb_[i] = gainsDb_[i] + gainDiff;
    }
  }
}

void RectangularMembraneBq::setGainDbSlope(const float s) {
  if (gainDbSlope_ != s) {
    gainDbSlope_ = s;
    setGainsDb(calculate_gainsDb_());
  }
}

void RectangularMembraneBq::setResonance(const float r) {
  if (resonance_ != r) {
    resonance_ = r;
    setResonances(resonance_);
  }
}

// Only used in case the calculation is delegated to another thread
void RectangularMembraneBq::setNormFrequencies(
    const std::vector<float>& normFrequencies) {
  normFrequencies_ = normFrequencies;
  update_log2frequencies_();
  setFrequenciesHz(
      calculate_frequencies_(fundamentalFrequencyHz_, normFrequencies_));
  setGainsDb(calculate_gainsDb_());
}

// Getters
float RectangularMembraneBq::getFundamentalFrequencyHz() {
  return fundamentalFrequencyHz_;
}
float RectangularMembraneBq::getLengthRatio() { return lengthRatio_; }
int RectangularMembraneBq::getNumPartialsPerDim() { return numPartialsPerDim_; }
float RectangularMembraneBq::getGainDbMax() { return gainDbMax_; }
float RectangularMembraneBq::getGainDbSlope() { return gainDbSlope_; }
float RectangularMembraneBq::getResonance() { return resonance_; }