// NonlinearMembrane.h: header file
#pragma once

#include <vector>

#include "RectangularMembrane.h"

class NonlinearMembrane : public RectangularMembrane {
 public:
  // constructors
  NonlinearMembrane() {}  // Default constructor
  NonlinearMembrane(float sampleRate, float fundamentalFrequencyHz,
                      float decayMax, float decaySlope, int numPartials = 100,
                      float lengthRatio = 0.5);

  // Set parameters
  void setup(float sampleRate, float fundamentalFrequencyHz, float decayMax,
             float decaySlope, int numPartials = 100, float lengthRatio = 0.5);

  void setPitchGlideFactor(float pf);
  float getPitchGlideFactor();
  void setPitchGlideAmplitude(float pa);
  float getPitchGlideAmplitude();


  float process(float in);  // Process sample

  ~NonlinearMembrane() {}  // Destructor

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