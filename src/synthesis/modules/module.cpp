#include "module.h"

#include <cassert>
#include <algorithm>
#include <iterator>
#include <vector>

using namespace synth;

int Module::last_id{ 0 };

Module::Module() 
: id(last_id++), // Initialize const member `id`
  inputs{}, 
  outputs{}, 
	out_buf{} // Initialize `out_buf` to nullptr
{
}

Module::Module(const NoBaseInit) 
: id(-1), // Initialize const member `id` with a dummy value
  inputs{}, 
  outputs{},
	out_buf{} // Initialize `out_buf` to nullptr
{
// dummy constructor
}

void Module::generate_buf() {
	std::fill(out_buf, out_buf + Config::buffer_size, 0.0);

	// do stuff here

	update_destination_bufs();
}

void Module::update_destination_bufs() const {
	for (Module* output : outputs) {
		memcpy(output->input_data[id].in_buf, out_buf, sizeof(float) * Config::buffer_size);
	}
}

void Module::add_input(Module* input) {
	inputs.push_back(input);
	input_data[(*input).id] = InputData{};
}

void Module::add_output(Module* output) {
	outputs.push_back(output);
	if (outputs.size() == 1) {
		out_buf = outputs[0]->input_data[id].in_buf; // store actual output buffer in the first output module. access it w a pointer & edit output module's "input" directly
	}
}