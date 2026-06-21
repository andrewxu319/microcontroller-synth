#ifdef TEENSY
#pragma once

#include "utils/global.h"

#include <MIDI.h>

namespace teensy {
	class MidiListener {
	public:
		MidiListener();
		void read();

	private:
		// RtMidiIn midi_in;
		// unsigned int num_ports;
		// std::vector<std::string> port_names;
		MIDI_NAMESPACE::SerialMIDI<HardwareSerial> serialMIDI;
		MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>> MIDI;
	};
}
#endif