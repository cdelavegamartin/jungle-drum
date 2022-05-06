#include <Bela.h>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>
#include <libraries/Scope/Scope.h>

#include <cmath>
#include <vector>
#include <algorithm>

#include "JungleDrum.h"


// Browser-based GUI to adjust parameters
Gui gGui;
GuiController gGuiController;

// Browser-based oscilloscope to visualise signal
// Scope gScope;

JungleDrum gDrum;

int gGateStatus = 0;  // Env on/off

// White noise
float gNoiseLength = 5.0;       // ms
float gNoiseInterval = 2000.0;  // ms

// samples ellapsed since last accelerometer measurement
int gSampleCounter = 0;

// Setup
bool setup(BelaContext *context, void *userData) {
  rt_printf("############ Start setup #####################\n");

  gDrum.setup(context->audioSampleRate);

  // Set up the GUI
  gGui.setup(context->projectName);
  gGuiController.setup(&gGui, "Controls");

  // Arguments: name, default value, minimum, maximum, increment
  // Create sliders for settings

  gGuiController.addSlider("Midi Note", 33, 21, 69, 1);

  gGuiController.addSlider("Num Partials per dim", 8, 1, 15, 1);
  gGuiController.addSlider("Aspect Ratio Lx/Ly", 0.5, 0.0, 1.0, 0.01);
  gGuiController.addSlider("Filterbank Resonance", 100.0, 30.0, 150.0, 1.0);
  gGuiController.addSlider("Partials gain Max (dB)", 50.0, 0.0, 80, 0.1);
  gGuiController.addSlider("Partials gain Slope (dB/octave)", 5.0, 0.0, 10.0,
                           0.1);

  gGuiController.addSlider("Input Amplitude", 0.15, 0.0, 0.5, 0.001);
  gGuiController.addSlider("Input Length (ms)", 5.0, 0.0, 20, 0.05);

  gGuiController.addSlider("Pitch Glide Amplitude", 1.0, 0.0, 2.0, 0.01);
  gGuiController.addSlider("Pitch Glide Sharpness", 0.5, 0.0001, 1.0, 0.001);

  gGuiController.addSlider("VCF Resonance", 0.95, 0.7, 1.0, 0.01);
  gGuiController.addSlider("Attack time (ms)", 10.0, 0.0, 200.0, 0.5);
  gGuiController.addSlider("Release time (ms)", 500.0, 0.0, 1000.0, 0.5);

  gGuiController.addSlider("Note off (ms)", 400, 10, 1000, 1.0);

  rt_printf("After Gui setup \n");
  // Set up the scope
  // gScope.setup(2, context->audioSampleRate);

  return true;
}

void render(BelaContext *context, void *userData) {
  float gOutMax = 0.0;
  // // Read the slider values
  float midiNote = gGuiController.getSliderValue(0);

  float npar = gGuiController.getSliderValue(1);
  float ratio = gGuiController.getSliderValue(2);
  float membraneResonance = gGuiController.getSliderValue(3);
  float maxGain = gGuiController.getSliderValue(4);
  float slopeGain = gGuiController.getSliderValue(5);

  float inAmplitude = gGuiController.getSliderValue(6);
  float inLength = gGuiController.getSliderValue(7);

  float glideFactor = gGuiController.getSliderValue(8);
  float expFactor = gGuiController.getSliderValue(9);

  float filterResonance = gGuiController.getSliderValue(10);
  float attackTimeMs = gGuiController.getSliderValue(11);
  float releaseTimeMs = gGuiController.getSliderValue(12);

  float noteOffTimeMs = gGuiController.getSliderValue(13);

  // These are only applied when the note triggers, setting only stores the
  // value

  gDrum.setNumPartialsPerDim(npar);
  gDrum.setLengthRatio(ratio);

  gDrum.setLengthMs(inLength);

  // These are fine to change at any time

  gDrum.setMembraneResonance(membraneResonance);
  gDrum.setGainDbMax(maxGain);
  gDrum.setGainDbSlope(slopeGain);

  gDrum.setAmplitude(inAmplitude);

  gDrum.setFilterResonanceMax(filterResonance);
  gDrum.setAttackTimeMs(attackTimeMs);
  gDrum.setReleaseTimeMs(releaseTimeMs);

  // // rt_printf("After Gui reads \n");

  for (unsigned int n = 0; n < context->audioFrames; n++) {
    float timeElapsedMilliseconds =
        1000 * gSampleCounter /
        context->audioSampleRate;

    if (timeElapsedMilliseconds > gNoiseInterval) {
      // Stats for hit

      rt_printf(
          "################################ PARAM "
          "############################## \n");
      rt_printf("Midi Note: %f   \n", midiNote);
      rt_printf("Lx/Ly ratio: %f   \n", ratio);
      rt_printf("Amplitude In: %f   \n", inAmplitude);
      rt_printf(
          "################################ DRUM  "
          "############################## \n");

      std::vector<float> freqs = gDrum.getFrequenciesHz();
      float fmax = *max_element(std::begin(freqs), std::end(freqs));
      rt_printf("Midi note in jungleDrum: %i \n", gDrum.getMidiNote());
      rt_printf("freq0: %f \n", freqs[0]);
      rt_printf("freqMax: %f \n", fmax);
      rt_printf("Numpar: %i \n", gDrum.getNumPartialsPerDim());

      rt_printf("resBank Size: %i \n", gDrum.getSize());
      rt_printf("Resonances size: %i \n", gDrum.getResonances().size());
      rt_printf("gains size: %i \n", gDrum.getGainsDb().size());
      rt_printf("Frequencies size: %i \n", freqs.size());
      rt_printf("Pitchglide: %f \n", gDrum.getPitchGlideFactor());

      rt_printf("Prev Max Out: %f   \n", gOutMax);
      // Only applied when triggering, this should be moved inside JungleDrum
      // once it is working
      gDrum.setPitchGlideFactor(glideFactor);
      gDrum.setExpFactor(expFactor);

      gOutMax = 0.0;
      gGateStatus = 1;

      rt_printf("Triggered \n");
      gDrum.noteOn(midiNote);

      gSampleCounter = 0;
    } else if (timeElapsedMilliseconds > noteOffTimeMs && gGateStatus) {
      gDrum.noteOff();
      gGateStatus = 0;
      gSampleCounter++;
    } else {
      gSampleCounter++;
    }

    float out = gDrum.process();

    //   // Debug
    if (fabs(out) > gOutMax) {
      gOutMax = fabs(out);
    }

    // Write the output to every audio channel
    for (unsigned int channel = 0; channel < context->audioOutChannels;
         channel++) {
      audioWrite(context, n, channel, out);
    }

    // gScope.log(noise, out);
  }
}

void cleanup(BelaContext *context, void *userData) {}
