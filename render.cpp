#include <Bela.h>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>
#include <libraries/Scope/Scope.h>
#include <libraries/math_neon/math_neon.h>
#include <cmath>
#include <string>
#include <map>
// #include <iostream>
#include <algorithm>
#include "Wavetable.h"
#include "Resonator.h"
#include "ResonatorBank.h"

// Browser-based GUI to adjust parameters
Gui gGui;
GuiController gGuiController;

// Browser-based oscilloscope to visualise signal
Scope gScope;

// Oscillator objects
Wavetable gSineOscillator, gSawtoothOscillator;

//resonator objects
Resonator gResonator;
ResonatorBank gBank;

// White noise var for testing
float gNoiseLength = 5; // ms
float gNoiseInterval = 1000; //ms
float gNoiseOn = 0;

// samples ellapsed since last accelerometer measurement
int gSampleCounter = 0;

std::vector<float> generateHarmonics(float fundamental, int nHarmonics){
	std::vector<float> freqs(nHarmonics, 0.0);
	// freqs.reserve(nHarmonics);
	for (int i = 0; i < nHarmonics; i++)
    {
        float coeff = (float)(i+1) + 0.01*(float)(i+1);
		freqs[i] = coeff * fundamental;
    }
	return freqs;
}
std::vector<float> generatePartials(float fundamental, int n, int m, float A=1.1, float B=0.9){
	std::vector<float> freqs(n*m, 0.0);
	// freqs.reserve(nHarmonics);
	for (int i = 0; i < n; i++)
    {
        for (int j= 0; j < m; j++){
			float coeff =  sqrtf(powf((float)(i+1)/A,2) + powf((float)(j+1)/B,2));
			freqs[i*m+j] = coeff * fundamental;
		}
		
    }
	return freqs;
}

//decays
int gNResonators = 100;
float default_decay = 0.999;
std::vector<float> decays(gNResonators, default_decay);
std::vector<float> fs = generatePartials(100, 10, 10);


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
	
	// Initialize filterbank
	gBank.setup(context->audioSampleRate, fs, decays);
	// Debug
	rt_printf("Size of bank: %i   ",gBank.getSize() );
	rt_printf("Size of fs: %i   ",fs.size() );
	rt_printf("Size of decays: %i   ",decays.size() );
	
	// Set up the GUI
	gGui.setup(context->projectName);
	gGuiController.setup(&gGui, "Oscillator and Filter Controls");	
	
	// Arguments: name, default value, minimum, maximum, increment
	// Create sliders for oscillator and filter settings
	gGuiController.addSlider("Oscillator Frequency", 50, 50, 3000, 10);
	gGuiController.addSlider("Oscillator Amplitude", 0.1, 0, 2.0, 0.1);
	gGuiController.addSlider("Resonator Frequency", 20, 20, 500, 1);
	gGuiController.addSlider("Resonator Decay", 0.999, 0.999, 0.99999, 0.00001);
	
	
	
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
	gSineOscillator.setFrequency(resFrequency);
	gSawtoothOscillator.setFrequency(resFrequency);

	// // Set cutoff frequency for the filter
	// gResonator.setFrequencyHz(resFrequency);
	
	// // Set resonance for the filter
	// gResonator.setDecay(resDecay);

	// Set cutoff frequency for the filter
	// std::vector<float> freqs = generateHarmonics(resFrequency, (int)gBank.getSize());
	std::vector<float> freqs = generatePartials(resFrequency, 10, 10);
	gBank.setFrequenciesHz(freqs);
	
	// Set resonance for the filter
	gBank.setDecays(resDecay);
	
	
    for(unsigned int n = 0; n < context->audioFrames; n++) {
    	// Uncomment one line or the other to choose sine or sawtooth oscillator
		float in = oscAmplitude * gSineOscillator.process();
		// float in = oscAmplitude * gSawtoothOscillator.process();
		
		//  Noise generator
		//TODO: Needs to become it's own object
		float noise = 0.0;
		float timeElapsedMilliseconds = 1000*gSampleCounter/context->audioSampleRate; // Since last event
		if(timeElapsedMilliseconds<gNoiseInterval){
		// Generate white noise: random values between -1 and 1
			if (timeElapsedMilliseconds<gNoiseLength)
			{
				noise = 2.0 * (float)rand() / (float)RAND_MAX - 1.0;
				// noise = gSineOscillator.process();

			}
			else{noise = 0.0;}
			gSampleCounter++;
		}
		else{
			gSampleCounter=0;
			}	
		// Process the input with the filter
		// float out = gResonator.process(in);
		noise*=oscAmplitude;
		float out = gBank.process(noise);
		// Debug
		// rt_printf("Size of bank: %f   ",gBank.getSize() );

        
        // Write the output to every audio channel
    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
    		audioWrite(context, n, channel, out);
    	}
    	
    	gScope.log(noise, out);
    }
}

void cleanup(BelaContext *context, void *userData)
{

}
