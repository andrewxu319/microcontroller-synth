#include "portamento.h"

#include "utils/config.h"
#include "utils/math.h"

using namespace synthesis;

// we don't need the exact frequency of the oscillator (accounting for pitch shifts etc) because ASSUMING OTHER PITCH SHIFTS ARE
// CONTINUOUS IF WE WANT THE RESULT TO BE CONTINUOUS, the relative number of cents required for portamento to shift is the same

Portamento::Portamento()
	: time{}, increment{}, prev_note{}, starting_pitch_shift{}
{
	;
}

void Portamento::generate_buf() {
	if (starting_pitch_shift == 0.0f) {
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	float_s pitch_shift{ std::abs(starting_pitch_shift) };
	for (size_t i{}; i < config::buffer_size; i++) {
		pitch_shift -= increment;
		if (pitch_shift >= 0.0f) {
			out_buf[i] = pitch_shift;
		}
		else {
			out_buf[i] = 0.0f;
			pitch_shift = 0.0f;
			starting_pitch_shift = 0.0f;
		}
	}
	if (starting_pitch_shift > 0.0f) {
		if (pitch_shift > 0.0f) {
			starting_pitch_shift = pitch_shift;
		}
	}
	else {
		if (pitch_shift > 0.0f) {
			starting_pitch_shift = -pitch_shift;
		}
		math::vec_scal_mult_float_s(out_buf, out_buf, -1.0f, config::buffer_size);
	}
	
	Module::generate_buf();
}

void Portamento::note_on(const uint8_t note, const uint8_t) {
	prev_note = note;
}

void Portamento::change_note(const uint8_t note) {
	starting_pitch_shift += (prev_note - note) * 100;
	increment = std::abs(starting_pitch_shift / time); // 100 cents per semitone
	prev_note = note;
}

void Portamento::set_time(double time_ms) {
	time = static_cast<size_t>(time_ms * (0.001 *config::sample_rate));
}