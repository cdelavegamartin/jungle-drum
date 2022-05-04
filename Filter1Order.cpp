// Filter1Order.cpp: file for implementing the 1st order filter stage

#include <cmath>
#include "Filter1Order.h"

// Constructor taking arguments for sample rate and table data
Filter1Order::Filter1Order(float sampleRate, float frequencyHz)
{
	setup(sampleRate, frequencyHz);
} 

void Filter1Order::setup(float sampleRate, float frequencyHz)
{
	// It's faster to multiply than to divide on most platforms,
	// therefore the inverse samplerate is stored
	inverseSampleRate_ = 1.0/sampleRate;	        // Audio sample rate	
	frequencyHz_ = frequencyHz;			// Frequency of the filter
    
	lastInputSample_ = 0.0;
	lastOutputSample_ = 0.0;

	// Calculate the coefficients
	calculate_coefficients_();


}

// Set the filter frequency
void Filter1Order::setFrequencyHz(float f)
{
	frequencyHz_ = f;
	calculate_coefficients_();
}

// Get the filter frequency
float Filter1Order::getFrequencyHz()
{
	return frequencyHz_;
}

// Calculate filter coefficients
void Filter1Order::calculate_coefficients_()
{
	
	float frequencyAng = 2.0*M_PI*frequencyHz_*inverseSampleRate_;
	
	// 4-th order polynomial approximation
	float g = (0.9892*frequencyAng 
					- 0.4342*powf(frequencyAng,2) 
					+ 0.1381*powf(frequencyAng,3) 
					- 0.0202*powf(frequencyAng,4));
	b0_ = g/1.3;
    b1_ = g*0.3/1.3;
    a1_ = g-1.0;;
	
	
}
	
// Get the next sample and update the phase
float Filter1Order::process(float in)
{
	// Calculate output sample
	float out = b0_*in + b1_*lastInputSample_ - a1_*lastOutputSample_;
	// Save lsat input and output 
	lastOutputSample_ = out, lastInputSample_ = in;
	return out;
}			