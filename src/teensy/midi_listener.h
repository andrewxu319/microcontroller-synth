#ifdef TEENSY
#pragma once

#include "midi/message.h"
#include "utils/global.h"

#include <MIDI.h>
#include <queue>

namespace teensy {
	class MidiListener {
	public:
		MidiListener(std::queue<midi::NoteMessage>& note_messages, std::queue<midi::CcMessage>& cc_messages);
		void read();

	private:
		// RtMidiIn midi_in;
		// unsigned int num_ports;
		// std::vector<std::string> port_names;
		MIDI_NAMESPACE::SerialMIDI<HardwareSerial> serialMIDI;
		MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>> MIDI;
		std::queue<midi::NoteMessage>& note_messages_;
		std::queue<midi::CcMessage>& cc_messages_;
	};
}
#endif