// Resonator.h: header file for first-order resonator stage implementation

#pragma once

class Resonator {
public:
	Resonator() {}													// Default constructor
	Resonator(float sampleRate, float frequencyHz, float decay); 			// Constructor with arguments
 						
	
	void setup(float sampleRate, float frequencyHz, float decay);			// Set parameters
		
	
	void setFrequencyHz(float f);	// Set the resonator frequency
	float getFrequencyHz();		// Get the resonator frequency
    void setDecay(float r);	// Set the resonator decay
	float getDecay();		// Get the resonator decay

    
	
	float process(float in);				// Process sample and update resonator state
	
	~Resonator() {}				// Destructor

private:
	float inverseSampleRate_;	        // inverse of the audio sample rate	
	float frequencyHz_;			// Frequency of the resonator
    float decay_;			// Decay of the resonator
    float b0_;
    float a1_;
    float a2_;
	float lastOutputSample_;
    float lastlastOutputSample_;

	void calculate_coefficients_();
	
};