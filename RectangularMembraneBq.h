// RectangularMembrane.h: header file
#pragma once

#include <vector>

#include "BiquadResonatorBank.h"

class RectangularMembraneBq : public BiquadResonatorBank {
 public:
  // constructors
  RectangularMembraneBq() {}  // Default constructor
  RectangularMembraneBq(float sampleRate, float fundamentalFrequencyHz,
                      float gainDbMax, float gainDbSlope, float resonance = 100,
                      int numPartialsPerDim = 10, float lengthRatio = 0.5);

  // Set parameters
  void setup(float sampleRate, float fundamentalFrequencyHz, float gainDbMax,
             float gainDbSlope, float resonance = 100, int numPartialsPerDim = 10,
             float lengthRatio = 0.5);

  void setFundamentalFrequencyHz(float f);
  float getFundamentalFrequencyHz();
  void setGainDbMax(float gainDbMax);
  float getGainDbMax();
  void setGainDbSlope(float gainDbSlope);
  float getGainDbSlope();
  void setResonance(float resonance);
  float getResonance();
  void setLengthRatio(float lengthRatio);
  float getLengthRatio();
  void setNumPartialsPerDim(int numPartialsPerDim);
  int getNumPartialsPerDim();

  // float process(float in);  // Process sample

  ~RectangularMembraneBq() {}  // Destructor

 private:
  float fundamentalFrequencyHz_;
  float gainDbMax_;
  float gainDbSlope_;
  float resonance_;
  int numPartialsPerDim_;
  float lengthRatio_;
  std::vector<float> calculate_frequencies_(int n, int m);
  std::vector<float> calculate_gainsDb_(const std::vector<float>& frequencies);
  void update_gainsDb();
  void update_freqs_and_gainsDb();
};