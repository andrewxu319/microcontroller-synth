#include "noise_generator.h"

#include "utils/config.h"
#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

NoiseGenerator::NoiseGenerator(const bool unipolar)
	: Module(mods, sizeof(mods) / sizeof(Module*)),
	gain{ 1.0 }
{
}

void NoiseGenerator::generate_buf() {
	// better way to do this? or just make mono?
	for (size_t i = 0; i < config::buffer_size; i += config::channels) {
		*(out_buf + i) = rng_dist(rng_engine);
		for (size_t j = 1; j <= config::channels; j++) {
			*(out_buf + i + j) = *(out_buf + i);
		}
	}

	if (!mods[Mods::GAIN].empty()) {
		float_s* effective_gain_buf{ in_bufs[mods[Mods::GAIN][0]->id].data };
		for (int i{ 1 }; i < mods[Mods::GAIN].size(); i++) {
			accelerator::vec_add_float_s(in_bufs[mods[Mods::GAIN][i]->id].data, effective_gain_buf, effective_gain_buf, config::buffer_size);
		}
		accelerator::vec_scal_add_float_s(effective_gain_buf, effective_gain_buf, gain, config::buffer_size);
		accelerator::vec_entrywise_mult_float_s(effective_gain_buf, out_buf, out_buf, config::buffer_size);
	}
	else {
		accelerator::vec_scal_mult_float_s(out_buf, out_buf, gain, config::buffer_size);
	}

	return;
}