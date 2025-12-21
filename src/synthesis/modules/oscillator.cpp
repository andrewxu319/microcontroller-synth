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
	: Module(mods, sizeof(mods) / sizeof(Module*)),
	phase { 0 }, freq{ 0 }, waveform{}, phase_increment{ 0 }, gain{ 1.0 }, velocity_gain{ 1.0 }
{
	load_waveform(waveform_path, unipolar);
}

void Oscillator::generate_buf() {
	if (freq == 0.0f) {
		//memset(out_buf, 0.0f, config::buffer_size * sizeof(float_s));
		*out_buf = EMPTY_BUF_MARKER; // put marker at start of buffer
		return;
	}

	// better way to do this? or just make mono?
	for (size_t i = 0; i < config::buffer_size; i += config::channels) {
		float_s mod_sum_cents{ 0 };
		for (const Module* module : mods[Mods::PITCH]) {
			mod_sum_cents += in_bufs[module->id].data[i];
		}
		// pitch shift. at audio rate. uses a linear approximation between semitones https://en.wikipedia.org/wiki/Cent_(music)#Piecewise_linear_approximation
		const int8_t semitones{ static_cast<int8_t>(static_cast<int16_t>(mod_sum_cents) / 100) };
		const double effective_freq{ (freq * (pow(pow(2, 1.0 / 12.0), semitones)) * (1.0 + 0.0005946 * (mod_sum_cents - semitones * 100))) };
		phase_increment = effective_freq * config::waveform_resolution / config::sample_rate;

		if (phase >= config::waveform_resolution) {
			phase = 0;
		}

		// if lfo, ignore every other sample
		//*(out_buf + i) = static_cast<float_s>(sin(2.0 * M_PI * (static_cast<double>(freq) / config::sample_rate) * phase));
		*(out_buf + i) = waveform[static_cast<size_t>(phase)]; // round?
		for (size_t j = 1; j <= config::channels; j++) {
			*(out_buf + i + j) = *(out_buf + i);
		}

		phase += phase_increment;
	}

	if (!mods[Mods::GAIN].empty()) {
		float_s* effective_gain_buf{ in_bufs[mods[Mods::GAIN][0]->id].data };
		for (int i{ 1 }; i < mods[Mods::GAIN].size(); i++) {
			accelerator::vec_add_float_s(in_bufs[mods[Mods::GAIN][i]->id].data, effective_gain_buf, effective_gain_buf, config::buffer_size);
		}
		accelerator::vec_scal_add_float_s(effective_gain_buf, effective_gain_buf, gain, config::buffer_size);
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
	set_freq(midi::notes[note]); // cant static_cast because of const
	velocity_gain = static_cast<float_s>(velocity) / 127;
	phase = 0;
}

void Oscillator::note_off() {
	set_freq(0.0f);
}

void Oscillator::set_freq(const float_s value) {
	freq = value;
	phase_increment = freq * config::waveform_resolution / config::sample_rate;
}

void Oscillator::set_gain(const float_s value) {
	gain = value;
}