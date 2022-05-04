#include "NonlinearMembrane.h"

#include <cmath>

NonlinearMembrane::NonlinearMembrane(float sampleRate,
                                     float fundamentalFrequencyHz,
                                     float decayMax, float decaySlope,
                                     float pitchGlideFactor, float expFactor, int numPartials,
                                     float lengthRatio) {
  setup(sampleRate, fundamentalFrequencyHz, decayMax, decaySlope,
        pitchGlideFactor, expFactor, numPartials, lengthRatio);
}

void NonlinearMembrane::setup(float sampleRate, float fundamentalFrequencyHz,
                              float decayMax, float decaySlope,
                              float pitchGlideFactor, float expFactor, int numPartials,
                              float lengthRatio) {
  RectangularMembrane::setup(sampleRate, fundamentalFrequencyHz, decayMax,
                             decaySlope, numPartials, lengthRatio);
  PitchGlide::setup(pitchGlideFactor, expFactor);
  counter_ = 0;
  blocksize_ = 512;
  accumulator_ = 0.0;
}

float NonlinearMembrane::process(float in) {
  //   if (in > 0.00001) {
  //     int nResonators_ = getSize();
  //     for (int i = 0; i < nResonators_; i++) {
  //       resonators_[i].setFrequencyHz(
  //           PitchGlide::applyPitchGlide(in,
  //           resonators_[i].getFrequencyHz()));
  //     }
  //   }
  // if (counter_ < blocksize_) {
  //   accumulator_ += fabs(in) / (float)blocksize_;
  //   counter_++;
  // } else {
  //     setFundamentalFrequencyHz(applyPitchGlide(accumulator_,getFundamentalFrequencyHz()));
  //     counter_ = 0;
  //     accumulator_ = 0.0;
  // }
  return ResonatorBank::process(in);
}