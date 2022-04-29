// ResonatorBank.cpp: file for implementing a 2nd Order resonating IIR filter
#pragma once

#include <cmath>
#include <vector>
#include "ResonatorBank.h"

// Constructor taking arguments for sample rate and table data
ResonatorBank::ResonatorBank(float sampleRate, std::vector<float>& frequenciesHz, std::vector<float>& decays)
{
	setup(sampleRate, frequenciesHz, decays);
} 

void ResonatorBank::setup(float sampleRate, std::vector<float>& frequenciesHz, std::vector<float>& decays)
{
	sampleRate_ = sampleRate;
    nResonators_ = frequenciesHz.size();	
	frequenciesHz_ = frequenciesHz;			// Frequencies of the resonators
    decays_ = decays;			// Frequencies of the resonators
    resonators_.resize(nResonators_);

    setFrequenciesHz(frequenciesHz);
    setDecays(decays);
    for (int i = 0; i < nResonators_; i++)
    {
        resonators_[i].setup(sampleRate_,frequenciesHz_[i], decays_[i]);
    }
	
    


}

// Set the ResonatorBank frequencies
void ResonatorBank::setFrequenciesHz(std::vector<float>& frequenciesHz)
{
	frequenciesHz_ = frequenciesHz;
	for (int i = 0; i < nResonators_; i++)
    {
        resonators_[i].setFrequencyHz(frequenciesHz_[i]);
    }
}

// Get the ResonatorBank frequencies
std::vector<float> ResonatorBank::getFrequenciesHz()
{
	return frequenciesHz_;
}

// Set the ResonatorBank decays
void ResonatorBank::setDecays(std::vector<float>& decays)
{
	decays_ = decays;
	for (int i = 0; i < nResonators_; i++)
    {
        resonators_[i].setDecay(decays_[i]);  
    }
}

// Overloaded function with just a float
void ResonatorBank::setDecays(float decay)
{
	for (int i = 0; i < nResonators_; i++)
    {
        decays_[i] = decay;
        resonators_[i].setDecay(decay);  
    }
}

// Get the ResonatorBank decays
std::vector<float> ResonatorBank::getDecays()
{
	return decays_;
}


std::vector<Resonator>::size_type ResonatorBank::getSize()
{
	return nResonators_;
}

	
// Process next sample 
float ResonatorBank::process(float in)
{
	// Calculate output sample
	float out = 0;
	for (int i = 0; i < nResonators_; i++)
    {
        out += resonators_[i].process(in);
    }
    out /= (float)nResonators_;
	return out;
}			