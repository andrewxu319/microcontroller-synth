#include "mixer.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

using namespace synthesis;

Mixer::Mixer() : Module(in_bufs) {}

void Mixer::generate_buf() {
	bool is_empty{ true };
	memset(out_buf, 0.0f, config::buffer_size * sizeof(float_s));
	for (const float_s* in_buf : in_bufs[BufTypes::AUDIO]) {
		if (in_buf[0] != EMPTY_BUF_MARKER) {
			if (is_empty) {
				memcpy(out_buf, in_buf, config::buffer_size * sizeof(float_s)); // first nonempty buffer, we copy it directly
				is_empty = false;
			}
			else {
				accelerator::vec_add_float_s(in_buf, out_buf, out_buf, config::buffer_size);
			}
		}
	}
	if (is_empty) {
		out_buf[0] = EMPTY_BUF_MARKER;
	}
}