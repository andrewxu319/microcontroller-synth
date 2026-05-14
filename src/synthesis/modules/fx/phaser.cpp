#include "phaser.h"

#include "utils/math.h"

using namespace synthesis;

Phaser::Phaser()
	: Fx(in_bufs),
	allpass_filters(1),
	stages{ 1 },
	center{},
	in_bufs{},
	feedback{},
	feedback_memory{}
{
	allpass_filters[0] = std::move(std::make_unique<PhaserAllpass>());
}

void Phaser::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	memcpy(out_buf, audio_in_buf, config::buffer_size * sizeof(float_s));
	float_s feedback_buf_sum[config::buffer_size];
	bool feedback_mods{ sum_bufs(BufType::FEEDBACK, feedback_buf_sum, feedback) };
	float_s center_buf_sum[config::buffer_size];
	bool center_mods{ sum_bufs(BufType::CENTER_FREQ, center_buf_sum, center) };

	for (int i{ 0 }; i < config::channel_buffer_size; i++) {
		float_s temp{ audio_in_buf[i] };
		for (int j{ 0 }; j < stages; j++) {
			if (center_mods) {
				allpass_filters[j]->set_cutoff(center_buf_sum[i]);
				allpass_filters[j]->compute_alpha();
				allpass_filters[j]->compute_coefficients();
			}
			temp = allpass_filters[j]->generate_one_sample(temp); // NEED TO GENERATE ONE SAMPLE!!!
		}

		if (feedback_mods) {
			out_buf[i] = temp + feedback_memory * feedback_buf_sum[i];
		}
		else {
			out_buf[i] = temp + feedback_memory * feedback;
		}
		feedback_memory = out_buf[i];
	}

	math::vec_add_float_s(audio_in_buf, out_buf, out_buf, config::buffer_size);

	mix_dry_wet();
}

void Phaser::add_buf(const float_s* buf, uint8_t buf_type) {
	Fx::add_buf(buf, buf_type);

	// would be more efficient to sum all CENTER_FREQ buffers, then pass them to the allpass filters. maybe do this later
	switch (buf_type) {
	case BufType::AUDIO:
		allpass_filters[0]->add_buf(audio_in_buf, Allpass::BufType::AUDIO);
		for (size_t i{ 1 }; i < stages; i++) {
			allpass_filters[i]->add_buf(allpass_filters[i - 1]->get_out_buf(), Allpass::BufType::AUDIO);
		}
		return;
	case BufType::CENTER_FREQ:
		for (const std::unique_ptr<PhaserAllpass>& allpass : allpass_filters) {
			allpass->add_buf(buf, Allpass::BufType::CUTOFF);
		}
		return;
	default:
		return;
	}
}

void Phaser::set_center_freq(const uint16_t value) {
	assert(value > 0);
	center = value;
	for (const std::unique_ptr<PhaserAllpass>& allpass : allpass_filters) {
		allpass->set_cutoff(value);
	}
}

void Phaser::set_stages(const uint8_t value) {
	assert(value > 0);

	uint8_t old_stages{ stages };
	stages = value;

	allpass_filters.resize(stages);
	if (stages > old_stages) {
		for (uint8_t i{ old_stages }; i < stages; i++) {
			allpass_filters[i] = std::move(std::make_unique<PhaserAllpass>());
			allpass_filters[i]->set_cutoff(center);

			for (uint8_t j{ BufType::AUDIO + 1 }; j < sizeof(BufType) / sizeof(BufType::AUDIO); j++) {
				for (const float_s* buf : in_bufs[j]) {
					allpass_filters[i]->add_buf(buf, j);
				}
			}
		}
	}

	if (audio_in_buf) {
		for (size_t i{ old_stages }; i < stages; i++) {
			allpass_filters[i]->add_buf(allpass_filters[i - 1]->get_out_buf(), Allpass::BufType::AUDIO);
		}
	}
}

void Phaser::set_feedback(const float_s value) {
	assert(value > -1.0 && value < 1.0);
	feedback = value;
}