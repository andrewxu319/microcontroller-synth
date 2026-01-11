#include "phaser.h"

#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

Phaser::Phaser()
	: Fx(in_bufs),
	all_pass_filters{ Dsp::FilterDesign<Dsp::RBJ::Design::AllPass, 1>{} },
	params{},
	stages{},
	center{},
	in_bufs{},
	feedback{},
	feedback_memory{}
{
	params[0] = config::sample_rate;
	params[2] = 1.0;
	all_pass_filters.back().setParams(params);
}

void Phaser::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	memcpy(out_buf, audio_in_buf, config::buffer_size * sizeof(float_s));
	float_s center_buf_sum[config::buffer_size];
	const bool center_mods{ sum_bufs(BufType::CENTER_FREQ, center_buf_sum, center) };
	float_s feedback_buf_sum[config::buffer_size];
	const bool feedback_mods{ sum_bufs(BufType::FEEDBACK, feedback_buf_sum, feedback) };

	for (int i{ 0 }; i < config::channel_buffer_size; i += config::control_rate) {
		if (center_mods) {
			params[1] = center_buf_sum[i];
			for (int j{ 0 }; j < stages; j++) {
				all_pass_filters[j].setParams(params);
			}
		}

		for (int j{ i }; j < i + config::control_rate; j++) {
			if (feedback_buf_sum) {
				feedback = feedback_buf_sum[j];
			}
			out_buf[j] += feedback_memory * feedback;

			float* data[1]{ out_buf + j }; // if mono. data[1] means array of 1 array pointer
			for (int k{ 0 }; k < stages; k++) {
				all_pass_filters[k].process(1, data);
			}

			feedback_memory = out_buf[j];
		}
	}

	accelerator::vec_add_float_s(audio_in_buf, out_buf, out_buf, config::buffer_size);

	mix_dry_wet();
}

void Phaser::set_center_freq(const uint16_t value) {
	assert(value > 0);
	center = value;
	params[1] = value;
	for (auto& filter : all_pass_filters) {
		filter.setParams(params);
	}
}

void Phaser::set_stages(const uint8_t value) {
	assert(value > 0);
	if (value > all_pass_filters.size()) {
		for (uint8_t i{ stages }; i < value; i++) {
			all_pass_filters.emplace_back(Dsp::FilterDesign<Dsp::RBJ::Design::AllPass, 1>{all_pass_filters.back()});
		}
	}
	else {
		all_pass_filters.resize(value);
	}
	stages = value;
}

void Phaser::set_feedback(const float_s value) {
	assert(value > -1.0 && value < 1.0);
	feedback = value;
}