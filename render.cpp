#include <Bela.h>
#include <libraries/ADSR/ADSR.h>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>
#include <libraries/Scope/Scope.h>
#include <libraries/math_neon/math_neon.h>

#include <cmath>
#include <map>
#include <string>
#include <vector>
// #include <iostream>
#include <algorithm>

#include "BiquadResonatorBank.h"
#include "Excitation.h"
#include "MoogVcf.h"
#include "NonlinearMembrane.h"
#include "RectangularMembrane.h"
#include "ResonatorBank.h"

// Browser-based GUI to adjust parameters
Gui gGui;
GuiController gGuiController;

// Browser-based oscilloscope to visualise signal
Scope gScope;

// resonator objects

RectangularMembrane gRectM;

NonlinearMembrane gNlinM;
BiquadResonatorBank gResBank;
PitchGlide gPitchGlide;

ADSR envelope;  // ADSR envelope

float gAttack = 0.1;   // Envelope attack (seconds)
float gDecay = 0.25;   // Envelope decay (seconds)
float gRelease = 0.5;  // Envelope release (seconds)
float gSustain = 1.0;  // Envelope sustain level

int gGateStatus = 0;  // Env on/off

// Filter objects
MoogVcf gFilter;

// filter configs mapped to int values for use with the slider
std::map<int, std::string> gConfigs{{1, "lp4"}, {2, "lp2"}, {3, "bp4"},
                                    {4, "bp2"}, {5, "hp4"}, {6, "hp2"}};

// Input Object
Excitation gNoiseIn;

// White noise
float gNoiseLength = 5.0;       // ms
float gNoiseInterval = 2000.0;  // ms

// samples ellapsed since last accelerometer measurement
int gSampleCounter = 0;

std::vector<float> freqsinit{100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

// Setup
bool setup(BelaContext *context, void *userData) {
  gRectM.setup(context->audioSampleRate, 15, 0.9999f, 0.00001f);
  // gNlinM.setup(context->audioSampleRate, 15, 0.9999f, 0.00001f);
  gPitchGlide.setup(500.0, 0.01);

  std::vector<float> resonances(10, 20);
  std::vector<float> gains(10, -5.0);
  gResBank.setup(context->audioSampleRate, freqsinit, resonances, gains);

  gNoiseIn.setup(context->audioSampleRate, gNoiseLength, 5.0f);
  //   rt_printf("Length ms: %f \n", gNoiseIn.getLengthMs());

  // Initialize the filter
  gFilter.setup(context->audioSampleRate, 1000.0, 0.0, "lp4", true);
  // Set up the GUI
  gGui.setup(context->projectName);
  gGuiController.setup(&gGui, "Oscillator and Filter Controls");

  // Set ADSR parameters
  envelope.setAttackRate(gAttack * context->audioSampleRate);
  envelope.setDecayRate(gDecay * context->audioSampleRate);
  envelope.setReleaseRate(gRelease * context->audioSampleRate);
  envelope.setSustainLevel(gSustain);

  // Arguments: name, default value, minimum, maximum, increment
  // Create sliders for oscillator and filter settings
  gGuiController.addSlider("Sqrt(Num Partials)", 15, 1, 1000, 1);
  gGuiController.addSlider("Input Amplitude", 0.01, 0, 100.0, 0.01);
  gGuiController.addSlider("Fundmental Frequency", 30, 20, 500, 1);
  gGuiController.addSlider("Max Gain", 0.0, -5, 60, 0.1);
  gGuiController.addSlider("Slope Decay", 0.5, 0.0, 1.0, 0.01);
  gGuiController.addSlider("Aspect Ratio Lx/Ly", 0.5, 0.0, 1.0, 0.01);
  gGuiController.addSlider("Input Length (ms)", 5, 0.0, 200, 0.1);
  gGuiController.addSlider("Input ExpCoeff", 10.0, 5.0, 20.0, 0.1);
  gGuiController.addSlider("Pitch Glide", 500.0, 0.0, 150000.0, 100.0);
  gGuiController.addSlider("Filter Resonance", 0.9, 0.7, 1.0, 0.01);
  gGuiController.addSlider("Filter Type", 4, 1, 6, 1);
  gGuiController.addSlider("Attack time (s)", 0.1, 0.0, 0.5, 0.01);
  gGuiController.addSlider("Decay time (s)", 0.25, 0.0, 0.5, 0.01);
  gGuiController.addSlider("Release time (s)", 0.25, 0.0, 0.5, 0.01);
  gGuiController.addSlider("Sustain Level", 1.0, 0.0, 1.0, 0.01);
  gGuiController.addSlider("ADSR gate off (ms)", 100, 10, 1000, 1.0);
  gGuiController.addSlider("Pitch Glide exp factor", 0.1, 0.0, 0.1, 0.001);
  //   rt_printf("After Gui setup \n");
  // Set up the scope
  gScope.setup(2, context->audioSampleRate);

  return true;
}

void render(BelaContext *context, void *userData) {
  float gOutMax = 0.0;
  // // Read the slider values
  float npar = gGuiController.getSliderValue(0);
  float inAmplitude = gGuiController.getSliderValue(1);
  float resFrequency = gGuiController.getSliderValue(2);
  float maxGain = gGuiController.getSliderValue(3);
  float slopeDecay = gGuiController.getSliderValue(4);
  float ratio = gGuiController.getSliderValue(5);
  float inLength = gGuiController.getSliderValue(6);
  float expCoeff = gGuiController.getSliderValue(7);
  float glideFactor = gGuiController.getSliderValue(8);
  float filterResonance = gGuiController.getSliderValue(9);
  float filterType = gGuiController.getSliderValue(10);
  gAttack = gGuiController.getSliderValue(11);
  gDecay = gGuiController.getSliderValue(12);
  gRelease = gGuiController.getSliderValue(13);
  gSustain = gGuiController.getSliderValue(14);
  float gateTime = gGuiController.getSliderValue(15);
  float expFactor = gGuiController.getSliderValue(16);

  // // Set ADSR parameters
  // envelope.setAttackRate(gAttack * context->audioSampleRate);
  // envelope.setDecayRate(gDecay * context->audioSampleRate);
  // envelope.setReleaseRate(gRelease * context->audioSampleRate);
  // envelope.setSustainLevel(gSustain);

  // inAmplitude = map(inAmplitude, 0.0, 1.0, 0.0, 0.01);
  // slopeDecay = map(slopeDecay, 0.0, 1.0, 0.0, 0.000003);
  // maxDecay = map(maxDecay, 0.0, 1.0, 0.9999, 1.0);

  // // rt_printf("After Gui reads \n");

  // // Check for changes to avoid unnecesary computations

  
  // if (gRectM.getDecayMax() != maxDecay) {
  //   gRectM.setDecayMax(maxDecay);
  // }
  // if (gRectM.getDecaySlope() != slopeDecay) {
  //   gRectM.setDecaySlope(slopeDecay);
  // }
  

  if (gNoiseIn.getAmplitude() != inAmplitude) {
    gNoiseIn.setAmplitude(inAmplitude);
  }
  gNoiseIn.setLengthMs(inLength);
  gNoiseIn.setExponentialCoefficient(expCoeff);
  //   rt_printf("After if checks \n");

  // if (gPitchGlide.getPitchGlideFactor() != glideFactor) {
  //   gPitchGlide.setPitchGlideFactor(glideFactor);
  // }
  // if (gPitchGlide.getExpFactor() != expFactor) {
  //   gPitchGlide.setExpFactor(expFactor);
  // }
  // //
  // gNlinM.setFundamentalFrequencyHz(gNlinM.applyPitchGlide(gOutMax,gNlinM.getFundamentalFrequencyHz()));

  for (unsigned int n = 0; n < context->audioFrames; n++) {
    float timeElapsedMilliseconds =
        1000 * gSampleCounter /
        context->audioSampleRate;  // Since last event
                                   // rt_printf("timeElapsedMilliseconds: %f
                                   // \n", timeElapsedMilliseconds);

    float noise = 0.0;
    float out = 0.0;
    if (timeElapsedMilliseconds > gNoiseInterval) {

      if (gRectM.getFundamentalFrequencyHz() != resFrequency) {
    gRectM.setFundamentalFrequencyHz(resFrequency);
  }
  if (gRectM.getLengthRatio() != ratio) {
    gRectM.setLengthRatio(ratio);
  }
      //     // 	if (gNlinM.getPitchGlideFactor() != glideFactor) {
      //     //     gNlinM.setPitchGlideFactor(glideFactor);
      //     //   }
      //     if (gRectM.getNumPartialsPerDim() != npar) {
      //       gRectM.setNumPartialsPerDim(npar);
      //     }
      //     rt_printf(
      //         "################################ PARAM "
      //         "############################## \n");
      //     rt_printf("Fundamental Frequency: %f   \n", resFrequency);
      //     rt_printf("Lx/Ly ratio: %f   \n", ratio);
      //     rt_printf("Amplitude In: %f   \n", inAmplitude);
      //     //   rt_printf(
      //     //       "################################ NLIN  "
      //     //       "############################## \n");

      rt_printf("Max Out: %f   \n", gOutMax);
      //     // //   std::vector<float> freqs = gNlinM.getFrequenciesHz();
      // float fmax = *max_element(std::begin(freqs), std::end(freqs));
      //     // //   //   std::vector<float> decs = gNlinM.getDecays();
      //     // //   //   float decmax = *max_element(std::begin(decs),
      //     // std::end(decs));
      //     // //   rt_printf("freq0: %f \n", freqs[0]);
      //     // //   rt_printf("freqMax: %f \n", fmax);
      //     // //   rt_printf("Numpar: %i \n", gNlinM.getNumPartialsPerDim());
      //     // //   rt_printf("resBank Size: %i \n", gNlinM.getSize());
      //     // //   rt_printf("Decays size: %i \n", gNlinM.getDecays().size());
      //     // //   rt_printf("Frequencies size: %i \n", freqs.size());
      //     // //   rt_printf("Pitchglide: %f \n",
      //     gNlinM.getPitchGlideFactor());

      //     rt_printf(
      //         "################################ RECT  "
      //         "############################## \n");

      //     rt_printf("Max Out: %f   \n", gOutMax);
          std::vector<float> freqs = gRectM.getFrequenciesHz();
      //     float fmax = *max_element(std::begin(freqs), std::end(freqs));
      //     //   std::vector<float> decs = gRectM.getDecays();
      //     //   float decmax = *max_element(std::begin(decs), std::end(decs));
      //     rt_printf("freq0: %f \n", freqs[0]);
      //     rt_printf("freqMax: %f \n", fmax);
      //     rt_printf("Numpar: %i \n", gRectM.getNumPartialsPerDim());
      //     rt_printf("resBank Size: %i \n", gRectM.getSize());
      //     rt_printf("Decays size: %i \n", gRectM.getDecays().size());
      //     rt_printf("Frequencies size: %i \n", freqs.size());

      //     //   std::vector<float> decs = gRectM.getDecays();
      //       gResBank.setFrequenciesHz(freqs);
      //     //   gResBank.setDecays(decs);
      //     //   gRectM.setFrequenciesHz(freqs);
      //     //   gRectM.setDecays(decs);

      //     envelope.gate(true);
      //     gGateStatus = 1;

      //     //   if (gNlinM.getNumPartialsPerDim() != npar) {
      //     //     gNlinM.setNumPartialsPerDim(npar);
      //     //   }
      //     //   if (gNlinM.getFundamentalFrequencyHz() != resFrequency) {
      //     //     gNlinM.setFundamentalFrequencyHz(resFrequency);
      //     //   }
      //     //   if (gNlinM.getDecayMax() != maxDecay) {
      //     //     gNlinM.setDecayMax(maxDecay);
      //     //   }
      //     //   if (gNlinM.getDecaySlope() != slopeDecay) {
      //     //     gNlinM.setDecaySlope(slopeDecay);
      //     //   }
      //     //   if (gNlinM.getLengthRatio() != ratio) {
      //     //     gNlinM.setLengthRatio(ratio);
      //     //   }

      //     gFilter.setFilterConfiguration(gConfigs[(int)filterType]);
      gResBank.setFrequenciesHz(freqs);
      gResBank.setGainsDb(maxGain);
      gResBank.setResonances(npar);
      gOutMax = 0.0;
      gNoiseIn.trigger();
      //   rt_printf("Triggered \n");
      noise = gNoiseIn.process();
      gSampleCounter = 0;
    } else if (timeElapsedMilliseconds > gateTime && gGateStatus) {
      envelope.gate(false);
      gGateStatus = 0;
      noise = gNoiseIn.process();
      gSampleCounter++;
    } else {
      noise = gNoiseIn.process();
      gSampleCounter++;
    }

    //   // Set cutoff frequency for the filter

    //   // gFilter.setFrequencyHz(
    //   //     gPitchGlide.applyPitchGlide(noise,
    //   gRectM.getFundamentalFrequencyHz()));
    //   // if (gFilter.getFrequencyHz() != gRectM.getFundamentalFrequencyHz())
    //   {
    //   //   rt_printf("Filter Freq: %f \n", gFilter.getFrequencyHz());
    //   // }

    //   // Set resonance for the filter
    //   // gFilter.setResonance(filterResonance * envelope.process());
    out = gResBank.process(noise);
    //   // out = gRectM.process(noise);
    //   // out = gNlinM.process(noise);
    //   // rt_printf("Out: %f   \n", out);
    //   // Debug
    if (fabs(out) > gOutMax) {
      gOutMax = fabs(out);
    }
    out = tanhf_neon(out);
    //   // out = gFilter.process(out);
    //   // rt_printf("Envelope state: %i \n", envelope.getState());
    //   // out = 0.0;
    // Debug
    // float out = 0.0;
    // float noise = 0.0;
    // Write the output to every audio channel
    for (unsigned int channel = 0; channel < context->audioOutChannels;
         channel++) {
      audioWrite(context, n, channel, out);
    }

    gScope.log(noise, out);
  }
}

void cleanup(BelaContext *context, void *userData) {}
