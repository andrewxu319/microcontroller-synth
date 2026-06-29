#ifndef TEENSY
#include "application.h"
#include "standalone/sound_engine.h"
#include "standalone/midi_listener.h"
#include "standalone/threading/scheduler.h"
#include "synthesis/modules/master.h"
#include "synthesis/synthesizer.h"

#include <portaudio/portaudio.h>

using namespace standalone;

int main() {
	Synthesizer synthesizer{};
	Scheduler scheduler{ synthesizer };
	SoundEngine sound_engine{ scheduler.out_buf, scheduler.sound_engine_buffer_counter, scheduler.buffer_ready };
	MidiListener midi_listener{ synthesizer.note_messages, synthesizer.cc_messages };
	midi_listener.open_port(config::midi_port);
	application(synthesizer);

	if constexpr (config::multithread) {
		scheduler.launch_threads();
	}

	sound_engine.start_stream();

	scheduler.scheduler_loop();

	// todo: stop token for ending program
	midi_listener.close_port();
	sound_engine.close();
}
#endif