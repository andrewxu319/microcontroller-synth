#include "soft_clip.h"

#include "utils/math.h"

#include <cmath>

using namespace synthesis;

SoftClip::SoftClip()
	: Fx(in_bufs), in_bufs{}, drive{} {
	;
}

void SoftClip::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	bool drive_mods{ sum_bufs(BufType::DRIVE, out_buf, drive) }; // out_buf now stores drive_buf_sum

	if (drive_mods) {
		math::vec_entrywise_mult_float_s(out_buf, audio_in_buf, out_buf, config::buffer_size);
	}
	else {
		math::vec_scal_mult_float_s(audio_in_buf, out_buf, drive, config::buffer_size);
	}

	for (size_t i{}; i < config::buffer_size; i++) {
		out_buf[i] = std::atan(out_buf[i]);
	}

	math::vec_scal_mult_float_s(out_buf, out_buf, 2.0f / 3.141592653589793f, config::buffer_size);
}

void SoftClip::set_drive(float_s value) {
	drive = value;
}