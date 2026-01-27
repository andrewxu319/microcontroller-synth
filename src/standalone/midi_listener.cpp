#include "midi_listener.h"

#include "midi/message.h"
#include "synthesis/synthesizer.h"

#include <thread>

using namespace midi;

namespace standalone::midi_listener{
	RtMidiIn midi_in{};
	unsigned int num_ports{};
	std::vector<std::string> port_names{};

	void init() {
		num_ports = midi_in.getPortCount();
		port_names = std::vector<std::string>{ num_ports };
		if (num_ports == 0) {
			printf("No MIDI input ports available.");
		}
		for (unsigned int i{ 0 }; i < num_ports; i++) {
			port_names[i] = midi_in.getPortName(i);
		}
	}

	void send_message(double delta_time, std::vector<unsigned char>* midi_message, void* user_data) { // delta_time unused but required by rtmidi
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
			synthesis::note_messages.push(
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
			synthesis::note_messages.push(
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
			synthesis::cc_messages.push(
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

	void open_port(const unsigned int port) {
		midi_in.openPort(port);
		midi_in.setCallback(send_message);
		printf("Using port %d: %s.\n", port, port_names[port].c_str());
	}

	void close_port() {
		midi_in.closePort();
	}

	//void loop() {
	//	while (true) {
	//		this_thread::sleep_for(chrono::milliseconds(10));
	//	}
	//}
}