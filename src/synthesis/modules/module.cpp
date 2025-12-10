#include "module.h"

#include "synthesis/synthesizer.h"

#include <cassert>
#include <algorithm>
#include <iterator>


using namespace synthesis;

int Module::last_id{ 0 };

Module::Module()
	: id{ last_id++ }, // Initialize const member `id`
	inputs{},
	outputs{},
	out_buf{} // Initialize `out_buf` to nullptr
{
}

Module::Module(const utils::NoBaseInit) 
: id(-1), // Initialize const member `id` with a dummy value
  inputs{}, 
  outputs{},
	out_buf{} // Initialize `out_buf` to nullptr
{
// dummy constructor
}

void Module::generate_buf() {
	// do nothing by default
	;
}

void Module::update_destination_bufs() const {
	for (Module* output : outputs) {
		memcpy(output->in_bufs[id].data, out_buf, sizeof(float_s) * config::buffer_size);
	}
}

void Module::add_input(Module* input) {
	input->add_output(this);
	inputs.emplace_back(input);
	in_bufs[input->id] = utils::array_wrapper<float_s, config::buffer_size>{};
}

//void Module::add_inputs(vector<Module*> inputs) {
//	for (const Module* input : inputs) {
//		add_input(input);
//	}
//}
#include <typeinfo>

void Module::add_output(Module* output) {
	outputs.emplace_back(output);
	if (synthesis::topo_sort() == -1) {
		printf("Failed to add output (ID: %d): circular in/out!\n", output->id);
		outputs.pop_back();
	}
	if (outputs.size() == 1) {
		out_buf = outputs[0]->in_bufs[id].data; // store actual output buffer in the first output module. access it w a pointer & edit output module's "input" directly
	}
}

void Module::add_outputs(const vector<Module*> outputs) {
	for (Module* output : outputs) {
		add_output(output);
	}
}