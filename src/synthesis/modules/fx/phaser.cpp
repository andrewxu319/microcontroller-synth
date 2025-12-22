#include "phaser.h"

#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

Phaser::Phaser()
	: Fx(mods, sizeof(mods) / sizeof(vector<Module*>)),
	all_pass_filters{ Dsp::FilterDesign<Dsp::RBJ::Design::AllPass, 1>{} },
	params{},
	stages{},
	center{},
	mods{}
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

	if (mods_ptr[Mods::WET].empty()) {
		accelerator::vec_scal_mult_float_s(audio_in_buf->data, out_buf, wet, config::buffer_size);
	}
	else {
		float_s* effective_wet_buf{ in_bufs[mods_ptr[Mods::WET][0]->id].data };
		for (int i{ 1 }; i < mods_ptr[Mods::WET].size(); i++) {
			accelerator::vec_add_float_s(in_bufs[mods_ptr[Mods::WET][i]->id].data, effective_wet_buf, effective_wet_buf, config::buffer_size);
		}
		accelerator::vec_scal_add_float_s(effective_wet_buf, effective_wet_buf, wet, config::buffer_size);
		accelerator::vec_entrywise_mult_float_s(effective_wet_buf, audio_in_buf->data, out_buf, config::buffer_size);
	}

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
		}
	}
	else {
		all_pass_filters.resize(value);
	}
	stages = value;
}