#include "mixer.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

using namespace synthesis;

Mixer::Mixer() {}

Mixer::Mixer(const utils::NoBaseInit _) : Module(utils::NO_BASE_INIT) {} // dummy constructor

void Mixer::generate_buf() {
	bool is_empty{ true };
	memset(out_buf, 0.0f, config::buffer_size * sizeof(float_s));
	for (const auto& in_buf : in_bufs) {
		if (in_buf.second.data[0] != EMPTY_BUF_MARKER) {
			if (is_empty) {
				memcpy(out_buf, in_buf.second.data, config::buffer_size * sizeof(float_s)); // first nonempty buffer, we copy it directly
				is_empty = false;
			}
			else {
				accelerator::vec_add_float_s(in_buf.second.data, out_buf, out_buf, config::buffer_size);
			}
		}
	}
	if (is_empty) {
		out_buf[0] = EMPTY_BUF_MARKER;
	}

	update_destination_bufs();
}