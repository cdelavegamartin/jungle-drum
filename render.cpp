#include <Bela.h>
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

#include "Excitation.h"
#include "RectangularMembrane.h"
#include "ResonatorBank.h"

// Browser-based GUI to adjust parameters
Gui gGui;
GuiController gGuiController;

// Browser-based oscilloscope to visualise signal
Scope gScope;

// resonator objects

RectangularMembrane gRectM;
ResonatorBank gResBank;

// Input Object
Excitation gNoiseIn;

// White noise
float gNoiseLength = 5;       // ms
float gNoiseInterval = 1000;  // ms

// samples ellapsed since last accelerometer measurement
int gSampleCounter = 0;

// Test push from Bela VSCode
bool setup(BelaContext *context, void *userData) {
  gRectM.setup(context->audioSampleRate, 10, 0.9999, 0.00001);
  std::vector<float> freqs(100, 0.0);
  std::vector<float> decays(100, 0.9999);
  gResBank.setup(context->audioSampleRate, freqs, decays);

  gNoiseIn.setup(context->audioSampleRate, gNoiseLength);
//   rt_printf("Length ms: %f \n", gNoiseIn.getLengthMs());
  // Set up the GUI
  gGui.setup(context->projectName);
  gGuiController.setup(&gGui, "Oscillator and Filter Controls");

  // Arguments: name, default value, minimum, maximum, increment
  // Create sliders for oscillator and filter settings
  gGuiController.addSlider("Sqrt(Num Partials)", 10, 5, 20, 1);
  gGuiController.addSlider("Input Amplitude", 0.1, 0, 1.0, 0.01);
  gGuiController.addSlider("Fundmental Frequency", 30, 20, 500, 1);
  gGuiController.addSlider("Max Decay", 0.5, 0.0, 1.0, 0.01);
  gGuiController.addSlider("Slope Decay", 0.5, 0.0, 1.0, 0.01);
  gGuiController.addSlider("Aspect Ratio Lx/Ly", 0.5, 0.0, 1.0, 0.01);
  gGuiController.addSlider("Input Length (ms)", 5, 0.0, 20, 0.1);
  gGuiController.addSlider("Input ExpCoeff", 10.0, 5.0, 20.0, 0.1);
//   rt_printf("After Gui setup \n");
  // Set up the scope
  gScope.setup(2, context->audioSampleRate);

  return true;
}

void render(BelaContext *context, void *userData) {
//   float gOutMax = 0.0;
  // Read the slider values
  float npar = gGuiController.getSliderValue(0);
  float inAmplitude = gGuiController.getSliderValue(1);
  float resFrequency = gGuiController.getSliderValue(2);
  float maxDecay = gGuiController.getSliderValue(3);
  float slopeDecay = gGuiController.getSliderValue(4);
  float ratio = gGuiController.getSliderValue(5);
  float inLength = gGuiController.getSliderValue(6);
  float expCoeff = gGuiController.getSliderValue(7);

  inAmplitude = map(inAmplitude, 0.0, 1.0, 0.0, 0.01);
  slopeDecay = map(slopeDecay, 0.0, 1.0, 0.0, 0.000001);
  maxDecay = map(maxDecay, 0.0, 1.0, 0.9999, 1.0);
  // rt_printf("After Gui reads \n");

  // Check for changes to avoid unnecesary computations
  if (gRectM.getNumPartials() != npar) {
    gRectM.setNumPartials(npar);
  }
  if (gRectM.getFundamentalFrequencyHz() != resFrequency) {
    gRectM.setFundamentalFrequencyHz(resFrequency);
  }
  if (gRectM.getDecayMax() != maxDecay) {
    gRectM.setDecayMax(maxDecay);
  }
  if (gRectM.getDecaySlope() != slopeDecay) {
    gRectM.setDecaySlope(slopeDecay);
  }
  if (gRectM.getLengthRatio() != ratio) {
    gRectM.setLengthRatio(ratio);
  }
  if (gNoiseIn.getAmplitude() != inAmplitude) {
    gNoiseIn.setAmplitude(inAmplitude);
  }
  //   rt_printf("After if checks \n");
  for (unsigned int n = 0; n < context->audioFrames; n++) {
    float timeElapsedMilliseconds =
        1000 * gSampleCounter /
        context->audioSampleRate;  // Since last event
                                   // rt_printf("timeElapsedMilliseconds: %f
                                   // \n", timeElapsedMilliseconds);

    float noise = 0.0;
    float out = 0.0;
    if (timeElapsedMilliseconds > gNoiseInterval) {
    //   rt_printf("Fundamental Frequency: %f   \n", resFrequency);
    //   rt_printf("Lx/Ly ratio: %f   \n", ratio);
    //   rt_printf("Amplitude In: %f   \n", inAmplitude);
    //   rt_printf("Max Out: %f   \n", gOutMax);
    //   std::vector<float> freqs = gRectM.getFrequenciesHz();
    //   float fmax = *max_element(std::begin(freqs), std::end(freqs));
	// //   std::vector<float> decs = gRectM.getDecays();
	// //   float decmax = *max_element(std::begin(decs), std::end(decs));
    //   rt_printf("freq0: %f \n", freqs[0]);
    //   rt_printf("freqMax: %f \n", fmax);
	//   rt_printf("Numpar: %i \n", gRectM.getNumPartials());
	//   rt_printf("resBank Size: %i \n", gRectM.getSize());
	//   rt_printf("Decays size: %i \n", gRectM.getDecays().size());
	//   rt_printf("Frequencies size: %i \n", freqs.size());


    //   std::vector<float> decs = gRectM.getDecays();
    //   gResBank.setFrequenciesHz(freqs);
    //   gResBank.setDecays(decs);
	//   gRectM.setFrequenciesHz(freqs);
    //   gRectM.setDecays(decs);
	  
      if (gNoiseIn.getLengthMs() != inLength) {
        gNoiseIn.setLengthMs(inLength);
      }
      if (gNoiseIn.getExponentialCoefficient() != expCoeff) {
        gNoiseIn.setExponentialCoefficient(expCoeff);
      }
    //   gOutMax = 0.0;
      gNoiseIn.trigger();
    //   rt_printf("Triggered \n");
      noise = gNoiseIn.process();
      gSampleCounter = 0;
    } else {
      noise = gNoiseIn.process();
      gSampleCounter++;
    }

    // out = gResBank.process(noise);
    // out = gRectM.process(0.0);
    out = gRectM.process(noise);

    // Debug
    // if (fabs(out) > gOutMax) {
    //   gOutMax = fabs(out);
    // }
    out = tanhf_neon(out);

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
