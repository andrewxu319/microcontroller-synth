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

void Voice::note_on(const char note) {
	active = true;
	current_note = note;
	for (const Module* output : outputs) {
		(*(Oscillator*)output).set_freq(midi::notes[note]); // cant static_cast because of const
	}
}

void Voice::note_off() {
	active = false;
	current_note = -1;
	for (const Module* output : outputs) {
		(*(Oscillator*)output).set_freq(0.0f);
	}
}