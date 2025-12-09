#include "mixer.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

using namespace synthesis;

Mixer::Mixer() {}

Mixer::Mixer(const utils::NoBaseInit _) : Module(utils::NO_BASE_INIT) {} // dummy constructor

void Mixer::generate_buf() {
	fill(out_buf, out_buf + config::buffer_size, 0.0);
	for (const auto& in_buf : in_bufs) {
		accelerator::vec_add_float32_t(in_buf.second.data, out_buf, out_buf, config::buffer_size);
	}

	update_destination_bufs();
}