#pragma once
// #include <libraries/OnePole/OnePole.h>

#include <vector>

class Excitation {
 public:
  // constructors
  Excitation() {}  // Default constructor
  Excitation(float sampleRate, float lengthMs);

  // Set parameters
  void setup(float sampleRate, float lengthMs);

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
  void reset_read_pos_();
};