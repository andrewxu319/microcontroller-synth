#include "module.h"

#include "synthesis/synthesizer.h"

#include <cassert>
#include <algorithm>
#include <iterator>

using namespace synthesis;

int Module::last_id{ 0 };

Module::Module(vector<Module*>* mods_, const uint8_t num_mods)
	: id{ last_id++ }, // Initialize const member `id`
	inputs{},
	outputs{},
	out_buf{}, // Initialize `out_buf` to nullptr
	mods_ptr{ mods_ }
{
}

Module::Module(const utils::NoBaseInit) 
	: id(-1), // Initialize const member `id` with a dummy value
	inputs{}, 
	outputs{},
	out_buf{}, // Initialize `out_buf` to nullptr
	mods_ptr{}
{
// dummy constructor
}

void Module::generate_buf() {
	// do nothing by default
	;
}

void Module::update_destination_bufs() const { // needed if more than one output
	for (Module* output : outputs) {
		if (output->in_bufs.count(id) && output->in_bufs[id].data != out_buf) {
			memcpy(output->in_bufs[id].data, out_buf, sizeof(float_s) * config::buffer_size);
		}
	}
}

int Module::add_input(Module* __restrict input, bool add_buf) {
	inputs.emplace_back(input);
	if (synthesis::topo_sort() == -1) {
		printf("Failed to add input (ID: %d): circular in/out!\n", input->id);
		inputs.pop_back();
		return -1;
	}
	if (add_buf) {
		in_bufs[input->id] = utils::array_wrapper<float_s, config::buffer_size>{};
	}
	return 0;
}

int Module::add_output(Module* __restrict output, bool add_buf) {
	outputs.emplace_back(output);
	if (output->add_input(this, add_buf) == -1) {
		outputs.pop_back();
		return -1;
	}
	if (outputs.size() == 1 && add_buf) {
		out_buf = outputs[0]->in_bufs[id].data; // store actual output buffer in the first output module. access it w a pointer & edit output module's "input" directly
	}
	return 0;
}

void Module::attach_mod(Module* __restrict mod, uint8_t target) {
	assert(find(inputs.begin(), inputs.end(), mod) != inputs.end());
	mods_ptr[target].emplace_back(mod);
}

void Module::note_on(const uint8_t note, const uint8_t velocity) {
	for (Module* output : outputs) {
		output->note_on(note, velocity);
	}
}
void Module::note_off() {
	for (Module* output : outputs) {
		output->note_off();
	}
}