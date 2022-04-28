#include <Bela.h>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>
#include <libraries/Scope/Scope.h>
#include <libraries/math_neon/math_neon.h>
#include <cmath>
#include <string>
#include <map>
#include <iostream>
#include "Wavetable.h"
#include "Resonator.h"

// Browser-based GUI to adjust parameters
Gui gGui;
GuiController gGuiController;

// Browser-based oscilloscope to visualise signal
Scope gScope;

// Oscillator objects
Wavetable gSineOscillator, gSawtoothOscillator;

//resonator objects
Resonator gResonator;

// White noise var for testing
gNoiseLength = 10; // ms
gNoiseInterval = 1000; //ms
gNoiseOn = 0;

//Test push from Bela VSCode
bool setup(BelaContext *context, void *userData)
{
	std::vector<float> wavetable;
	const unsigned int wavetableSize = 1024;
		
	// Populate a buffer with the first 64 harmonics of a sawtooth wave
	wavetable.resize(wavetableSize);
	
	// Generate a sawtooth wavetable (a ramp from -1 to 1)
	for(unsigned int n = 0; n < wavetableSize; n++) {
		wavetable[n] = -1.0 + 2.0 * (float)n / (float)(wavetableSize - 1);
	}
	
	// Initialise the sawtooth wavetable, passing the sample rate and the buffer
	gSawtoothOscillator.setup(context->audioSampleRate, wavetable);

	// Recalculate the wavetable for a sine
	for(unsigned int n = 0; n < wavetableSize; n++) {
		wavetable[n] = sin(2.0 * M_PI * (float)n / (float)wavetableSize);
	}	
	
	// Initialise the sine oscillator
	gSineOscillator.setup(context->audioSampleRate, wavetable);

	// Initialize the filter
	gResonator.setup(context->audioSampleRate, 1000.0, 0.9);
	
	// Debug for filter types
	// ogains_orig = gFilter.getFilterConfiguration();
	// for (auto i: ogains_orig){
	// 		std::cout << i << ' ' << std::endl;
	// 	}

	// Set up the GUI
	gGui.setup(context->projectName);
	gGuiController.setup(&gGui, "Oscillator and Filter Controls");	
	
	// Arguments: name, default value, minimum, maximum, increment
	// Create sliders for oscillator and filter settings
	gGuiController.addSlider("Oscillator Frequency", 3000, 2000, 5000, 200);
	gGuiController.addSlider("Oscillator Amplitude", 0.1, 0, 2.0, 0.1);
	gGuiController.addSlider("Resonator Frequency", 1000, 500, 5000, 500);
	gGuiController.addSlider("Resonator Decay", 0.9, 0.0, 1.0, 0.05);
	
	
	
	// Set up the scope
	gScope.setup(2, context->audioSampleRate);

	return true;
}

void render(BelaContext *context, void *userData)
{
	// Read the slider values
	float oscFrequency = gGuiController.getSliderValue(0);	
	float oscAmplitude = gGuiController.getSliderValue(1);
	float resFrequency = gGuiController.getSliderValue(2);
	float resDecay = gGuiController.getSliderValue(3);
	

	// Set the oscillator frequency
	gSineOscillator.setFrequency(oscFrequency);
	gSawtoothOscillator.setFrequency(oscFrequency);

	// Set cutoff frequency for the filter
	gResonator.setFrequencyHz(resFrequency);
	
	// Set resonance for the filter
	gResonator.setDecay(resDecay);
	
	
    for(unsigned int n = 0; n < context->audioFrames; n++) {
    	// Uncomment one line or the other to choose sine or sawtooth oscillator
		float in = oscAmplitude * gSineOscillator.process();
		// float in = oscAmplitude * gSawtoothOscillator.process();

		if(1000*gSampleCounter/context->audioSampleRate>gNoise){
		// Generate white noise: random values between -1 and 1
		float noise = 2.0 * (float)rand() / (float)RAND_MAX - 1.0;

		// Process the input with the filter
		float out = gFilter.process(in);

        
        // Write the output to every audio channel
    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
    		audioWrite(context, n, channel, out);
    	}
    	
    	gScope.log(in, out);
    }
}

void cleanup(BelaContext *context, void *userData)
{

}
