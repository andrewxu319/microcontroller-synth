#pragma once

#include "module.h"

#include "utils/utils.h"

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

		Oscillator(const std::string& waveform_path = "zeros", const bool unipolar = false);
		void generate_buf() override;
		void load_waveform(const std::string& path, const bool unipolar = false);
		void note_on(const uint8_t note, const uint8_t velocity) override;
		void note_off() override;
		void set_freq(const float_s value);
		void set_gain(const float_s value);
		void set_phase(const float_s value);

	protected:
		std::vector<const float_s*> in_bufs[2];

	private:
		bool on;
		float_s phase; // in waveform indices
		float_s phase_increment;
		float_s freq;  // in hz
		float_s gain;
		float_s velocity_gain;
		int8_t transpose; // semitones
		int8_t tune; // cents

		static double get_semitone_shift_multiplier(const int8_t semitones);
	};
}