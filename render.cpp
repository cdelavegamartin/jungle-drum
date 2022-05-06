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

// #include "BiquadResonatorBank.h"
#include "Excitation.h"
#include "JungleDrum.h"
#include "MoogVcf.h"
// #include "NonlinearMembrane.h"
// #include "RectangularMembrane.h"
#include "RectangularMembraneBq.h"
#include "PitchGlide.h"
// #include "ResonatorBank.h"
#include "PitchGlide.h"
#include "RectangularMembrane.h"
#include "ResonatorBank.h"

// Browser-based GUI to adjust parameters
Gui gGui;
GuiController gGuiController;

// Browser-based oscilloscope to visualise signal
// Scope gScope;

// resonator objects

// RectangularMembrane gRectM;
RectangularMembraneBq gRectMBq;

// NonlinearMembrane gNlinM;
BiquadResonatorBank gResBank;
PitchGlide gPitchGlide;

// ADSR envelope;  // ADSR envelope

float gAttack = 0.1;   // Envelope attack (seconds)
float gDecay = 0.25;   // Envelope decay (seconds)
float gRelease = 0.5;  // Envelope release (seconds)
float gSustain = 1.0;  // Envelope sustain level

JungleDrum gDrum;

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

// std::vector<float> freqsinit{100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

// Setup
bool setup(BelaContext *context, void *userData) {

  rt_printf("############ Start setup #####################\n");

  
  // gRectM.setup(context->audioSampleRate, 30, 0.9999f, 0.00001f);
  gRectMBq.setup(context->audioSampleRate, 30, 30.0f, 2.0f);
  // gNlinM.setup(context->audioSampleRate, 15, 0.9999f, 0.00001f);
  gPitchGlide.setup(500.0, 0.01);

  // std::vector<float> resonances(10, 20);
  // std::vector<float> gains(10, -5.0);
  // gResBank.setup(context->audioSampleRate, freqsinit, resonances, gains);

  gNoiseIn.setup(context->audioSampleRate, gNoiseLength, 5.0f);
  //   rt_printf("Length ms: %f \n", gNoiseIn.getLengthMs());

  gDrum.setup(context->audioSampleRate, 21, 0.9999f, 0.00001f);
  // Initialize the filter
  gFilter.setup(context->audioSampleRate, 1000.0, 0.0, "bp2", true);
  // Set up the GUI
  gGui.setup(context->projectName);
  gGuiController.setup(&gGui, "Oscillator and Filter Controls");

  // Set ADSR parameters
  // envelope.setAttackRate(gAttack * context->audioSampleRate);
  // envelope.setDecayRate(gDecay * context->audioSampleRate);
  // envelope.setReleaseRate(gRelease * context->audioSampleRate);
  // envelope.setSustainLevel(gSustain);
  // envelope.setTargetRatioDR(100.0);

  // Arguments: name, default value, minimum, maximum, increment
  // Create sliders for oscillator and filter settings
  gGuiController.addSlider("Num Partials per dim", 5, 1, 15, 1);
  gGuiController.addSlider("Input Amplitude", 5.0, 0, 20.0, 0.01);
  gGuiController.addSlider("Fundmental Frequency", 30, 20, 500, 1);
  gGuiController.addSlider("Gain Max (dB)", 50.0, 0.0, 80, 0.1);
  gGuiController.addSlider("Gain Slope (dB/(fpar/ffun-1))", 0.5, 0.0, 5.0,
                           0.01);
  gGuiController.addSlider("Aspect Ratio Lx/Ly", 0.5, 0.0, 1.0, 0.01);
  gGuiController.addSlider("Input Length (ms)", 5, 0.0, 200, 0.1);
  gGuiController.addSlider("Input ExpCoeff", 10.0, 5.0, 20.0, 0.1);
  gGuiController.addSlider("Pitch Glide", 0.0, 0.0, 2.0, 0.01);
  gGuiController.addSlider("Filter Resonance", 0.9, 0.7, 1.0, 0.01);
  gGuiController.addSlider("Filter Type", 4, 1, 6, 1);
  gGuiController.addSlider("Attack time (s)", 0.1, 0.0, 0.5, 0.01);
  gGuiController.addSlider("Decay time (s)", 0.25, 0.0, 0.5, 0.01);
  gGuiController.addSlider("Release time (s)", 0.25, 0.0, 0.5, 0.01);
  gGuiController.addSlider("Sustain Level", 1.0, 0.0, 1.0, 0.01);
  gGuiController.addSlider("ADSR gate off (ms)", 100, 10, 1000, 1.0);
  gGuiController.addSlider("Pitch Glide exp factor", 0.1, 0.0, 1.0, 0.001);
  gGuiController.addSlider("Filterbank Resonance", 100.0, 50.0, 300.0, 1.0);
    rt_printf("After Gui setup \n");
  // Set up the scope
  // gScope.setup(2, context->audioSampleRate);

  return true;
}

void render(BelaContext *context, void *userData) {
  float gOutMax = 0.0;
  // // Read the slider values
  float npar = gGuiController.getSliderValue(0);
  float inAmplitude = gGuiController.getSliderValue(1);
   float resFrequency = gGuiController.getSliderValue(2);
  // float midiNote = gGuiController.getSliderValue(2);
  
  float maxGain = gGuiController.getSliderValue(3);
  float slopeGain = gGuiController.getSliderValue(4);
 
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
  float resonance = gGuiController.getSliderValue(17);

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

  for (unsigned int n = 0; n < context->audioFrames; n++) {
    float timeElapsedMilliseconds =
        1000 * gSampleCounter /
        context->audioSampleRate;  // Since last event
                                   // rt_printf("timeElapsedMilliseconds: %f
                                   // \n", timeElapsedMilliseconds);

    float noise = 0.0;
    float out = 0.0;
    if (timeElapsedMilliseconds > gNoiseInterval) {
      if (gNoiseIn.getAmplitude() != inAmplitude) {
        gNoiseIn.setAmplitude(inAmplitude);
      }
      gNoiseIn.setLengthMs(inLength);
      gNoiseIn.setExponentialCoefficient(expCoeff);
      if (gRectMBq.getFundamentalFrequencyHz() != resFrequency) {
        gRectMBq.setFundamentalFrequencyHz(resFrequency);
      }
      if (gRectMBq.getLengthRatio() != ratio) {
        gRectMBq.setLengthRatio(ratio);
      }

      if (gRectMBq.getNumPartialsPerDim() != npar) {
        gRectMBq.setNumPartialsPerDim(npar);
      }

      if (gRectMBq.getGainDbMax() != maxGain) {
        gRectMBq.setGainDbMax(maxGain);
      }
      if (gRectMBq.getGainDbSlope() != slopeGain) {
        gRectMBq.setGainDbSlope(slopeGain);
      }
      if (gRectMBq.getResonance() != resonance) {
        gRectMBq.setResonance(resonance);
      }

      rt_printf(
          "################################ PARAM "
          "############################## \n");
      rt_printf("Fundamental Frequency: %f   \n", resFrequency);
      rt_printf("Lx/Ly ratio: %f   \n", ratio);
      rt_printf("Amplitude In: %f   \n", inAmplitude);
      rt_printf(
          "################################ MEMBQ  "
          "############################## \n");

      rt_printf("Max Out: %f   \n", gOutMax);
      std::vector<float> freqs = gRectMBq.getFrequenciesHz();
      float fmax = *max_element(std::begin(freqs), std::end(freqs));
      rt_printf("freq0: %f \n", freqs[0]);
      rt_printf("freqMax: %f \n", fmax);
      rt_printf("Numpar: %i \n", gRectMBq.getNumPartialsPerDim());

      rt_printf("resBank Size: %i \n", gRectMBq.getSize());
      rt_printf("Resonances size: %i \n", gRectMBq.getResonances().size());
      rt_printf("gains size: %i \n", gRectMBq.getGainsDb().size());
      rt_printf("Frequencies size: %i \n", freqs.size());
      // rt_printf("Pitchglide: %f \n", gRectMBq.getPitchGlideFactor());

      //     rt_printf(
      //         "################################ RECT  "
      //         "############################## \n");

      //     rt_printf("Max Out: %f   \n", gOutMax);
      // std::vector<float> freqs = gRectM.getFrequenciesHz();
      //     float fmax = *max_element(std::begin(freqs), std::end(freqs));
      //     //   std::vector<float> decs = gRectM.getDecays();
      //     //   float decmax = *max_element(std::begin(decs),
      //     std::end(decs)); rt_printf("freq0: %f \n", freqs[0]);
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

      //     gFilter.setFilterConfiguration(gConfigs[(int)filterType]);
      // gResBank.setFrequenciesHz(freqs);
      // gResBank.setGainsDb(maxGain);
      // gResBank.setResonances(npar);
      gOutMax = 0.0;
      // Check for changes to avoid unnecesary computations
      if (gDrum.getNumPartials() != npar) {
        gDrum.setNumPartials(npar);
      }
      
      if (gDrum.getDecayMax() != maxDecay) {
        gDrum.setDecayMax(maxDecay);
      }
      if (gDrum.getDecaySlope() != slopeDecay) {
        gDrum.setDecaySlope(slopeDecay);
      }
      if (gDrum.getLengthRatio() != ratio) {
        gDrum.setLengthRatio(ratio);
      }

      if (gDrum.getAmplitude() != inAmplitude) {
        gDrum.setAmplitude(inAmplitude);
      }
      gDrum.setLengthMs(inLength);
      gDrum.setExponentialCoefficient(expCoeff);
      //   rt_printf("After if checks \n");

      if (gDrum.getPitchGlideFactor() != glideFactor) {
        gDrum.setPitchGlideFactor(glideFactor);
      }
      if (gDrum.getExpFactor() != expFactor) {
        gDrum.setExpFactor(expFactor);
      }

      envelope.gate(true);
      gGateStatus = 1;

      gDrum.setFilterConfiguration(gConfigs[(int)filterType]);

      //   gOutMax = 0.0;
      gNoiseIn.trigger();
      //   rt_printf("Triggered \n");
      noise = gNoiseIn.process();
      gSampleCounter = 0;
    } else if (timeElapsedMilliseconds > gateTime && gGateStatus) {
      // envelope.gate(false);
      gDrum.noteOff();
      gGateStatus = 0;
      // noise = gNoiseIn.process();
      gSampleCounter++;
    } else {
      // noise = gNoiseIn.process();
      gSampleCounter++;
    }

    //   // Set cutoff frequency for the filter

    gFilter.setFrequencyHz(
        gPitchGlide.applyPitchGlide(noise, gRectM.getFundamentalFrequencyHz()));
    // if (gFilter.getFrequencyHz() != gRectM.getFundamentalFrequencyHz()) {
    //   rt_printf("Filter Freq: %f \n", gFilter.getFrequencyHz());
    // }

    //   // Set resonance for the filter
    //   // gFilter.setResonance(filterResonance * envelope.process());
    // out = gResBank.process(noise);
    out = gRectMBq.process(noise);
    //   // out = gNlinM.process(noise);
    //   // rt_printf("Out: %f   \n", out);
    //   // Debug
    if (fabs(out) > gOutMax) {
      gOutMax = fabs(out);
    }
    out = tanhf_neon(out);
    out = gFilter.process(out);

    // Write the output to every audio channel
    for (unsigned int channel = 0; channel < context->audioOutChannels;
         channel++) {
      audioWrite(context, n, channel, out);
    }

    // gScope.log(noise, out);
  }
}

void cleanup(BelaContext *context, void *userData) {}
