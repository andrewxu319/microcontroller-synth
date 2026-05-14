#include "performed_oscillator.h"

#include "midi/notes.h"

using namespace synthesis;

PerformedOscillator::PerformedOscillator(const std::string& waveform_path, bool unipolar)
	: Oscillator(waveform_path, unipolar), on{ false } {
}

void PerformedOscillator::generate_buf() {
	Oscillator::generate_buf();
}

void PerformedOscillator::note_on(const uint8_t note) {
	const int16_t transposed_note{ note + transpose };

	// // we could do all this, but tbh those frequencies are too extreme to be useful
	//if (transposed_note < 0) {
	//	// think of it as a simple interval up, then octave(s) down
	//	const uint8_t octaves_to_go_down{ static_cast<uint8_t>((-1 - transposed_note) / 12 + 1) };
	//	const uint8_t semitones_to_go_up{ static_cast<uint8_t>(transpose % 12 + 12) };
	//	set_freq(midi::notes[note + (semitones_to_go_up == 12 ? 0 : semitones_to_go_up)] / pow(2, octaves_to_go_down));
	//	// unaccounted hypothetical: note is at the upper end of the midi mapping & transpose is very low => note + semitones_to_go_up is invalid
	//}
	//else if (transposed_note > 127) {
	//	// this might be wrong, fix
	//	const uint8_t octaves_to_go_up{ static_cast<uint8_t>((transposed_note - 127 - 1) / 12 + 1) };
	//	const uint8_t semitones_to_go_down{ static_cast<uint8_t>(12 - transpose % 12) };
	//	set_freq(midi::notes[note - (semitones_to_go_down == 12 ? 0 : semitones_to_go_down)] * pow(2, octaves_to_go_up));
	//}
	if (transposed_note >= 0 && transposed_note <= 127) {
		set_freq(midi::notes[transposed_note]); // cant static_cast because of const
	}
	//else {
	//	set_freq(midi::notes)
	//}

	//phase = 0;
	on = true;
}


void PerformedOscillator::note_on(const uint8_t note, const uint8_t velocity) {
	note_on(note);
	velocity_gain = static_cast<float_s>(velocity) / 127;
	phase = 0;
}

void PerformedOscillator::change_note(const uint8_t note) {
	note_on(note);
}
