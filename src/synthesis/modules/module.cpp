#include "module.h"

#include <cassert>
#include <algorithm>
#include <iterator>

using namespace synth;

Module::Module() {
	std::vector<Module*>inputs{};
	std::vector<Module*>outputs{};

	init();
}

Module::Module(const NoBaseInit) {
	// dummy constructor
	out_buf = nullptr;
}

void Module::init() {
	assert(outputs.size() > 0);
	out_buf = &outputs[0]->input_data[id].in_buf; // store actual output buffer in the first output module. access it w a pointer & edit output module's "input" directly
}

void Module::generate_buf() {
	(*out_buf).fill(0.0);

	// do stuff here

	update_destination_bufs();
}

void Module::update_destination_bufs() const {
	for (Module* output : outputs) {
		output->input_data[id].in_buf = *out_buf;
	}
}