// PitchGlide.cpp

#include "PitchGlide.h"

#include <cmath>

PitchGlide::PitchGlide(float pitchGlideFactor, float expFactor) {
  setup(pitchGlideFactor, expFactor);
}
void PitchGlide::setup(float pitchGlideFactor, float expFactor) {
  pitchGlideFactor_ = pitchGlideFactor;
  expFactor_ = expFactor;
  movingAvg_ = 0.0;
}

void PitchGlide::setPitchGlideFactor(float pitchGlideFactor) {
  pitchGlideFactor_ = pitchGlideFactor;
}

float PitchGlide::getPitchGlideFactor() { return pitchGlideFactor_; }

void PitchGlide::setExpFactor(float expFactor) { expFactor_ = expFactor; }

float PitchGlide::getExpFactor() { return expFactor_; }

float PitchGlide::applyPitchGlide(float in, float f) {
  calculate_moving_avg_(in);
  return f * (1.0f + pitchGlideFactor_ * movingAvg_);
}

void PitchGlide::calculate_moving_avg_(float in) {
  movingAvg_ = expFactor_ * in + (1.0f - expFactor_) * movingAvg_;
}