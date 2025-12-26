#include "module.h"

#include "synthesis/synthesizer.h"
#include "utils/accelerator.h"

#include <cassert>
#include <algorithm>
#include <iterator>

using namespace synthesis;

int Module::last_id{ 0 };

Module::Module(vector<float*>* mods_, const uint8_t num_mods)
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

void Module::attach_mod(float_s* __restrict mod, uint8_t target) {
	// // turning this off because chorus effect requires mod buffers that are not out_buf's
	//bool found{ false };
	//for (const Module* input : inputs) {
	//	if (mod == input->out_buf) {
	//		found = true;
	//	}
	//}
	//if (!found) {
	//	printf("Invalid modulator: not an input!\n");
	//}

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

float_s* Module::sum_mods(const uint8_t target) {
	if (!mods_ptr[target].empty()) {
		float_s* mod_sum{ mods_ptr[target][0] };
		for (size_t i{ 1 }; i < mods_ptr[target].size(); i++) {
			accelerator::vec_add_float_s(mods_ptr[target][i], mod_sum, mod_sum, config::buffer_size);
		}
		return mod_sum;
	}
	else {
		return nullptr;
	}
}