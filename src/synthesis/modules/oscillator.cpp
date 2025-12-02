#include "oscillator.h"
#include "utils/config.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cassert>

Oscillator::Oscillator() 
	: phase{ 0 }, freq{ 0 }, period{ 0 }, wavetable {
} {
}

void Oscillator::generate_buf() {
	if (freq == 0) {
		std::fill(out_buf, out_buf + config::buffer_size, 0.0);
		return;
	}

	for (size_t i = 0; i < config::buffer_size; i += 2) {
		if (phase >= period * 2) {
			phase = 0;
		}

		// if lfo, ignore every other sample
		//*(out_buf + i) = static_cast<float32_t>(sin(2.0 * M_PI * (static_cast<double>(freq) / config::sample_rate) * phase));
		*(out_buf + i) = wavetable[phase * freq * config::wavetable_resolution / 2 / config::sample_rate];
		*(out_buf + i + 1) = *(out_buf + i);

		phase++;
		//printf("%f\n", *(out_buf + i));
	}

	return;
}

void Oscillator::set_freq(const int value) {
	assert(value >= config::sample_rate / config::wavetable_resolution); // don't want to deal with interpolation rn but we might need to down the line
	assert(value >= 20 && value <= 20000);
	freq = value;
	period = config::sample_rate / freq;
}