#pragma once

#include "oscillator.h"

#include "utils/utils.h"

namespace synthesis {
	class PerformedOscillator : public Oscillator {
	public:
		PerformedOscillator(const std::string& waveform_path = "zeros", const bool unipolar = false);
		void generate_buf() override;
		void note_on(const uint8_t note);
		void note_on(const uint8_t note, const uint8_t velocity);
		void change_note(const uint8_t note);

	private:
		bool on;
	};
}