// JungleDrum.h: header file for the KickDrum/bass synth hybrid
#pragma once
#include <libraries/ADSR/ADSR.h>

#include "Excitation.h"
#include "MoogVcf.h"
#include "PitchGlide.h"
#include "RectangularMembraneBq.h"

class JungleDrum : public RectangularMembraneBq,
                   public Excitation,
                   public PitchGlide,
                   public MoogVcf {
 public:
  // constructors
  JungleDrum() {}  // Default constructor
  JungleDrum(float sampleRate, int midiNote = 33, int numPartialsPerDim = 10,
             float lengthRatio = 0.5f, float membraneResonance = 100.0f,
             float gainDbMax = 30.0f, float gainDbSlope = 5.0f,
             float impulseAmplitude = 0.02f, float impulseLengthMs = 5.0f,
             float pitchGlideFactor = 1.0f, float expFactor = 0.5f,
             float filterResonance = 0.9f, float attackTimeMs = 100.0f,
             float releaseTimeMs = 500.0f);

  // Set parameters
  void setup(float sampleRate, int midiNote = 33, int numPartialsPerDim = 10,
             float lengthRatio = 0.5f, float membraneResonance = 100.0f,
             float gainDbMax = 30.0f, float gainDbSlope = 5.0f,
             float impulseAmplitude = 0.02f, float impulseLengthMs = 5.0f,
             float pitchGlideFactor = 1.0f, float expFactor = 0.5f,
             float filterResonance = 0.9f, float attackTimeMs = 100.0f,
             float releaseTimeMs = 500.0f);

  void trigger();   // Trigger parameter readds for new note and envelope
  float process();  // Process sample
  void noteOn(int midiNote);  // start new note
  void noteOff();             // Stop currently playing note

  // resonance envelope accesors
  void setAttackTimeMs(float attackTimeMs);
  void setReleaseTimeMs(float releaseTimeMs);

  // Delayed setters for Membrane shape
  void setNumPartialsPerDim(int numPartialsPerDim);
  void setLengthRatio(float lengthRatio);
  void setGainDbSlope(float gainDbSlope);

  // Delayed setters for Pitch Glide
  void setPitchGlideMax(float pitchGlideMax);
  void setPitchGlideSharp(float pitchGlideSharp);

  // Disambiguate resonance accesors
  void setMembraneResonance(float resonance);
  float getMembraneResonance();
  // max resonance
  void setFilterResonanceMax(float filterResonance);
  float getFilterResonanceMax();

  // midinote accesors
  int getMidiNote();

  // Reruns setup,
  void reset();

  ~JungleDrum() {}  // Destructor

 private:
  ADSR resonanceEnvelope_;
  int midiNote_;
  float filterResonanceMax_;
  int numPartialsPerDimNew_;
  float lengthRatioNew_;
  float pitchGlideMaxNew_;
  float pitchGlideSharpNew_;
  float gainDbSlopeNew_;
  float convert_midi_to_hz();
};