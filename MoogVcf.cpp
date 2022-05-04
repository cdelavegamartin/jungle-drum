// Implements the 4-stage Moog VCF ladder filter, including alternative configurations

#include <cmath>

#include <libraries/math_neon/math_neon.h>

#include "MoogVcf.h"

// Constructor taking arguments
MoogVcf::MoogVcf(float sampleRate, float frequencyHz, float resonance, 
            std::string filterConfig, bool useNonLin)
{
	setup(sampleRate, frequencyHz, resonance, filterConfig, useNonLin);
} 

void MoogVcf::setup(float sampleRate, float frequencyHz, float resonance, 
                    std::string filterConfig, bool useNonLin)
{
	inverseSampleRate_ = 1.0/sampleRate;  // Audio sample rate	
	frequencyHz_ = frequencyHz;			// Frequency of the filter
    resonance_ = resonance;
    useNonLin_ = useNonLin;             // Use tanh on the input
    lastStageOutput_ = 0.0;
    
    // Compensation gain is fixed by design
    compgain_ = 0.5;

    // initialize stages
    for (Filter1Order &filter : filterStages_) {
        filter.setup(sampleRate, frequencyHz);
    }


    
	// calculate the gain values for the different filter configurations 
    setFilterConfiguration(filterConfig);

	// Calculate the feedback gain for the resonance
	calculate_resgain_();


}

// Set the filter frequency
void MoogVcf::setFrequencyHz(float f)
{
	frequencyHz_ = f;

    // Update coeffs of the filter stages
	for (Filter1Order &filter : filterStages_) {
        filter.setFrequencyHz( frequencyHz_);
    }

    // The resonance gain polynomial interpolation exp also depends on 
    // the cutoff frequency and needs to be updated accordingly
    calculate_resgain_();
}

// Get the filter frequency
float MoogVcf::getFrequencyHz()
{
	return frequencyHz_;
}

// Set the filter resonance
void MoogVcf::setResonance(float res)
{
	resonance_ = res;
    
    calculate_resgain_();  
	
}

// Get the filter resonance
float MoogVcf::getResonance()
{
	return resonance_;
}

// Calculate gain coefficients, defaults to the lp4 configuration
// TODO: Refactor, How?
void MoogVcf::setFilterConfiguration(std::string filterConfig)
{
	
	if (filterConfig=="hp2")
    {
        outputGains_[0]=0.0;
        outputGains_[1]=-2.0;
        outputGains_[2]=1.0;
        outputGains_[3]=0.0;
        outputGains_[4]=0.0;
    }
    else if (filterConfig=="hp4")
    {
        outputGains_[0]=1.0;
        outputGains_[1]=-4.0;
        outputGains_[2]=6.0;
        outputGains_[3]=-4.0;
        outputGains_[4]=1.0;
    }
    else if (filterConfig=="bp2")
    {
        outputGains_[0]=0.0;
        outputGains_[1]=2.0;
        outputGains_[2]=-2.0;
        outputGains_[3]=0.0;
        outputGains_[4]=0.0;
    }
    else if (filterConfig=="bp4")
    {
        outputGains_[0]=0.0;
        outputGains_[1]=0.0;
        outputGains_[2]=4.0;
        outputGains_[3]=-8.0;
        outputGains_[4]=4.0;
    }
    else if (filterConfig=="lp2")
    {
        outputGains_[0]=0.0;
        outputGains_[1]=0.0;
        outputGains_[2]=1.0;
        outputGains_[3]=0.0;
        outputGains_[4]=0.0;
    }
    else if (filterConfig=="lp4")
    {
        outputGains_[0]=0.0;
        outputGains_[1]=0.0;
        outputGains_[2]=0.0;
        outputGains_[3]=0.0;
        outputGains_[4]=1.0;
    }
    else 
    {
        outputGains_[0]=0.0;
        outputGains_[1]=0.0;
        outputGains_[2]=0.0;
        outputGains_[3]=0.0;
        outputGains_[4]=1.0;
    }

	
}

std::vector<float> MoogVcf::getFilterConfiguration()
{
	return outputGains_;
}

void MoogVcf::calculate_resgain_()
{
	// Intermediate variable
    float frequencyAng = 2.0f*(float)M_PI*frequencyHz_*inverseSampleRate_;
    
    // Hardcoded value for plotting
    // resGain_ = 0.75;
    
    // calculate accurate resonance gain according to polynomial fit (from paper)
    resGain_ = resonance_*(1.0029f + 0.0526f*frequencyAng
                          - 0.0926f*powf(frequencyAng,2)
                          - 0.0218f*powf(frequencyAng,3));
	
}


// Get the next sample and update the phase
float MoogVcf::process(float in)
{
	// sample is used for holding the value through the processing
	float sample = in;

    // Output initialiazed to sum contributions
    float out = 0.0;

    // Add the contribution of the feedback loop
    sample = sample - 4.0f*resGain_*(lastStageOutput_-0.5f*sample);
    
    // The nonlinearity can be deactivated with this flag
    if (useNonLin_) {
        sample = tanhf_neon(sample);
    }

    for (int i = 0; i < filterStages_.size(); i++) {
        
        // Contribution to out from the node before current stage 
        out += outputGains_[i]*sample;
        // Process sample with filter stage
        sample = filterStages_[i].process(sample);
    }
    // Save output sample for the loop (notice last gain is afterwards) 
    lastStageOutput_ = sample;

    // Contribution from the cascaded filter stages
    out += outputGains_[4]*sample;

	return out;
}						