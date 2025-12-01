#include "oscillator.h"
#include "utils/config.h"

#define _USE_MATH_DEFINES
#include <math.h>

Oscillator::Oscillator() {
	phase = 0;
	freq = 0;
}

void Oscillator::generate_buf() {
	if (freq == 0) {
		out_buf->fill(0.0);
		return;
	}

	for (size_t i = 0; i < Config::buffer_size; i++) {
		if (phase >= Config::sample_rate / freq) {
			phase = 0;
		}

		(*out_buf)[i] = static_cast<float>(sin(2.0 * M_PI * (static_cast<double>(freq) / Config::sample_rate) * phase));
		phase++;
	}
}