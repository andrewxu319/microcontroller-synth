#include "fx.h"

#include "utils/math.h"

using namespace synthesis;

Fx::Fx(std::vector<const float_s*>* in_bufs_)
	: Module::Module(in_bufs_),
	wet{ 1.0 },
	audio_in_buf{}
{
}

void Fx::mix_dry_wet() {
	if (in_bufs_ptr[WET].empty()) {
		math::vec_scal_mult_float_s(out_buf, out_buf, wet, config::buffer_size);
		if (audio_in_buf[0] != EMPTY_BUF_MARKER) {
			math::axpy(audio_in_buf, out_buf, out_buf, 1.0f - wet, config::buffer_size);
		}
	}
	else {
		float_s effective_wet_buf[config::buffer_size];
		sum_bufs(WET, effective_wet_buf, wet);
		math::vec_entrywise_mult_float_s(effective_wet_buf, out_buf, out_buf, config::buffer_size);

		// get 1.0 - wet for dry signal
		math::vec_scal_mult_float_s(effective_wet_buf, effective_wet_buf, -1.0, config::buffer_size);
		math::vec_scal_add_float_s(effective_wet_buf, effective_wet_buf, 1.0, config::buffer_size);
		float_s wet_scaled_audio[config::buffer_size];
		math::vec_entrywise_mult_float_s(effective_wet_buf, audio_in_buf, wet_scaled_audio, config::buffer_size);
		math::vec_add_float_s(wet_scaled_audio, out_buf, out_buf, config::buffer_size);
	}
}

int Fx::add_input(Module* __restrict input, const uint8_t buf_type) {
	assert(in_bufs_ptr[0].empty()); // only one audio input alowed
	const int ret{ Module::add_input(input, buf_type) };
	if (ret == 0 && buf_type == AUDIO && in_bufs_ptr[AUDIO].size() == 1) {
		audio_in_buf = input->get_out_buf();
	}
	return ret;
}

int Fx::add_input(MultichannelModule* __restrict input, const uint8_t buf_type) {
	if (buf_type != -1) {
		printf("Cannot add multichannel input to fx with specified buffer type. Only one buffer allowed---must add manually.\n");
	}
	return add_input(static_cast<Module*>(input), -1);
}

void Fx::add_buf(const float_s* buf, uint8_t buf_type) {
	Module::add_buf(buf, buf_type);

	if (buf_type == AUDIO && in_bufs_ptr[AUDIO].size() == 1) {
		audio_in_buf = buf;
	}
}

void Fx::set_wet(float_s value) {
	wet = value;
}