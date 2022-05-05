// NonlinearMembrane.h: header file
#pragma once

#include "PitchGlide.h"
#include "RectangularMembrane.h"

class NonlinearMembrane : public RectangularMembrane, public PitchGlide {
 public:
  // constructors
  NonlinearMembrane() {}  // Default constructor
  NonlinearMembrane(float sampleRate, float fundamentalFrequencyHz,
                    float decayMax, float decaySlope,
                    float pitchGlideFactor = 0.0,  float expFactor=0.99, int numPartialsPerDim = 100,
                    float lengthRatio = 0.5);

  // Set parameters
  void setup(float sampleRate, float fundamentalFrequencyHz, float decayMax,
             float decaySlope, float pitchGlideFactor = 0.0,  float expFactor=0.99,
             int numPartialsPerDim = 100, float lengthRatio = 0.5);

  float process(float in);  // Process sample

  ~NonlinearMembrane() {}  // Destructor

 private:
  int blocksize_;
  int counter_;
  float accumulator_;
};