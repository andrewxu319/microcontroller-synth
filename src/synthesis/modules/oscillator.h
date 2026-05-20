#pragma once

#include "module.h"

#include <string>

// idea for multiple voices: arrays of phase, freq, period, one for each voice

namespace synthesis {
	class Oscillator : public Module {
	public:
		float_s waveform[config::waveform_resolution];
		enum BufType {
			GAIN,
			PITCH // IN CENTS
		};

		Oscillator(const std::string& waveform_path = "zeros", bool unipolar = false);
		void generate_buf() override;
		void load_waveform(const std::string& path, bool unipolar = false);
		void set_freq(const float_s value);
		void set_gain(const float_s value);
		void set_phase(const float_s value);

	protected:
		std::vector<const float_s*> in_bufs[2];
		float_s phase; // in waveform indices
		float_s phase_increment;
		float_s freq;  // in hz
		float_s gain;
		float_s velocity_gain;
		int8_t transpose; // semitones
		int8_t tune; // cents
		bool freq_changed; // freq changed mid-note

		static double get_semitone_shift_multiplier(const int8_t semitones);
	};
}