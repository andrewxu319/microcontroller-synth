#include "fx.h"

#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

Fx::Fx(vector<Module*>* mods_, const uint8_t num_mods)
	: Module::Module(mods_, num_mods),
	wet{ 0 },
	audio_input{},
	audio_in_buf{}
{
}

void Fx::mix_dry_wet(const uint8_t wet_mod) {
	if (mods_ptr[wet_mod].empty()) {
		accelerator::vec_scal_mult_float_s(out_buf, out_buf, wet, config::buffer_size);
		if (audio_in_buf->data[0] != EMPTY_BUF_MARKER) {
			accelerator::vec_mult_add_float_s(audio_in_buf->data, out_buf, out_buf, 1.0f - wet, config::buffer_size);
		}
	}
	else {
		float_s* effective_wet_buf{ in_bufs[mods_ptr[wet_mod][0]->id].data };
		for (int i{ 1 }; i < mods_ptr[wet_mod].size(); i++) {
			accelerator::vec_add_float_s(in_bufs[mods_ptr[wet_mod][i]->id].data, effective_wet_buf, effective_wet_buf, config::buffer_size);
		}
		accelerator::vec_scal_add_float_s(effective_wet_buf, effective_wet_buf, wet, config::buffer_size);
		accelerator::vec_entrywise_mult_float_s(effective_wet_buf, out_buf, out_buf, config::buffer_size);

		// get 1.0 - wet for dry signal
		accelerator::vec_scal_mult_float_s(effective_wet_buf, effective_wet_buf, -1.0, config::buffer_size);
		accelerator::vec_scal_add_float_s(effective_wet_buf, effective_wet_buf, 1.0, config::buffer_size);
		accelerator::vec_entrywise_mult_float_s(effective_wet_buf, audio_in_buf->data, audio_in_buf->data, config::buffer_size);
		accelerator::vec_add_float_s(audio_in_buf->data, out_buf, out_buf, config::buffer_size);
	}
}

int Fx::add_input(Module* __restrict input, bool add_buf) {
	const int ret{ Module::add_input(input, add_buf) };
	if (add_buf) {
		audio_in_buf = &in_bufs.begin()->second;
	}
	return ret;
}

void Fx::set_audio_input(Module* __restrict audio_input_) {
	assert(find(inputs.begin(), inputs.end(), audio_input_) != inputs.end());
	audio_input = audio_input_;
	audio_in_buf = &in_bufs[audio_input->id];
}