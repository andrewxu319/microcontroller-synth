#pragma once

#include "module.h"

#include "utils/utils.h"

#include <string>

// idea for multiple voices: arrays of phase, freq, period, one for each voice

namespace synthesis {
	class Oscillator : public Module {
	public:
		int phase; // in samples
		float_s freq;  // in hz
		float_s gain;
		float_s period; // in samples
		float_s wavetable[config::wavetable_resolution];

		Oscillator(const string& wavetable_path);
		void load_wavetable(const string& path);
		void set_freq(const float_s value);
		void set_gain(const float_s value);

	private:
		void generate_buf();
	};
}