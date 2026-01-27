#pragma once

#include "synthesis/modules/module.h"
#include "utils/global.h"

#include <functional>

namespace synthesis {
	class Envelope : public Module {
	public:
		Envelope();
		void generate_buf() override;
		void note_on(const uint8_t note, const uint8_t velocity) override;
		void note_off() override;

		void set_attack(const float_s value_s);
		void set_decay(const float_s value_s);
		void set_sustain(const float_s value_s);
		void set_release(const float_s value_s);

	private:
		enum class EnvelopeState {
			attack,
			decay,
			sustain,
			release,
			off
		};

		float_s attack;
		float_s decay;
		float_s sustain;
		float_s release;
		std::array<double, 5> state_durations;
		EnvelopeState state;
		size_t t;
		float_s gain;
		float_s delta_gain;
	};
}