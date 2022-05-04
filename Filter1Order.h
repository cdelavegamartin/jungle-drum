// Filter.h: header file for first-order filter stage implementation

#pragma once

class Filter1Order {
public:
	Filter1Order() {}													// Default constructor
	Filter1Order(float sampleRate, float frequencyHz); 			// Constructor with arguments
 						
	
	void setup(float sampleRate, float frequencyHz);			// Set parameters
		
	
	void setFrequencyHz(float f);	// Set the filter frequency
	float getFrequencyHz();		// Get the filter frequency

    
	
	float process(float in);				// Process sample and update filter state
	
	~Filter1Order() {}				// Destructor

private:
	float inverseSampleRate_;	        // inverse of the audio sample rate	
	float frequencyHz_;			// Frequency of the filter
    float b0_;
    float b1_;
    float a1_;
	float lastInputSample_;
	float lastOutputSample_;

	void calculate_coefficients_();
	
};