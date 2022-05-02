#pragma once
// #include <libraries/OnePole/OnePole.h>

#include <vector>

class Excitation {
 public:
  // constructors
  Excitation() {}  // Default constructor
  Excitation(float sampleRate, float lengthMs, float expCoeff);

  // Set parameters
  void setup(float sampleRate, float lengthMs, float expCoeff);

  void setLengthMs(float l);
  float getLengthMs();
  void setAmplitude(float amp);
  float getAmplitude();
  void setExponentialCoefficient(float coeff);
  float getExponentialCoefficient();

  void trigger();
  float process();  // Process sample

  ~Excitation() {}  // Destructor

 private:
  float sampleRate_;
  float lengthMs_;
  float lengthMsNew_;
  int lengthSamples_;
  float expCoeff_;
  float expCoeffNew_;
  float amplitude_;
  float readPosition_;
  float step_;

  void calculate_internal_parameters_();
  void reset_read_pos_();
};