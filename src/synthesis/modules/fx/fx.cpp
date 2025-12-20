#include "fx.h"

#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

Fx::Fx(Module** mods_, const uint8_t num_mods)
	: Module::Module(mods_, num_mods),
	wet{ 0 },
	audio_input{},
	audio_in_buf{}
{
}

void Fx::generate_buf() {
	// call this at the end
	// first have the specific fx write the wet output into out_buf
	accelerator::vec_scal_mult_float_s(out_buf, out_buf, wet, config::buffer_size);
	accelerator::vec_mult_add_float_s(audio_in_buf->data, out_buf, out_buf, 1.0f - wet, config::buffer_size);
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