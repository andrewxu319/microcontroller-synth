#include "voice.h"
#include "midi/notes.h"

#include <cassert>

using namespace synthesis;

Voice::Voice()
	: active{ false },
	current_note{ -1 }
{
	;
}

void Voice::note_on(const uint8_t note, const uint8_t velocity) {
	active = true;
	current_note = note;
	for (const Module* output : outputs) {
		((Oscillator*)output)->set_freq(midi::notes[note]); // cant static_cast because of const
		((Oscillator*)output)->set_gain(static_cast<float_s>(velocity) / 127);
	}
}

void Voice::note_off() {
	active = false;
	current_note = -1;
	for (const Module* output : outputs) {
		((Oscillator*)output)->set_freq(0.0f);
	}
}