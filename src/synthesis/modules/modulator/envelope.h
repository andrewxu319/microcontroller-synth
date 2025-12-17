#pragma once

#include "modulator.h"
#include "utils/includes.h"

#include <functional>

namespace synthesis {
	class Envelope : public Modulator {
	public:
		Envelope();
		void generate_buf() override;
		void note_on(const uint8_t note, const uint8_t velocity) override;
		void note_off() override;

		void set_attack(const double value);
		void set_decay(const double value);
		void set_sustain(const double value);
		void set_release(const double value);

	private:
		enum class EnvelopeState {
			attack,
			decay,
			sustain,
			release,
			off
		};

		double attack;
		double decay;
		double sustain;
		double release;
		array<double, 5> state_durations;
		EnvelopeState state;
		size_t t;
		float_s gain;
		float_s delta_gain;
	};
}