
#pragma once

class BiquadCoeff
{
public:
	typedef enum
	{
		lowpass,
		highpass,
		bandpass,
		notch,
		peak,
		lowshelf,
		highshelf
	} Type;
	struct Settings {
		float fs; ///< Sample rate in Hz
		Type type; ///< Filter type
		float cutoff; ///< Cutoff in Hz
		float q; ///< Quality factor
		float peakGainDb; ///< Maximum filter gain
	};
};

/**
 * A class to compute Biquad filter coefficents.
 */
template <typename T>
class BiquadCoeffT : public BiquadCoeff
{
public:
	typedef T sample_t;
	BiquadCoeffT() {}
	BiquadCoeffT(const Settings& settings) { setup(settings); }
	int setup(const Settings& settings)
	{
		type = settings.type;
		Fs = settings.fs;
		Fc = settings.cutoff / Fs;
		Q = settings.q;
		peakGain = settings.peakGainDb;
		calc();
		return 0;
	}

	void setType(Type type) {
		this->type = type;
		calc();
	}
	void setQ(sample_t Q) {
		this->Q = Q;
		calc();
	}
	void setFc(sample_t Fc) {
		this->Fc = Fc/this->Fs;
		calc();
	}
	void setPeakGain(sample_t peakGainDB) {
		this->peakGain = peakGainDB;
		calc();
	}

	Type getType() { return type; }
	sample_t getQ() { return Q; }
	sample_t getFc() { return Fc; }
	sample_t getPeakGain() { return peakGain; }

	sample_t a0, a1, a2, b1, b2;
private:
	sample_t Fc, Q, peakGain;
	sample_t Fs;
	Type type;
	void calc(void);
};

/**
 * A class to process biquad filters.
 *
 * These filters use `double` data types internally.
 */
class Biquad : public BiquadCoeffT<float> {
public:
	Biquad() {};
	Biquad(const Settings& s) {
		setup(s);
	}
	/**
	 * Process one input sample and return one output sample.
	 */
	sample_t process(sample_t in)
	{
		sample_t out = in * a0 + z1;
		z1 = in * a1 + z2 - b1 * out;
		z2 = in * a2 - b2 * out;
		return out;
	}

	/**
	 * Reset the internal state of the filter to 0;
	 */
	void clean()
	{
		z1 = z2 = 0.0f;
	}
protected:
	sample_t z1 = 0.0f;
	sample_t z2 = 0.0f;
};
extern template class BiquadCoeffT<float>;
