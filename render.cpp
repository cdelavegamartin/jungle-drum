#include <Bela.h>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>
#include <libraries/Midi/Midi.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "JungleDrum.h"

JungleDrum gDrum;

// Type of Control 0=GUI, 1=MIDI
bool gController = 1;

////////// GUI ////////////////
// // Browser-based GUI to adjust parameters
// Gui gGui;
// GuiController gGuiController;

// // Period for GUI controlled demo
// float gNoiseInterval = 2000.0;  // ms

// // samples ellapsed since last trigger measurement
// int gSampleCounter = 0;

// int gGateStatus = 0;  // Env on/off

/////////// MIDI ////////////////

Midi gMidi;

const char *gMidiPort0 = "hw:1,0,0";

enum gMidiControls {
  MRES = 1,
  SLOPE,
  INLEN,
  PGAMP,
  PGSHARP,
  VCFRES,
  VCFATT,
  VCFREL
};

// List of active notes for midi
const int kMaxNotes = 16;
int gActiveNotes[kMaxNotes];
int gActiveNoteCount = 0;

// Setup
bool setup(BelaContext *context, void *userData) {
  rt_printf("############ Start setup #####################\n");

  gDrum.setup(context->audioSampleRate);

  if (gController) {  // Initialise the MIDI device
    if (gMidi.readFrom(gMidiPort0) < 0) {
      rt_printf("Unable to read from MIDI port %s\n", gMidiPort0);
      return false;
    }
    gMidi.writeTo(gMidiPort0);
    gMidi.enableParser(true);

    // Fix parameters for MIDI setup (Not enough knobs)
    gDrum.setNumPartialsPerDim(10);
    gDrum.setLengthRatio(0.37);
    gDrum.setGainDbMax(60.0);
  }

  // else if (!gController) {
  //   // Set up the GUI
  //   gGui.setup(context->projectName);
  //   gGuiController.setup(&gGui, "Controls");

  //   // Arguments: name, default value, minimum, maximum, increment
  //   // Create sliders for settings

  //   gGuiController.addSlider("Midi Note", 33, 21, 69, 1);

  //   gGuiController.addSlider("Num Partials per dim", 8, 1, 15, 1);
  //   gGuiController.addSlider("Aspect Ratio Lx/Ly", 0.5, 0.0, 1.0, 0.01);
  //   gGuiController.addSlider("Filterbank Resonance", 100.0, 30.0,
  //   150.0, 1.0); gGuiController.addSlider("Partials gain Max (dB)", 50.0,
  //   0.0, 80, 0.1); gGuiController.addSlider("Partials gain Slope
  //   (dB/octave)", 5.0, 0.0, 10.0,
  //                            0.1);

  //   gGuiController.addSlider("Input Amplitude", 0.15, 0.0, 0.5, 0.001);
  //   gGuiController.addSlider("Input Length (ms)", 5.0, 0.0, 20, 0.05);

  //   gGuiController.addSlider("Pitch Glide Amplitude", 1.0, 0.0, 2.0, 0.01);
  //   gGuiController.addSlider("Pitch Glide Sharpness", 0.5, 0.0001, 1.0,
  //   0.001);

  //   gGuiController.addSlider("VCF Resonance", 0.95, 0.7, 1.0, 0.01);
  //   gGuiController.addSlider("Attack time (ms)", 10.0, 0.0, 200.0, 0.5);
  //   gGuiController.addSlider("Release time (ms)", 500.0, 0.0, 1000.0, 0.5);

  //   gGuiController.addSlider("Note off (ms)", 400, 10, 1000, 1.0);
  // }
  rt_printf("############ Finished Setup #####################\n");
  return true;
}

// void midiNoteOn(int noteNumber, int velocity) {
//   if (gActiveNoteCount < kMaxNotes) {
//     gActiveNotes[gActiveNoteCount] = noteNumber;
//     gActiveNoteCount++;
//     gDrum.setAmplitude(map(velocity, 1, 127, 0.001, 0.05));
//     gDrum.noteOn(noteNumber);
//   }
// }

// void midiNoteOff(int noteNumber){
//   bool activeNoteChanged = false;

//   for(int i = gActiveNoteCount-1; i>=0;i--){
//     if(gActiveNotes[i]==noteNumber){
//       if(i==gActiveNoteCount-1){
//         activeNoteChanged = true;
//       }
//       for(int j=i; j<gActiveNoteCount-1; j++){
//         gActiveNotes[j] = gActiveNotes[j+1];
//       }
//       gActiveNoteCount--;
//     }
//   }
//   if(activeNoteChanged){
//     gDrum.noteOff();
//   } 
// }

void render(BelaContext *context, void *userData) {
  float gOutMax = 0.0;

  if (gController) {
    // MIDI Control
    // At the beginning of each callback, look for available MIDI
    // messages that have come in since the last block
    while (gMidi.getParser()->numAvailableMessages() > 0) {
      MidiChannelMessage message;
      message = gMidi.getParser()->getNextChannelMessage();
      // message.prettyPrint();  // Print the message data

      // A MIDI "note on" message type might actually hold a real
      // note onset (e.g. key press), or it might hold a note off (key release).
      // The latter is signified by a velocity of 0.
      if (message.getType() == kmmNoteOn) {
        int noteNumber = message.getDataByte(0);
        int velocity = message.getDataByte(1);

        // Velocity of 0 is really a note off
        if (velocity == 0) {
          // midiNoteOff(noteNumber);
          gDrum.noteOff();
        } else {
          // midiNoteOn(noteNumber, velocity);
          // float inDb = map(velocity, 1, 127, -40, 0);
          // gDrum.setAmplitude(0.5 * powf(10.0, inDb / 20.0));
          gDrum.setAmplitude(map(velocity, 1, 127, 0.001, 0.05));
          gDrum.noteOn(noteNumber);
        }

      } else if (message.getType() == kmmNoteOff) {
        // We can also encounter the "note off" message type which is the same
        // as "note on" with a velocity of 0.

        gDrum.noteOff();

      } else if (message.getType() == kmmControlChange) {
        float data = message.getDataByte(1) / 127.0f;
        switch (message.getDataByte(0)) {
          case MRES:
            data = map(data, 0.0, 1.0, 30.0, 120.0);
            gDrum.setMembraneResonance(data);

            break;
          case SLOPE:
            data = map(data, 0.0, 1.0, 0.0, 10.0);
            gDrum.setGainDbSlope(data);

            break;
          case INLEN:
            data = map(data, 0.0, 1.0, 1.0, 20.0);
            gDrum.setLengthMs(data);

            break;
          case PGAMP:
            data = map(data, 0.0, 1.0, 0.0, 2.0);
            gDrum.setPitchGlideMax(data);

            break;
          case PGSHARP:
            data = map(data, 0.0, 1.0, 0.0001, 1.0);
            gDrum.setPitchGlideSharp(data);

            break;
          case VCFRES:
            data = map(data, 0.0, 1.0, 0.7, 1.05);
            gDrum.setFilterResonanceMax(data);

            break;
          case VCFATT:
            data = map(data, 0.0, 1.0, 0.01, 200.0);
            gDrum.setAttackTimeMs(data);

            break;
          case VCFREL:
            data = map(data, 0.0, 1.0, 10.0, 1000.0);
            gDrum.setReleaseTimeMs(data);

            break;
          case 20:

            gDrum.reset();

            break;
          default:
            rt_printf("Unassigned MIDI CC %i \n", message.getDataByte(0));
        }
      }
    }
  }

  // else if (!gController) {
  //   // GUI Control
  //   // Read the slider values
  //   float midiNote = gGuiController.getSliderValue(0);

  //   float npar = gGuiController.getSliderValue(1);
  //   float ratio = gGuiController.getSliderValue(2);
  //   float membraneResonance = gGuiController.getSliderValue(3);
  //   float maxGain = gGuiController.getSliderValue(4);
  //   float slopeGain = gGuiController.getSliderValue(5);

  //   float inAmplitude = gGuiController.getSliderValue(6);
  //   float inLength = gGuiController.getSliderValue(7);

  //   float glideFactor = gGuiController.getSliderValue(8);
  //   float expFactor = gGuiController.getSliderValue(9);

  //   float filterResonance = gGuiController.getSliderValue(10);
  //   float attackTimeMs = gGuiController.getSliderValue(11);
  //   float releaseTimeMs = gGuiController.getSliderValue(12);

  //   float noteOffTimeMs = gGuiController.getSliderValue(13);
  //   // These are only applied when the note triggers, setting only stores the
  //   // value

  //   gDrum.setNumPartialsPerDim(npar);
  //   gDrum.setLengthRatio(ratio);

  //   gDrum.setLengthMs(inLength);

  //   // These are fine to change at any time

  //   gDrum.setMembraneResonance(membraneResonance);
  //   gDrum.setGainDbMax(maxGain);
  //   gDrum.setGainDbSlope(slopeGain);

  //   gDrum.setAmplitude(inAmplitude);

  //   gDrum.setFilterResonanceMax(filterResonance);
  //   gDrum.setAttackTimeMs(attackTimeMs);
  //   gDrum.setReleaseTimeMs(releaseTimeMs);
  //   gDrum.setPitchGlideFactor(glideFactor);
  //   gDrum.setExpFactor(expFactor);
  // }

  // // rt_printf("After Gui reads \n");

  for (unsigned int n = 0; n < context->audioFrames; n++) {
    // if (!gController) {
    //   float timeElapsedMilliseconds =
    //       1000 * gSampleCounter / context->audioSampleRate;

    //   if (timeElapsedMilliseconds > gNoiseInterval) {
    //     // Stats for hit

    //     rt_printf(
    //         "################################ PARAM "
    //         "############################## \n");
    //     rt_printf("Midi Note: %f   \n", midiNote);
    //     rt_printf("Lx/Ly ratio: %f   \n", ratio);
    //     rt_printf("Amplitude In: %f   \n", inAmplitude);
    //     rt_printf(
    //         "################################ DRUM  "
    //         "############################## \n");

    //     std::vector<float> freqs = gDrum.getFrequenciesHz();
    //     float fmax = *max_element(std::begin(freqs), std::end(freqs));
    //     rt_printf("Midi note in jungleDrum: %i \n", gDrum.getMidiNote());
    //     rt_printf("freq0: %f \n", freqs[0]);
    //     rt_printf("freqMax: %f \n", fmax);
    //     rt_printf("Numpar: %i \n", gDrum.getNumPartialsPerDim());

    //     rt_printf("resBank Size: %i \n", gDrum.getSize());
    //     rt_printf("Resonances size: %i \n", gDrum.getResonances().size());
    //     rt_printf("gains size: %i \n", gDrum.getGainsDb().size());
    //     rt_printf("Frequencies size: %i \n", freqs.size());
    //     rt_printf("Pitchglide: %f \n", gDrum.getPitchGlideFactor());

    //     rt_printf("Prev Max Out: %f   \n", gOutMax);
    //     // Only applied when triggering, this should be moved inside
    //     JungleDrum
    //     // once it is working
    //

    //     gOutMax = 0.0;
    //     gGateStatus = 1;

    //     rt_printf("Triggered \n");
    //     gDrum.noteOn(midiNote);

    //     gSampleCounter = 0;
    //   } else if (timeElapsedMilliseconds > noteOffTimeMs && gGateStatus) {
    //     gDrum.noteOff();
    //     gGateStatus = 0;
    //     gSampleCounter++;
    //   } else {
    //     gSampleCounter++;
    //   }
    // }
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

  // rt_printf("Prev Max Out: %f   \n", gOutMax);
}

void cleanup(BelaContext *context, void *userData) {}
