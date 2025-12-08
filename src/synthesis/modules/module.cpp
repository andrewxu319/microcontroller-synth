#include "module.h"

#include <cassert>
#include <algorithm>
#include <iterator>


using namespace synthesis;

int Module::last_id{ 0 };

Module::Module(const vector<Module*>& outputs_ = vector<Module*>{})
	: id{ last_id++ }, // Initialize const member `id`
	inputs{},
	outputs{},
	out_buf{} // Initialize `out_buf` to nullptr
{
	add_outputs(outputs_);
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
	// do nothing by default
	;
}

void Module::update_destination_bufs() const {
	for (Module* output : outputs) {
		memcpy(output->in_bufs[id].data, out_buf, sizeof(float32_t) * config::buffer_size);
	}
}

void Module::add_input(Module* input) {
	inputs.emplace_back(input);
	in_bufs[(*input).id] = array_wrapper<float32_t, config::buffer_size>{};
}

//void Module::add_inputs(vector<Module*> inputs) {
//	for (const Module* input : inputs) {
//		add_input(input);
//	}
//}
#include <typeinfo>

void Module::add_output(Module* output) {
	output->add_input(this);
	outputs.push_back(output);
	if (outputs.size() == 1) {
		out_buf = outputs[0]->in_bufs[id].data; // store actual output buffer in the first output module. access it w a pointer & edit output module's "input" directly
	}
}

void Module::add_outputs(vector<Module*> outputs) {
	for (Module* output : outputs) {
		add_output(output);
	}
}