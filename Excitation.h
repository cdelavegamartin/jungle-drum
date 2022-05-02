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
  void setExponentialCoefficient(float coeff);
  float getExponentialCoefficient();

  void trigger();
  float process();  // Process sample

  ~Excitation() {}  // Destructor

 private:
  float sampleRate_;
  float lengthMs_;
  int lengthSamples_;
  float expCoeff_;
  float amplitude_;
  std::vector<float> buffer_;
  int bufferReadPointer_;

  // OnePole Filter_;

  void allocate_buffer_();
  void fill_buffer_();
};