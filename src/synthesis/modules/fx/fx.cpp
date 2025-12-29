#include "fx.h"

#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

Fx::Fx(vector<const float_s*>* in_bufs_)
	: Module::Module(in_bufs_),
	wet{ 0 },
	audio_in_buf{}
{
}

void Fx::mix_dry_wet() {
	if (in_bufs[WET].empty()) {
		accelerator::vec_scal_mult_float_s(out_buf, out_buf, wet, config::buffer_size);
		if (audio_in_buf[0] != EMPTY_BUF_MARKER) {
			accelerator::vec_mult_add_float_s(audio_in_buf, out_buf, out_buf, 1.0f - wet, config::buffer_size);
		}
	}
	else {
		float_s effective_wet_buf[config::buffer_size];
		sum_bufs(WET, effective_wet_buf);
		accelerator::vec_scal_add_float_s(effective_wet_buf, effective_wet_buf, wet, config::buffer_size);
		accelerator::vec_entrywise_mult_float_s(effective_wet_buf, out_buf, out_buf, config::buffer_size);

		// get 1.0 - wet for dry signal
		accelerator::vec_scal_mult_float_s(effective_wet_buf, effective_wet_buf, -1.0, config::buffer_size);
		accelerator::vec_scal_add_float_s(effective_wet_buf, effective_wet_buf, 1.0, config::buffer_size);
		float_s wet_scaled_audio[config::buffer_size];
		accelerator::vec_entrywise_mult_float_s(effective_wet_buf, audio_in_buf, wet_scaled_audio, config::buffer_size);
		accelerator::vec_add_float_s(wet_scaled_audio, out_buf, out_buf, config::buffer_size);
	}
}

int Fx::add_input(Module* __restrict input, const uint8_t buf_type) {
	assert(in_bufs[0].empty()); // only one audio input alowed
	const int ret{ Module::add_input(input, buf_type) };
	if (ret == 0) {
		audio_in_buf = input->get_out_buf();
	}
	return ret;
}