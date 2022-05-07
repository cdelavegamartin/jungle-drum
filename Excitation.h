// Excitation.h: Header file for the raised cosine excitation, its parameters
// are the length and amplitude
#pragma once

#include <vector>

class Excitation {
 public:
  // constructors
  Excitation() {}  // Default constructor
  Excitation(float sampleRate, float amplitude = 0.2f, float lengthMs = 5.0f);

  // Set parameters
  void setup(float sampleRate, float amplitude = 0.2f, float lengthMs = 5.0f);

  void setLengthMs(float l);
  float getLengthMs();
  void setAmplitude(float amp);
  float getAmplitude();

  void trigger();
  float process();  // Process sample

  ~Excitation() {}  // Destructor

 private:
  float sampleRate_;
  float lengthMs_;
  float lengthMsNew_;
  int lengthSamples_;
  float amplitude_;
  float readPosition_;
  float step_;

  void calculate_internal_parameters_();
};