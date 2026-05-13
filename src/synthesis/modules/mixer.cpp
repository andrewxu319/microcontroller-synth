#include "mixer.h"
#include "utils/utils.h"
#include "utils/math.h"

using namespace synthesis;

Mixer::Mixer()
	: Module(in_bufs),
	in_bufs{},
	gains{}
{}

void Mixer::generate_buf() {
	bool is_empty{ true };
	memset(out_buf, 0, config::buffer_size * sizeof(float_s));
	for (const float_s* in_buf : in_bufs[BufType::AUDIO]) {
		if (in_buf[0] != EMPTY_BUF_MARKER) {
			if (is_empty) {
				memcpy(out_buf, in_buf, config::buffer_size * sizeof(float_s)); // first nonempty buffer, we copy it directly
				if (gains[in_buf] != 1.0) {
					math::vec_scal_mult_float_s(out_buf, out_buf, gains[in_buf], config::buffer_size);
				}
				is_empty = false;
			}
			else {
				if (gains[in_buf] == 1.0) {
					math::vec_add_float_s(in_buf, out_buf, out_buf, config::buffer_size);
				}
				else {
					math::vec_mult_add_float_s(in_buf, out_buf, out_buf, gains[in_buf], config::buffer_size);
				}
			}
		}
	}
	if (is_empty) {
		out_buf[0] = EMPTY_BUF_MARKER;
	}
}

void Mixer::add_buf(const float_s* buf, uint8_t buf_type) {
	if (buf_type == BufType::AUDIO) {
		gains[buf] = 1.0;
	}

	Module::add_buf(buf, buf_type);
}

void Mixer::set_in_buf_gain(const float_s* __restrict in_buf, const float_s value) {
	gains[in_buf] = value;
}