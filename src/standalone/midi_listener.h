#pragma once

#include "utils/global.h"

#include <rtmidi/RtMidi.h>

namespace standalone::midi_listener {
	extern RtMidiIn midi_in;
	extern unsigned int num_ports;
	extern vector<string> port_names;

	void init();
	void open_port(const unsigned int port);
	void close_port();
	//void loop();
}