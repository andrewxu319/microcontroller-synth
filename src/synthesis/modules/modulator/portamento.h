#pragma once

#include "synthesis/modules/module.h"
#include "utils/global.h"

namespace synthesis {
	class Portamento : public Module {
	public:
		Portamento();
		void generate_buf() override;
		void note_on(const uint8_t note, const uint8_t) override;
		void change_note(const uint8_t note) override;
		void set_time(double time_ms);

	private:
		size_t time; // samples
		float_s starting_pitch_shift;
		float_s increment;
		uint8_t prev_note;
	};
}