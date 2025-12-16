#include "envelope.h"

#include "utils/config.h"

#include <cassert>

using namespace synthesis;

Envelope::Envelope() 
	: state{ EnvelopeState::off },
	state_durations{},
	gain{ 0.0f },
	delta_gain{ 0.0f }
{
	set_attack(0);
	set_decay(0);
	set_sustain(0);
	set_release(0);
}

void Envelope::generate_buf() {
	if (state == EnvelopeState::off) {
		memset(out_buf, 0.0f, config::buffer_size * sizeof(float_s));
	}

	for (int i{ 0 }; i < config::buffer_size; i++) {
		if (t >= state_durations[static_cast<size_t>(state)] && state != EnvelopeState::off && state != EnvelopeState::sustain) {
			t = 0;
			state = static_cast<EnvelopeState>(static_cast<size_t>(state) + 1);
			switch (state) {
			case EnvelopeState::decay:
				delta_gain = (sustain - 1.0) / state_durations[static_cast<size_t>(EnvelopeState::decay)];
				gain = 1.0;
				break;
			case EnvelopeState::sustain:
				delta_gain = 0;
				gain = sustain;
				printf("sustain\n");
				break;
			case EnvelopeState::off:
				delta_gain = 0;
				gain = 0;
				memset(&out_buf[i], 0.0f, config::buffer_size * sizeof(float_s) - i);
				break;
			default:
				break;
			}
		}

		gain += delta_gain;
		out_buf[i] = gain;
		t++;
	}
}

void Envelope::note_on(const uint8_t note, const uint8_t velocity) {
	state = EnvelopeState::attack;
	delta_gain = 1.0 / state_durations[static_cast<size_t>(EnvelopeState::attack)];
	t = 0;
}

void Envelope::note_off(const uint8_t note) {
	state = EnvelopeState::release;
	delta_gain = -sustain / state_durations[static_cast<size_t>(EnvelopeState::release)];
	t = 0;
}

void Envelope::set_attack(const double value) {
	attack = value;
	state_durations[static_cast<size_t>(EnvelopeState::attack)] = value * config::sample_rate;
}

void Envelope::set_decay(const double value) {
	decay = value;
	state_durations[static_cast<size_t>(EnvelopeState::decay)] = value * config::sample_rate;
}

void Envelope::set_sustain(const double value) {
	assert(0.0 <= value && value <= 1.0);
	sustain = value;
}

void Envelope::set_release(const double value) {
	release = value;
	state_durations[static_cast<size_t>(EnvelopeState::release)] = value * config::sample_rate;
}