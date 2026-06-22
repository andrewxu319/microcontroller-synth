#ifndef TEENSY
#include "application.h"
#include "standalone/sound_engine.h"
#include "standalone/midi_listener.h"
#include "synthesis/modules/master.h"
#include "synthesis/synthesizer.h"

#include <portaudio/portaudio.h>

using namespace standalone;

int main() {
	Synthesizer synthesizer{};
	SoundEngine sound_engine{ synthesizer };
	MidiListener midi_listener{ synthesizer.note_messages, synthesizer.cc_messages };
	midi_listener.open_port(config::midi_port);
	application(synthesizer);

	sound_engine.start_stream();

	while (true) {
		Pa_Sleep(10000);
	}

	midi_listener.close_port();
	sound_engine.close();
}
#endif