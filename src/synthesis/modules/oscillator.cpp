#include "oscillator.h"
#include "standalone/file_io.h"
#include "utils/config.h"
#include "utils/accelerator.h"
#include "midi/notes.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cassert>

using namespace synthesis;

Oscillator::Oscillator(const string& wavetable_path = "zeros")
	: Module(mods, sizeof(mods) / sizeof(Module*)),
	phase{ 0 }, freq{ 0 }, wavetable{}, phase_increment{ 0 } {
	load_wavetable(wavetable_path);
}

void Oscillator::generate_buf() {
	if (freq == 0.0f) {
		//memset(out_buf, 0.0f, config::buffer_size * sizeof(float_s));
		*out_buf = EMPTY_BUF_MARKER; // put marker at start of buffer
		return;
	}

	// better way to do this? or just make mono?
	for (size_t i = 0; i < config::buffer_size; i += config::channels) {
		if (phase >= config::wavetable_resolution) {
			phase = 0;
		}

		// if lfo, ignore every other sample
		//*(out_buf + i) = static_cast<float_s>(sin(2.0 * M_PI * (static_cast<double>(freq) / config::sample_rate) * phase));
		*(out_buf + i) = wavetable[static_cast<size_t>(phase)]; // round?
		for (size_t j = 1; j <= config::channels; j++) {
			*(out_buf + i + j) = *(out_buf + i);
		}

		phase += phase_increment;
	}

	accelerator::vec_scal_mult_float_s(out_buf, out_buf, gain, config::buffer_size);

	if (mods[Mods::GAIN]) {
		accelerator::vec_entrywise_mult_float_s(in_bufs[mods[Mods::GAIN]->id].data, out_buf, out_buf, config::buffer_size);
	}

	return;
}

void Oscillator::load_wavetable(const string& path) {
#ifdef STANDALONE
	standalone::file_io::read_wav(config::wavetable_path + path + ".wav", wavetable);
#endif
}

void Oscillator::note_on(const uint8_t note, const uint8_t velocity) {
	set_freq(midi::notes[note]); // cant static_cast because of const
	set_gain(static_cast<float_s>(velocity) / 127);
}

void Oscillator::note_off() {
	set_freq(0.0f);
}

void Oscillator::set_freq(const float_s value) {
	freq = value;
	phase_increment = freq * config::wavetable_resolution / config::sample_rate;
}

void Oscillator::set_gain(const float_s value) {
	gain = value;
}