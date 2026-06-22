#include "master.h"

#include "utils/math.h"
#include "synthesis/synthesizer.h"

#include <algorithm>

using namespace synthesis;

Master::Master()
	: out_buf{}
{
}

void Master::generate_buf() {
	bool is_empty{ true };
	for (const float_s* in_buf : in_bufs[Mixer::BufType::AUDIO]) {
		if (in_buf[0] != EMPTY_BUF_MARKER) {
			if (is_empty) {
				memcpy(out_buf, in_buf, config::buffer_size * sizeof(float_s)); // first nonempty buffer, we copy it directly
				is_empty = false;
			}
			else {
				math::vec_add_float_s(in_buf, out_buf, out_buf, config::buffer_size);
			}
		}
	}
	if (is_empty) {
		memset(out_buf, 0, config::buffer_size * sizeof(float_s));
	}

	Module::generate_buf();

	// clip between -1.0 and 1.0
}