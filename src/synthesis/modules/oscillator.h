#pragma once

#include "module.h"

#include <string>

// idea for multiple voices: arrays of phase, freq, period, one for each voice

namespace synthesis {
	class Oscillator : public Module {
	public:
		int phase; // in samples
		float32_t freq;  // in hz
		float32_t period; // in samples
		float32_t wavetable[config::wavetable_resolution];

		Oscillator(const vector<Module*> outputs_, const string& wavetable_path);
		void set_freq(const float32_t val);
		void load_wavetable(const string& path);

	private:
		void generate_buf();
	};
}