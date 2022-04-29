// ResonatorBank.h: header file for bank of Resonator objects
#pragma once

#include <vector>
#include "Resonator.h"

class ResonatorBank {
public:
	ResonatorBank() {}													// Default constructor
	ResonatorBank(float sampleRate, std::vector<float>& frequenciesHz, std::vector<float>& decays); 			// Constructor with arguments
 						
	
	void setup(float sampleRate, std::vector<float>& frequencies, std::vector<float>& decays);			// Set parameters
		
	
	void setFrequenciesHz(std::vector<float>& f);	// Set the ResonatorBank frequency
	std::vector<float> getFrequenciesHz();		// Get the ResonatorBank frequency
    void setDecays(std::vector<float>& r);	// Set the ResonatorBank decay
    void setDecays(float r);	// Set the ResonatorBank decay
	std::vector<float> getDecays();		// Get the ResonatorBank decay
    std::vector<Resonator>::size_type getSize();


    
	
	float process(float in);				// Process sample
	
	~ResonatorBank() {}				// Destructor

private:
	float sampleRate_;	        // audio sample rate
    std::vector<Resonator>::size_type nResonators_;	        // nummber of resonators
	std::vector<float> frequenciesHz_;			// Frequency of the ResonatorBank
    std::vector<float> decays_;			// Decay of the ResonatorBank
    std::vector<Resonator> resonators_;
	
};

