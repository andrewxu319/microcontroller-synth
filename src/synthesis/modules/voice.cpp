#include "voice.h"
#include "midi/notes.h"



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

	for (Module* output : outputs) {
		output->note_on(note, velocity);
	}
}

void Voice::note_off() {
	active = false;
	current_note = -1;

	for (Module* output : outputs) {
		output->note_off();
	}
}

void Voice::change_note(const uint8_t note) {
	current_note = note;

	for (Module* output : outputs) {
		output->change_note(note);
	}
}