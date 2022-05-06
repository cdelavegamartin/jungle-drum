// JungleDrum.h: header file
#pragma once
#include <libraries/ADSR/ADSR.h>

#include "Excitation.h"
#include "MoogVcf.h"
#include "PitchGlide.h"
#include "RectangularMembrane.h"

class JungleDrum : public RectangularMembrane,
                   public Excitation,
                   public PitchGlide,
                   public MoogVcf {
 public:
  // constructors
  JungleDrum() {}  // Default constructor
  JungleDrum(float sampleRate, int midiNote, float decayMax,
             float decaySlope, float pitchGlideFactor = 0.0,
             float expFactor = 0.99, int numPartialsPerDim = 10,
             float lengthRatio = 0.5, float impulseLengthMs = 5.0,
             float filterResonance = 0.9, float attackTimeMs = 100.0,
             float releaseTimeMs = 500.0);

  // Set parameters
  void setup(float sampleRate, int midiNote, float decayMax,
             float decaySlope, float pitchGlideFactor = 0.0,
             float expFactor = 0.99, int numPartialsPerDim = 10,
             float lengthRatio = 0.5, float impulseLengthMs = 5.0,
             float filterResonance = 0.9, float attackTimeMs = 100.0,
             float releaseTimeMs = 500.0);

  void trigger();   // Trigger new note
  float process();  // Process sample
  void noteOn(int midiNote);
  void noteOff();

  // resonance envelope accesors
  void setAttackTimeMs(float attackTimeMs);
  void setReleaseTimeMs(float releaseTimeMs);
  // midinote accesors
  int getMidiNote();

  ~JungleDrum() {}  // Destructor

 private:
  ADSR resonanceEnvelope_;
  int midiNote_;
  float outAmplitude_;
  float convert_midi_to_hz();
};