#include "noise_generator.h"

#include "utils/config.h"
#include "utils/accelerator.h"
#include "utils/rng.h"



using namespace synthesis;

NoiseGenerator::NoiseGenerator(const bool unipolar)
	: Module(in_bufs),
	gain{ 1.0 }
{
}

void NoiseGenerator::generate_buf() {
	// better way to do this? or just make mono?
	for (size_t i = 0; i < config::buffer_size; i += config::channels) {
		*(out_buf + i) = utils::rng_normal(0.0f, 0.3333f);
		for (size_t j = 1; j <= config::channels; j++) {
			*(out_buf + i + j) = *(out_buf + i);
		}
	}

	float_s effective_gain_buf[config::buffer_size];
	if (sum_bufs(BufType::GAIN, effective_gain_buf, gain)) {
		accelerator::vec_entrywise_mult_float_s(effective_gain_buf, out_buf, out_buf, config::buffer_size);
	}
	else {
		accelerator::vec_scal_mult_float_s(out_buf, out_buf, gain, config::buffer_size);
	}

	return;
}

void NoiseGenerator::set_gain(const float_s value) {
	gain = value;
}