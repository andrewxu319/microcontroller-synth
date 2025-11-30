#include "mixer.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

Mixer::Mixer() {}

Mixer::Mixer(const NoBaseInit _) : Module(NO_BASE_INIT) {} // dummy constructor

void Mixer::generate_buf() {
	(*out_buf).fill(0.0);
	for (const auto& input : input_data) {
		accelerator::vec_add<float, Config::buffer_size>(&input.second.in_buf, out_buf, out_buf);
	}

	update_destination_bufs();
}