#include "voice.h"
#include "midi/notes.h"

#include <cassert>

using namespace synthesis;

Voice::Voice(const vector<Module*> outputs_)
	: Module(outputs_),
	active{ false },
	current_note{ -1 }
{
	;
}

Voice::Voice(const vector<Oscillator*> outputs_)
	: Voice(vector<Module*>{outputs_.begin(), outputs_.end()}) { // convert it to a vector of Module*'s
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