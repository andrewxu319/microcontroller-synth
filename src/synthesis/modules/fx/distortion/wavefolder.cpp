#include "wavefolder.h"

#include "utils/math.h"

using namespace synthesis;

Wavefolder::Wavefolder()
    : Fx(in_bufs), gain{ 1.0f }, offset{}
{}

void Wavefolder::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	bool gain_mods{ sum_bufs(BufType::GAIN, out_buf, gain) }; // out_buf now stores gain_buf_sum
	if (gain_mods) {
		math::vec_entrywise_mult_float_s(out_buf, audio_in_buf, out_buf, config::buffer_size);
	}
	else {
		math::vec_scal_mult_float_s(audio_in_buf, out_buf, gain, config::buffer_size);
	}

    float_s offset_sum_buf[config::buffer_size];
    bool offset_mods{ sum_bufs(BufType::OFFSET, offset_sum_buf, offset) }; // out_buf now stores offset_buf_sum
	if (offset_mods) {
		math::vec_add_float_s(out_buf, offset_sum_buf, out_buf, config::buffer_size);
	}
	else {
		math::vec_scal_add_float_s(out_buf, out_buf, offset, config::buffer_size);
	}
}

// in raw factor, not dB
void Wavefolder::set_gain(float_s value) {
    gain = value;
}

void Wavefolder::set_offset(float_s value) {
    offset = value;
}