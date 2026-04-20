#ifndef TEENSY
#include "application.h"
#include "standalone/sound_engine.h"
#include "standalone/midi_listener.h"

#include <portaudio/portaudio.h>

int main() {
	standalone::sound_engine::init();
	standalone::midi_listener::init();
	standalone::midi_listener::open_port(config::midi_port);

	application();

	standalone::sound_engine::start_stream();

	while (true) {
		Pa_Sleep(10000);
	}

	standalone::midi_listener::close_port();
	standalone::sound_engine::close();
}
#endif