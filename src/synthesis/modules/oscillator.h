#pragma once

#include "module.h"

#include "utils/utils.h"

#include <string>

// idea for multiple voices: arrays of phase, freq, period, one for each voice

namespace synthesis {
	class Oscillator : public Module {
	public:
		float_s wavetable[config::wavetable_resolution];
		enum Mods {
			GAIN
		};

		Oscillator(const string& wavetable_path = "zeros", const bool unipolar = false);
		void generate_buf() override;
		void load_wavetable(const string& path, const bool unipolar = false);
		void note_on(const uint8_t note, const uint8_t velocity) override;
		void note_off() override;
		void set_freq(const float_s value);
		void set_gain(const float_s value);

	private:
		vector<Module*> mods[1];
		double phase; // in wavetable indices
		double phase_increment;
		float_s freq;  // in hz
		float_s gain;
		float_s velocity_gain;
	};
}