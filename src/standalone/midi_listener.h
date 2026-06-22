#ifndef TEENSY
#pragma once

#include "midi/message.h"
#include "utils/global.h"

#include <rtmidi/RtMidi.h>
#include <queue>

namespace standalone {
	class MidiListener {
	public:
		MidiListener(std::queue<midi::NoteMessage>& note_messages, std::queue<midi::CcMessage>& cc_messages);
		void open_port(const unsigned int port);
		void close_port();
		//void loop();
		
	private:
		RtMidiIn midi_in;
		unsigned int num_ports;
		std::vector<std::string> port_names;
		std::queue<midi::NoteMessage>& note_messages_;
		std::queue<midi::CcMessage>& cc_messages_;

		static void send_message(double delta_time, std::vector<unsigned char>* midi_message, void* this_ptr);
	};
}
#endif