#include "master.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

#include <algorithm>

Master::Master()
	: Mixer(NO_BASE_INIT),
	out_buf{}
{
}

void Master::generate_buf() {
	std::fill(out_buf, out_buf + Config::buffer_size, 0.0f);
	for (const auto& input : input_data) {
		accelerator::vec_add_float(input.second.in_buf, out_buf, out_buf, Config::buffer_size);
	}
}