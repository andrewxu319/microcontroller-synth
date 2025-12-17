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
	Module::note_on(note, velocity);
}

void Voice::note_off() {
	active = false;
	current_note = -1;
	Module::note_off();
}