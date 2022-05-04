// RectangularMembrane.h: header file
#pragma once

#include <vector>

#include "ResonatorBank.h"

class RectangularMembrane : public ResonatorBank {
 public:
  // constructors
  RectangularMembrane() {}  // Default constructor
  RectangularMembrane(float sampleRate, float fundamentalFrequencyHz,
                      float decayMax, float decaySlope, int numPartials = 10,
                      float lengthRatio = 0.5);

  // Set parameters
  void setup(float sampleRate, float fundamentalFrequencyHz, float decayMax,
             float decaySlope, int numPartials = 10, float lengthRatio = 0.5);

  void setFundamentalFrequencyHz(float f);
  float getFundamentalFrequencyHz();
  void setDecayMax(float decayMax);
  float getDecayMax();
  void setDecaySlope(float decaySlope);
  float getDecaySlope();
  void setLengthRatio(float lengthRatio);
  float getLengthRatio();
  void setNumPartials(int numPartials);
  int getNumPartials();

  // float process(float in);  // Process sample

  ~RectangularMembrane() {}  // Destructor

 private:
  float fundamentalFrequencyHz_;
  float decayMax_;
  float decaySlope_;
  int numPartials_;
  float lengthRatio_;
  std::vector<float> calculate_frequencies_(int n, int m);
  std::vector<float> calculate_decays_(std::vector<float>& frequencies);
  void update_decays();
  void update_freqs_and_decays();
};