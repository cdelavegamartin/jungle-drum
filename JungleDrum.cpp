#include "JungleDrum.h"

#include <libraries/math_neon/math_neon.h>

#include <cmath>

JungleDrum::JungleDrum(float sampleRate, int midiNote, float decayMax,
                       float decaySlope, float pitchGlideFactor,
                       float expFactor, int numPartialsPerDim, float lengthRatio,
                       float impulseLengthMs, float filterResonance,
                       float attackTimeMs, float releaseTimeMs) {
  setup(sampleRate, midiNote, decayMax, decaySlope, pitchGlideFactor, expFactor,
        numPartialsPerDim, lengthRatio, impulseLengthMs, filterResonance,
        attackTimeMs, releaseTimeMs);
}

void JungleDrum::setup(float sampleRate, int midiNote, float decayMax,
                       float decaySlope, float pitchGlideFactor,
                       float expFactor, int numPartialsPerDim, float lengthRatio,
                       float impulseLengthMs, float filterResonance,
                       float attackTimeMs, float releaseTimeMs) {
  midiNote_ = midiNote;
  RectangularMembrane::setup(sampleRate, convert_midi_to_hz(), decayMax,
                             decaySlope, numPartialsPerDim, lengthRatio);
  Excitation::setup(sampleRate, impulseLengthMs, 15.0);
  PitchGlide::setup(pitchGlideFactor, expFactor);
  MoogVcf::setup(sampleRate, convert_midi_to_hz(), filterResonance, "bp2");

  setAttackTimeMs(attackTimeMs);
  setReleaseTimeMs(releaseTimeMs);
  resonanceEnvelope_.setDecayRate(0.0f);
  resonanceEnvelope_.setSustainLevel(1.0f);
  resonanceEnvelope_.setTargetRatioDR(100.0f);
}

void JungleDrum::setAttackTimeMs(float attackTimeMs) {
  resonanceEnvelope_.setAttackRate(0.001 * attackTimeMs *
                                   RectangularMembrane::sampleRate_);
}

void JungleDrum::setReleaseTimeMs(float releaseTimeMs) {
  resonanceEnvelope_.setReleaseRate(0.001 * releaseTimeMs *
                                    RectangularMembrane::sampleRate_);
}
int JungleDrum::getMidiNote() { return midiNote_; }

void JungleDrum::trigger() {
  Excitation::trigger();
  resonanceEnvelope_.gate(true);
}
float JungleDrum::process() {
  // Get next input sample
  float in = Excitation::process();
  //   Apply pitch glide to filter cutoff frequency (Rescale to 0-1 with
  //   excitation amplitude)
  MoogVcf::setFrequencyHz(PitchGlide::applyPitchGlide(
      in / getAmplitude(), getFundamentalFrequencyHz()));

  float out = RectangularMembrane::process(in);

  out = tanhf_neon(out);
  out = MoogVcf::process(out);

  return out;
}

float JungleDrum::convert_midi_to_hz() {
  return 440.0f * powf(2, (float)midiNote_ - 69.0f) / 12.0f;
}

void JungleDrum::noteOn(int midiNote) {
  midiNote_ = midiNote;
  setFundamentalFrequencyHz(convert_midi_to_hz());
  trigger();
}

void JungleDrum::noteOff() { resonanceEnvelope_.gate(false); }