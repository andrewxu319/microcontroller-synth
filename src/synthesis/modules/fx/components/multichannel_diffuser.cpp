// todo: make this use an actual multichannel delay line

#include "multichannel_diffuser.h"

#include <cmath>

using namespace synthesis;

MultichannelDiffuser::MultichannelDiffuser(size_t capacity_, uint8_t num_channels_)
	: MultichannelModule(in_bufs, num_channels_),
	in_bufs{}, capacity{ capacity_ }, num_channels{ num_channels_ }, sqrt_num_channels{ static_cast<float_s>(sqrt(num_channels_)) }, max_delay{},
	delay_line{ capacity_, num_channels_ }, flip_polarities(static_cast<int8_t>(num_channels_), false), hadamard_a{}
{
	assert((num_channels & (num_channels - 1)) == 0); // check num_channels is power of 2
}

void MultichannelDiffuser::init() {
	shuffle(in_bufs[BufType::AUDIO].begin(), in_bufs[BufType::AUDIO].end(), utils::rng_engine);
	for (uint8_t i{ 0 }; i < num_channels; i++) {
		delay_line.add_buf(in_bufs[BufType::AUDIO][i], WetOnlyDelayLine::BufType::AUDIO);

		if (utils::rng_uniform(0.0, 1.0) >= 0.5) { // 50% chance
			flip_polarities[i] = true;
		}
	}
	delay_line.init();
}

void MultichannelDiffuser::generate_buf() {
	delay_line.generate_buf();
	for (uint8_t i{ 0 }; i < num_channels; i++) {
		if (flip_polarities[i]) {
			math::vec_scal_mult_float_s(delay_line.get_out_bufs()[i].data(), out_bufs[i].data(), -1.0f, config::buffer_size);
		}
		else {
			memcpy(out_bufs[i].data(), delay_line.get_out_bufs()[i].data(), config::buffer_size * sizeof(float_s));
		}
	}
	fast_hadamard_transform(out_bufs);
}

void MultichannelDiffuser::resize(size_t capacity) {
	delay_line.resize(capacity);
}

void MultichannelDiffuser::set_num_channels(uint8_t value) {
	MultichannelModule::set_num_channels(value);
	delay_line.set_num_channels(value);
	flip_polarities.resize(value);
	num_channels = value;
	sqrt_num_channels = sqrt(value);
}

void MultichannelDiffuser::set_delay(double value_ms) {
	max_delay = value_ms;
	delay_line.set_delay(value_ms, 0); // have first one take the max delay time

	double window_size{ max_delay / num_channels };
	double next_window_lower_bound{ window_size / 2 };
	for (uint8_t i{ 1 }; i < num_channels; i++) {
		// split max delay window into num_channels windows. pick random number in each channel's window
		delay_line.set_delay(utils::rng_uniform(next_window_lower_bound, next_window_lower_bound += window_size), i);
	}
}

void MultichannelDiffuser::fast_hadamard_transform(std::vector<MultichannelModule::Buffer>& data) {
	for (uint8_t h{ 1 }; h < num_channels; h *= 2) {
		for (uint8_t i{ 0 }; i < num_channels; i += 2 * h) {
			for (uint8_t j{ i }; j < i + h; j++) {
				memcpy(hadamard_a, data[j].data(), config::buffer_size * sizeof(float_s));
				math::vec_add_float_s(data[j].data(), data[j + h].data(), data[j].data(), config::buffer_size);
				math::axpy(hadamard_a, data[j + h].data(), data[j + h].data(), -1.0f, config::buffer_size);
			}
		}
	}

	for (MultichannelModule::Buffer& buffer : data) {
		math::vec_scal_mult_float_s(buffer.data(), buffer.data(), 1.0f / sqrt_num_channels, config::buffer_size);
	}
}