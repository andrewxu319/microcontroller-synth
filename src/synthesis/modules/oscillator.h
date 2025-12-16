#pragma once

#include "module.h"

#include "utils/utils.h"

#include <string>

// idea for multiple voices: arrays of phase, freq, period, one for each voice

namespace synthesis {
	class Oscillator : public Module {
	public:
		double phase; // in wavetable indices
		float_s freq;  // in hz
		float_s gain;
		float_s wavetable[config::wavetable_resolution];
		Module* gain_mod;

		Oscillator(const string& wavetable_path);
		void load_wavetable(const string& path);
		void note_on(const uint8_t note, const uint8_t velocity);
		void note_off(const uint8_t note);
		void set_freq(const float_s value);
		void set_gain(const float_s value);

	private:
		double phase_increment;

		void generate_buf();
	};
}