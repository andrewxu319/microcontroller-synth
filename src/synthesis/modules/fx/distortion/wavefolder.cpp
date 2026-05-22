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

    math::axpy(audio_in_buf, offset, out_buf, gain, config::buffer_size);
}

// in raw factor, not dB
void Wavefolder::set_gain(float_s value) {
    gain = value;
}

void Wavefolder::set_offset(float_s value) {
    offset = value;
}