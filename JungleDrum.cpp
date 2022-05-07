#include "JungleDrum.h"

#include <libraries/math_neon/math_neon.h>

#include <cmath>

JungleDrum::JungleDrum(float sampleRate, int midiNote, int numPartialsPerDim,
                       float lengthRatio, float membraneResonance,
                       float gainDbMax, float gainDbSlope,
                       float impulseAmplitude, float impulseLengthMs,
                       float pitchGlideFactor, float expFactor,
                       float filterResonance, float attackTimeMs,
                       float releaseTimeMs) {
  setup(sampleRate, midiNote, numPartialsPerDim, lengthRatio, membraneResonance,
        gainDbMax, gainDbSlope, impulseAmplitude, impulseLengthMs,
        pitchGlideFactor, expFactor, filterResonance, attackTimeMs,
        releaseTimeMs);
}

void JungleDrum::setup(float sampleRate, int midiNote, int numPartialsPerDim,
                       float lengthRatio, float membraneResonance,
                       float gainDbMax, float gainDbSlope,
                       float impulseAmplitude, float impulseLengthMs,
                       float pitchGlideFactor, float expFactor,
                       float filterResonance, float attackTimeMs,
                       float releaseTimeMs) {
  midiNote_ = midiNote;
  filterResonanceMax_ = filterResonance;
  RectangularMembraneBq::setup(sampleRate, convert_midi_to_hz(),
                               numPartialsPerDim, lengthRatio,
                               membraneResonance, gainDbMax, gainDbSlope);
  Excitation::setup(sampleRate, impulseAmplitude, impulseLengthMs);
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
                                   RectangularMembraneBq::sampleRate_);
}

void JungleDrum::setReleaseTimeMs(float releaseTimeMs) {
  resonanceEnvelope_.setReleaseRate(0.001 * releaseTimeMs *
                                    RectangularMembraneBq::sampleRate_);
}

void JungleDrum::setFilterResonanceMax(float filterResonance) {
  filterResonanceMax_ = filterResonance;
}

float JungleDrum::getFilterResonanceMax() { return filterResonanceMax_; }

void JungleDrum::setMembraneResonance(float resonance) {
  RectangularMembraneBq::setResonance(resonance);
}

float JungleDrum::getMembraneResonance() {
  return RectangularMembraneBq::getResonance();
}

int JungleDrum::getMidiNote() { return midiNote_; }

// Delayed setters for Membrane shape
void JungleDrum::setNumPartialsPerDim(int numPartialsPerDim) {
  numPartialsPerDimNew_ = numPartialsPerDim;
}
void JungleDrum::setLengthRatio(float lengthRatio) {
  lengthRatioNew_ = lengthRatio;
}

void JungleDrum::setGainDbSlope(float gainDbSlope) {
  gainDbSlopeNew_= gainDbSlope;
}

void JungleDrum::setPitchGlideMax(float pitchGlideMax) {
  pitchGlideMaxNew_= pitchGlideMax;
}

void JungleDrum::setPitchGlideSharp(float pitchGlideSharp) {
  pitchGlideSharpNew_= pitchGlideSharp;
}

void JungleDrum::trigger() {
  Excitation::trigger();
  resonanceEnvelope_.gate(true);

  // Apply stored values to membrane
  RectangularMembraneBq::setNumPartialsPerDim(numPartialsPerDimNew_);
  RectangularMembraneBq::setLengthRatio(lengthRatioNew_);
  RectangularMembraneBq::setGainDbSlope(gainDbSlopeNew_);
  // Apply stored values to pitch glide
  PitchGlide::setPitchGlideFactor(pitchGlideMaxNew_);
  PitchGlide::setExpFactor(pitchGlideSharpNew_);

}

float JungleDrum::process() {
  // Get next input sample
  float in = Excitation::process();
  //   Apply pitch glide to filter cutoff frequency (Rescale to 0-1 with
  //   excitation amplitude)
  MoogVcf::setFrequencyHz(PitchGlide::applyPitchGlide(
      in / getAmplitude(), getFundamentalFrequencyHz()));
  MoogVcf::setResonance(filterResonanceMax_ * resonanceEnvelope_.process());
  float out = RectangularMembraneBq::process(in);

  out = MoogVcf::process(out);
  // out = tanhf_neon(out);

  return out;
}

float JungleDrum::convert_midi_to_hz() {
  return 440.0f * powf(2, (((float)midiNote_ - 69.0f) / 12.0f));
}

void JungleDrum::noteOn(int midiNote) {
  midiNote_ = midiNote;
  setFundamentalFrequencyHz(convert_midi_to_hz());
  trigger();
}

void JungleDrum::noteOff() { resonanceEnvelope_.gate(false); }

void JungleDrum::reset(){
  JungleDrum::setup(RectangularMembraneBq::sampleRate_);
}