#include "phaser.h"

#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

Phaser::Phaser()
	: Fx(mods, sizeof(mods) / sizeof(vector<Module*>)),
	all_pass_filters{ Dsp::FilterDesign<Dsp::RBJ::Design::AllPass, 1>{} },
	feedback_filters{ Dsp::FilterDesign<Dsp::RBJ::Design::AllPass, 1>{} },
	params{},
	stages{},
	center{},
	mods{},
	feedback{}
{
	params[0] = config::sample_rate;
	params[2] = 1.0;
	all_pass_filters.back().setParams(params);
}

void Phaser::generate_buf() {
	if (audio_in_buf->data[0] == EMPTY_BUF_MARKER) {
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	memcpy(out_buf, audio_in_buf->data, config::buffer_size * sizeof(float_s));

	for (int i{ 0 }; i < config::actual_buffer_size; i += config::control_rate) {
		if (!mods[Mods::CENTER_FREQ].empty()) {
			params[1] = center;
			for (const Module* module : mods[Mods::CENTER_FREQ]) {
				params[1] += in_bufs[module->id].data[i];
			}
			for (int j{ 0 }; j < stages; j++) {
				all_pass_filters[j].setParams(params);
			}
		}

		float* data[1]{ out_buf + i }; // if mono. data[1] means array of 1 array pointer
		for (int j{ 0 }; j < stages; j++) {
			all_pass_filters[j].process(config::control_rate, data);
		}
	}

	accelerator::vec_add_float_s(audio_in_buf->data, out_buf, out_buf, config::buffer_size);

	// // this implementation is wrong
	// // refer to 12/21 commit for optimized dry wet mix code
	//float_s feedback_signal[config::buffer_size]{};
	//memcpy(feedback_signal, out_buf, config::buffer_size * sizeof(float_s));
	//float* data[1]{ feedback_signal }; // if mono. data[1] means array of 1 array pointer
	//for (int i{ 0 }; i < stages; i++) {
	//	all_pass_filters[i].reset();
	//	all_pass_filters[i].process(config::buffer_size, data);
	//}
	//if (mods[Mods::FEEDBACK].empty()) {
	//	accelerator::vec_mult_add_float_s(feedback_signal, out_buf, out_buf, feedback, config::buffer_size);
	//}
	//else {
	//	float_s* effective_feedback{ in_bufs[mods[Mods::FEEDBACK][0]->id].data };
	//	for (int i{ 1 }; i < mods[Mods::FEEDBACK].size(); i++) {
	//		accelerator::vec_add_float_s(in_bufs[mods[Mods::FEEDBACK][i]->id].data, effective_feedback, effective_feedback, config::buffer_size);
	//	}
	//	accelerator::vec_scal_add_float_s(effective_feedback, effective_feedback, feedback, config::buffer_size);
	//	accelerator::vec_entrywise_mult_float_s(effective_feedback, feedback_signal, feedback_signal, config::buffer_size);
	//	accelerator::vec_add_float_s(feedback_signal, out_buf, out_buf, config::buffer_size);
	//}

	mix_dry_wet(Mods::WET);
}

void Phaser::set_center_freq(const double value) {
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
			feedback_filters.emplace_back(Dsp::FilterDesign<Dsp::RBJ::Design::AllPass, 1>{all_pass_filters.back()});
		}
	}
	else {
		all_pass_filters.resize(value);
	}
	stages = value;
}

void Phaser::set_feedback(const float_s value) {
	feedback = value;
}