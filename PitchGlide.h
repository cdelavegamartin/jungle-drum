// PitchGlide.h: Header for an auxiliary class implementing the modulation of a
// frequency dependent on an input. Uses an exponential moving average with a
// tunable factor, adjusting the weighting of previous values, and therefore the
// "sharpness" or reactivity. The max pitch glide can also be adjusted. Doesn't
// assume a normalized input amplitude, but that makes it more interpretable

#pragma once

class PitchGlide {
 public:
  // constructors
  PitchGlide() {}  // Default constructor
  PitchGlide(float pitchGlideFactor, float expFactor);

  // Set parameters
  void setup(float pitchGlideFactor, float expFactor);

  // Accesors
  void setPitchGlideFactor(float pf);
  float getPitchGlideFactor();
  void setExpFactor(float ef);
  float getExpFactor();

  // Apply the pitch glide
  float applyPitchGlide(float in, float f);

  ~PitchGlide() {}  // Destructor

 private:
  float pitchGlideFactor_;
  float expFactor_;
  float movingAvg_;

  void calculate_moving_avg_(float in);  // Internal helper function
};