#include "module.h"

#include "synthesis/synthesizer.h"
#include "utils/accelerator.h"

#include <cassert>
#include <algorithm>
#include <iterator>

using namespace synthesis;

int Module::last_id{ -1 }; // master is -1
const float_s Module::empty_buf[config::buffer_size] = { 0.0 };

Module::Module(vector<const float*>* in_bufs_)
	: id{ last_id++ }, // Initialize const member `id`
	inputs{},
	outputs{},
	in_bufs{ in_bufs_ },
	out_buf{} // Initialize `out_buf` to nullptr
{
}

void Module::generate_buf() {
	// do nothing by default
	;
}

int Module::add_input(Module* __restrict input, const uint8_t buf_type) { // -1 means no buffer
	inputs.emplace_back(input);
	if (synthesis::topo_sort() == -1) {
		printf("Failed to add input (ID: %d): circular in/out!\n", input->id);
		inputs.pop_back();
		return -1;
	}
	if (buf_type != static_cast<uint8_t>(-1)) {
		in_bufs[buf_type].emplace_back(input->out_buf);
	}
	return 0;
}

int Module::add_output(Module* __restrict output, const uint8_t buf_type) { // -1 means no buffer
	outputs.emplace_back(output);
	if (output->add_input(this, buf_type) == -1) {
		outputs.pop_back();
		return -1;
	}
	return 0;
}

void Module::add_buf(const float_s* __restrict buf, uint8_t buf_type) {
	in_bufs[buf_type].emplace_back(buf);
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

const float_s* Module::get_out_buf() {
	return out_buf;
}

bool Module::sum_bufs(const uint8_t buf_type, float_s* dest) {
	if (in_bufs[buf_type].empty()) {
		return false;
	}

	memset(dest, 0.0, config::buffer_size * sizeof(float_s));
	for (const float_s* in_buf : in_bufs[buf_type]) {
		accelerator::vec_add_float_s(in_buf, dest, dest, config::buffer_size);
	}
	return true;
}