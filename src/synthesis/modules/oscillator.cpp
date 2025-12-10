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
	: phase{ 0 }, freq{ 0 }, period{ 0 }, wavetable{} {
	load_wavetable(wavetable_path);
}

void Oscillator::generate_buf() {
	if (freq == 0.0f) {
		*out_buf = EMPTY_BUF_MARKER; // put marker at start of buffer
		return;
	}

	// better way to do this? or just make mono?
	for (size_t i = 0; i < config::buffer_size; i += 2) {
		if (phase >= period * 2) {
			phase = 0;
		}

		// if lfo, ignore every other sample
		//*(out_buf + i) = static_cast<float_s>(sin(2.0 * M_PI * (static_cast<double>(freq) / config::sample_rate) * phase));
		*(out_buf + i) = wavetable[static_cast<size_t>(phase * freq * config::wavetable_resolution / 2 / config::sample_rate)]; // round?
		*(out_buf + i + 1) = *(out_buf + i);

		phase++;
		//printf("%f\n", *(out_buf + i));
	}

	accelerator::vec_mult_float_s(out_buf, out_buf, config::buffer_size);

	return;
}

void Oscillator::load_wavetable(const string& path) {
#ifdef STANDALONE
	standalone::file_io::read_wav(config::wavetable_path + path + ".wav", wavetable);
#endif
}

void Oscillator::set_freq(const float_s value) {
	if (value != 0.0f) {
		assert(value >= config::sample_rate / config::wavetable_resolution); // don't want to deal with interpolation rn but we might need to down the line
		assert(value >= 20 && value <= 20000);
	}
	freq = value;
	period = config::sample_rate / freq;
}

void Oscillator::set_gain(const float_s value) {
	// input checking? i don't think there's a need yet to cap at -1.0 / 1.0
	gain = value;
	accelerator::set_scalar(gain);
}