#ifndef TEENSY
#pragma once

#include "utils/global.h"

#include <rtmidi/RtMidi.h>

namespace standalone {
	class MidiListener {
	public:
		MidiListener();
		void open_port(const unsigned int port);
		void close_port();
		//void loop();
		
	private:
		RtMidiIn midi_in;
		unsigned int num_ports;
		std::vector<std::string> port_names;

		static void send_message(double delta_time, std::vector<unsigned char>* midi_message, void* user_data);
	};
}
#endif