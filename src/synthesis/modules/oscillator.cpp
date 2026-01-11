#include "oscillator.h"
#include "standalone/file_io.h"
#include "utils/config.h"
#include "utils/accelerator.h"
#include "midi/notes.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cassert>

using namespace synthesis;

Oscillator::Oscillator(const string& waveform_path, const bool unipolar)
	: Module(in_bufs),
	phase { 0 }, freq{ 0.0f }, waveform{}, phase_increment{ 0 }, gain{ 1.0 }, velocity_gain{ 1.0 }
{
	load_waveform(waveform_path, unipolar);
}

double Oscillator::get_semitone_shift_multiplier(const int8_t semitones) {
	switch (semitones) {
	case  0: return 1.0;
	case  1: return 1.05946309436;
	case -1: return 0.94387431268;
	case  2: return 1.12246204831;
	case -2: return 0.89089871814;
	case  3: return 1.18920711500;
	case -3: return 0.84089641525;
	default:
		return pow(pow(2, 1.0 / 12.0), semitones);
	}
}

void Oscillator::generate_buf() {
	if (freq == 0.0f) {
		//memset(out_buf, 0.0f, config::buffer_size * sizeof(float_s));
		*out_buf = EMPTY_BUF_MARKER; // put marker at start of buffer
		return;
	}

	// better way to do this? or just make mono?
	float_s pitch_buf_sum[config::buffer_size];
	const bool pitch_mods{ sum_bufs(BufType::PITCH, pitch_buf_sum) }; // "constant" parameter is 0
	for (size_t i = 0; i < config::buffer_size; i += config::channels) {
		if (pitch_mods) {
		// pitch shift. at audio rate. uses a linear approximation between semitones https://en.wikipedia.org/wiki/Cent_(music)#Piecewise_linear_approximation
			const int8_t semitones{ static_cast<int8_t>(static_cast<int16_t>(pitch_buf_sum[i]) / 100) };
		// below line: todo---vectorize all this math
			const double effective_freq{ freq * get_semitone_shift_multiplier(semitones) * (1.0 + 0.0005946 * (pitch_buf_sum[i] - semitones * 100)) };
			phase_increment = effective_freq * (static_cast<float_s>(config::waveform_resolution) / config::sample_rate);
		}

		if (phase >= config::waveform_resolution) {
			phase -= config::waveform_resolution;
		}

		// // with interpolation
		//const float_s prev_sample{ waveform[static_cast<size_t>(phase)] };
		//const float_s next_sample{ waveform[static_cast<size_t>((phase >= config::waveform_resolution - 1) ? 0 : phase + 1)] };
		//const float_s interpolation_ratio{ phase - static_cast<uint16_t>(phase) };
		//*(out_buf + i) = prev_sample * (1.0 - interpolation_ratio) + next_sample * interpolation_ratio;
		*(out_buf + i) = waveform[static_cast<size_t>(phase)]; // round?

		for (size_t j = 1; j < config::channels; j++) {
			*(out_buf + i + j) = *(out_buf + i);
		}

		phase += phase_increment;
	}

	float_s effective_gain_buf[config::buffer_size];
	if (sum_bufs(BufType::GAIN, effective_gain_buf, gain)) {
		accelerator::vec_entrywise_mult_float_s(effective_gain_buf, out_buf, out_buf, config::buffer_size);
		accelerator::vec_scal_mult_float_s(out_buf, out_buf, velocity_gain, config::buffer_size);
	}
	else {
		accelerator::vec_scal_mult_float_s(out_buf, out_buf, gain * velocity_gain, config::buffer_size);
	}

	return;
}

void Oscillator::load_waveform(const string& path, const bool unipolar) {
#ifdef STANDALONE
	standalone::file_io::read_wav(config::waveform_path + path + ".wav", waveform);
#endif
	if (unipolar) { // already rescaled to -1 to 1
		accelerator::vec_scal_add_float_s(waveform, waveform, 1.0f, config::buffer_size);
		accelerator::vec_scal_mult_float_s(waveform, waveform, 0.5f, config::buffer_size);
	}
}

void Oscillator::note_on(const uint8_t note, const uint8_t velocity) {
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
	velocity_gain = static_cast<float_s>(velocity) / 127;
	phase = 0;
}

void Oscillator::note_off() {
	set_freq(0.0f);
}

void Oscillator::set_freq(const float_s value) {
	assert(value <= config::sample_rate / 2);
	freq = value;
	phase_increment = freq * config::waveform_resolution / config::sample_rate;
}

void Oscillator::set_gain(const float_s value) {
	gain = value;
}

void Oscillator::set_phase(const float_s value) {
	assert(value >= 0.0 && value <= 1.0);
	phase = value * config::waveform_resolution;
}