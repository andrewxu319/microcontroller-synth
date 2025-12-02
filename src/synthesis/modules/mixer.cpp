#include "mixer.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

Mixer::Mixer() {}

Mixer::Mixer(const NoBaseInit _) : Module(NO_BASE_INIT) {} // dummy constructor

void Mixer::generate_buf() {
	std::fill(out_buf, out_buf + config::buffer_size, 0.0);
	for (const auto& input : input_data) {
		accelerator::vec_add_float32_t(input.second.in_buf, out_buf, out_buf, config::buffer_size);
	}

	update_destination_bufs();
}