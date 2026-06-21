#ifndef TEENSY
#include "midi_listener.h"

#include "midi/message.h"
#include "synthesis/synthesizer.h"

#include <thread>

using namespace midi;
using namespace standalone;

MidiListener::MidiListener() 
	: midi_in{},
	num_ports{},
	port_names{}
{
	num_ports = midi_in.getPortCount();
	port_names = std::vector<std::string>{ num_ports };
	if (num_ports == 0) {
		printf("No MIDI input ports available.");
	}
	for (unsigned int i{ 0 }; i < num_ports; i++) {
		port_names[i] = midi_in.getPortName(i);
	}
}

void MidiListener::send_message(double delta_time, std::vector<unsigned char>* midi_message, void* user_data) { // delta_time unused but required by rtmidi
	//utils::timer::start();
	size_t len = midi_message->size(); // bytes
	//for (unsigned char b : *midi_message) {
	//	printf("%x ", b);
	//}
	//printf("\n");

	// parse message https://midi.org/expanded-midi-1-0-messages-list
	switch ((*midi_message)[0] >> 4) { // integer divide by 16
	case 0x8:
		// note off
		Synthesizer::instance().note_messages.push(
			NoteMessage{
				NoteMessage::NoteFunction::NOTE_OFF,
				static_cast<uint8_t>((*midi_message)[0] & 0x0f), // mod 16
				(*midi_message)[1],
				(*midi_message)[2]
			}
		);
		break;
	case 0x9:
		// note on
		Synthesizer::instance().note_messages.push(
			NoteMessage{
				NoteMessage::NoteFunction::NOTE_ON,
				static_cast<uint8_t>((*midi_message)[0] & 0x0f), // mod 16
				(*midi_message)[1],
				(*midi_message)[2]
			}
		);
		break;
	case 0xb:
		// cc
		Synthesizer::instance().cc_messages.push(
			CcMessage{
				(*midi_message)[1],
				static_cast<uint8_t>((*midi_message)[0] & 0x0f), // mod 16
				(*midi_message)[2]
			}
		);
		break;
	case 0xe:
		// pitch bend
		break;
	default:
		// maybe add later
		break;
	}
}

void MidiListener::open_port(const unsigned int port) {
	midi_in.openPort(port);
	midi_in.setCallback(&MidiListener::send_message);
	printf("Using port %d: %s.\n", port, port_names[port].c_str());
}

void MidiListener::close_port() {
	midi_in.closePort();
}

//void MidiListener::loop() {
//	while (true) {
//		this_thread::sleep_for(chrono::milliseconds(10));
//	}
//}
#endif