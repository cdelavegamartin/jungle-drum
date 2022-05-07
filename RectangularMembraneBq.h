// RectangularMembrane.h: Header file for the rectangular membrane
// implementation. It's a wrapper for the resonator bank, with extra checks to
// avoid costly computations if not necessary. It also calculates the
// frequencies given the fundamental and the membrane side ratio.
#pragma once

#include <vector>

#include "BiquadResonatorBank.h"

class RectangularMembraneBq : public BiquadResonatorBank {
 public:
  // constructors
  RectangularMembraneBq() {}  // Default constructor
  RectangularMembraneBq(float sampleRate, float fundamentalFrequencyHz,
                        int numPartialsPerDim = 10, float lengthRatio = 0.5f,
                        float resonance = 100.0f, float gainDbMax = 30.0f,
                        float gainDbSlope = 5.0f);

  // Set parameters
  void setup(float sampleRate, float fundamentalFrequencyHz,
             int numPartialsPerDim = 10, float lengthRatio = 0.5f,
             float resonance = 100.0f, float gainDbMax = 30.0f,
             float gainDbSlope = 5.0f);

  // Accesors
  void setFundamentalFrequencyHz(float f);
  float getFundamentalFrequencyHz();
  void setGainDbMax(const float gainDbMax);
  float getGainDbMax();
  void setGainDbSlope(const float gainDbSlope);
  float getGainDbSlope();
  void setResonance(const float resonance);
  float getResonance();
  void setLengthRatio(const float lengthRatio);
  float getLengthRatio();
  void setNumPartialsPerDim(const int numPartialsPerDim);
  int getNumPartialsPerDim();

  // Future proofing to be able to delegate the coefficient calculations to a
  // lower priority thread. Not used at the moment
  void setNormFrequencies(const std::vector<float>& normFrequencies);
  std::vector<float> getNormFrequencies();

  ~RectangularMembraneBq() {}  // Destructor

 private:
  float fundamentalFrequencyHz_;  // f0
  float gainDbMax_;               // maximum gain (f0)
  float gainDbSlope_;             // db/octave gain attenuation
  float resonance_;               // Single resonance value for all resonators
  int numPartialsPerDim_;         // Total number of partials is n*n
  float lengthRatio_;             // Ratio of the sides of the plate
  std::vector<float> normFrequencies_;      // Normalized frequencies (f0=1)
  std::vector<float> normFrequenciesLog2_;  // auxiliary vector

  // auxiliary functions
  std::vector<float> calculate_normalized_frequencies_(const int numParDim,
                                                       const float lengthRatio);
  std::vector<float> calculate_frequencies_(
      const float fundamental, const std::vector<float>& normFrequencies);
  std::vector<float> calculate_gainsDb_();
  void update_log2frequencies_();
  void update_freqs_and_gainsDb();
};