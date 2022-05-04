// MoogVcf.h: header file for first-order filter stage implementation

#pragma once

#include <string>
#include <vector>

#include "Filter1Order.h"

class MoogVcf {
public:
	MoogVcf() :  outputGains_(5), filterStages_(4) {}													// Default constructor
	MoogVcf(float sampleRate, float frequencyHz, float resonance, 
            std::string filterConfig="lp4", bool useNonLin=true); 			// Constructor with arguments
 						
	
	void setup(float sampleRate, float frequencyHz, float resonance, 
               std::string filterConfig="lp4", bool useNonLin=true);  			// Constructor with arguments
	
	void setFrequencyHz(float f);	// Set the filter frequency
	float getFrequencyHz();		// Get the filter frequency

    void setResonance(float res);	// Set the filter resonance
	float getResonance();		// Get the filter resonance

    void setFilterConfiguration(std::string filterConfig);	// Set the filter configuration
	std::vector<float> getFilterConfiguration();		// Get the filter output gains

    
	
	float process(float in);				// Process sample and update filter state
	
	~MoogVcf() {}				// Destructor

private:
	float inverseSampleRate_;	        // audio sample rate	
	float frequencyHz_;			// Frequency of the filter
    float resonance_;			// Resonance of the filter
    std::vector<float> outputGains_;
    float resGain_;
    float compgain_;
    float lastStageOutput_;
    bool useNonLin_;
    std::vector<Filter1Order> filterStages_;

	void calculate_resgain_();
	
};