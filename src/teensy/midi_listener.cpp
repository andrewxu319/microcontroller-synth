#ifdef TEENSY
#include "midi_listener.h"

#include "midi/message.h"
#include "synthesis/synthesizer.h"
#include "utils/config.h"

using namespace midi;

namespace teensy::midi_listener{
	MIDI_NAMESPACE::SerialMIDI<HardwareSerial> serialMIDI{ config::teensy_serial };
	MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>> MIDI{ static_cast<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>&>(serialMIDI) };

	void init() {
		config::teensy_serial.begin(115200);
		MIDI.begin(MIDI_CHANNEL_OMNI);
	}

	void read() {
		while (true){
		if (MIDI.read()) {
			byte type{ MIDI.getType() };
			byte data_1{ MIDI.getData1() };
			byte data_2{ MIDI.getData2() };
			byte channel{ MIDI.getChannel() };
			Serial.print("read: ");
Serial.print(type);
Serial.print(", ");
Serial.print(channel);
Serial.print(", ");
Serial.print(data_1);
Serial.print(", ");
Serial.println(data_2);

			// parse message https://midi.org/expanded-midi-1-0-messages-list
			switch (type) { // integer divide by 16
			case midi::NoteOff:
				printf("Note off: %d\n", data_1);
				synthesis::note_messages.push(
					NoteMessage{ NoteMessage::NoteFunction::NOTE_OFF, channel, data_1, data_2 }
				);
				break;
			case midi::NoteOn:
				printf("Note on: %d\n", data_1);
				synthesis::note_messages.push(
					NoteMessage{ NoteMessage::NoteFunction::NOTE_ON, channel, data_1, data_2 }
				);
				break;
			case midi::ControlChange:
				synthesis::cc_messages.push(
					CcMessage{ data_1, channel, data_2 }
				);
				break;
			case midi::PitchBend:
				break;
			default:
				// maybe add later
				break;
			}
		}
	}
}}
#endif