// PitchGlide.h

#pragma once

class PitchGlide {
 public:
  // constructors
  PitchGlide() {}  // Default constructor
  PitchGlide(float pitchGlideFactor, float expFactor);

  // Set parameters
  void setup(float pitchGlideFactor, float expFactor);

  void setPitchGlideFactor(float pf);
  float getPitchGlideFactor();
  void setExpFactor(float ef);
  float getExpFactor();
  float applyPitchGlide(float in, float f);

  ~PitchGlide() {}  // Destructor

 private:
  float pitchGlideFactor_;
  float expFactor_;
  float movingAvg_;

  void calculate_moving_avg_(float in);
};