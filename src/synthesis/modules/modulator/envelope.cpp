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
	int t{ 0 };
	while (t < config::buffer_size) {
		switch (state) {
		case EnvelopeState::attack:
			for (; t < config::buffer_size; t++) {
				if (gain >= 1.0) {
					state = EnvelopeState::decay;
					//printf("decay");
					delta_gain = (sustain - 1.0) / state_durations[static_cast<size_t>(EnvelopeState::decay)];
					gain = 1.0;
					break;
				}
				gain += delta_gain;
				out_buf[t] = gain;
			}
			break;
		case EnvelopeState::decay:
			for (; t < config::buffer_size; t++) {
				if (gain <= sustain) {
					state = EnvelopeState::sustain;
					//printf("sustain\n");
					gain = sustain;
					break;
				}
				gain += delta_gain;
				out_buf[t] = gain;
			}
			break;
		case EnvelopeState::sustain:
			for (; t < config::buffer_size; t++) {
				out_buf[t] = gain;
			}
			break;
		case EnvelopeState::release:
			for (; t < config::buffer_size; t++) {
				if (gain <= 0.0) {
					state = EnvelopeState::off;
					//printf("off\n");
					gain = 0.0;
					Module::note_off();
					break;
				}
				gain += delta_gain;
				out_buf[t] = gain;
			}
			break;
		case EnvelopeState::off:
			memset(&out_buf[t], 0.0f, (config::buffer_size - t) * sizeof(float_s));
			return;
		default:
			break;
		}
	}
}

void Envelope::note_on(const uint8_t note, const uint8_t velocity) {
	state = EnvelopeState::attack;
	delta_gain = 1.0 / state_durations[static_cast<size_t>(EnvelopeState::attack)];
	t = 0;
	//printf("attack\n");
	Module::note_on(note, velocity);
}

void Envelope::note_off() {
	state = EnvelopeState::release;
	delta_gain = -sustain / state_durations[static_cast<size_t>(EnvelopeState::release)];
	//printf("release\n");
}

void Envelope::set_attack(const double value) {
	attack = value;
	state_durations[static_cast<size_t>(EnvelopeState::attack)] = value * config::sample_rate;
	printf("Attack set: %f\n", value);
}

void Envelope::set_decay(const double value) {
	decay = value;
	state_durations[static_cast<size_t>(EnvelopeState::decay)] = value * config::sample_rate;
	printf("Decay set: %f\n", value);
}

void Envelope::set_sustain(const double value) {
	assert(0.0 <= value && value <= 1.0);
	sustain = value;
	printf("Sustain set: %f\n", value);
}

void Envelope::set_release(const double value) {
	release = value;
	state_durations[static_cast<size_t>(EnvelopeState::release)] = value * config::sample_rate;
	printf("Release set: %f\n", value);
}