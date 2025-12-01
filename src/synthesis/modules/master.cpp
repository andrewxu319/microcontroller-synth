#include "master.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

Master::Master() : Mixer(NO_BASE_INIT) {
	std::vector<Module*>inputs{};
}

void Master::generate_buf() {
	out_buf_to_sound_engine.fill(0.0);
	for (const auto& input : input_data) {
		accelerator::vec_add<float, Config::buffer_size>(&input.second.in_buf, &out_buf_to_sound_engine, &out_buf_to_sound_engine);
	}
}