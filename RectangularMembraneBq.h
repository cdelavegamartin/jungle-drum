// RectangularMembrane.h: header file
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
  void setNormFrequencies(const std::vector<float>& normFrequencies);
  std::vector<float> getNormFrequencies();

  // float process(float in);  // Process sample

  ~RectangularMembraneBq() {}  // Destructor

 private:
  float fundamentalFrequencyHz_;
  float gainDbMax_;
  float gainDbSlope_;
  float resonance_;
  int numPartialsPerDim_;
  float lengthRatio_;
  std::vector<float> normFrequencies_;
  std::vector<float> normFrequenciesLog2_;

  std::vector<float> calculate_normalized_frequencies_(const int numParDim,
                                                       const float lengthRatio);
  std::vector<float> calculate_frequencies_(
      const float fundamental, const std::vector<float>& normFrequencies);
  std::vector<float> calculate_gainsDb_();
  void update_log2frequencies_();

  // void update_gainsDb();
  void update_freqs_and_gainsDb();
};