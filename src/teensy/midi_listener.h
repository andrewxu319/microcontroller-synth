#ifdef TEENSY
#pragma once

#include "utils/global.h"

#include <MIDI.h>

namespace teensy::midi_listener {
	// extern RtMidiIn midi_in;
	// extern unsigned int num_ports;
	// extern std::vector<std::string> port_names;
	extern MIDI_NAMESPACE::SerialMIDI<HardwareSerial> serialMIDI;
	extern MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>> MIDI;

	void init();
	void read();
}
#endif