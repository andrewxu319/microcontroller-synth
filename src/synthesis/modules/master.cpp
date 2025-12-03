#include "master.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

#include <algorithm>

using namespace synthesis;

Master::Master()
	: Mixer(NO_BASE_INIT),
	out_buf{}
{
}

void Master::generate_buf() {
	fill(out_buf, out_buf + config::buffer_size, 0.0f);
	for (const auto& in_buf : in_bufs) {
		accelerator::vec_add_float32_t(in_buf.second.data, out_buf, out_buf, config::buffer_size);
		// clip between -1.0 and 1.0
	}
}

