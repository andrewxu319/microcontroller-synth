#include "master.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

#include <algorithm>
#include <stack>
#include <unordered_set>
#include <unordered_map>

using namespace synthesis;

Master::Master()
	: Mixer(utils::NO_BASE_INIT),
	out_buf{},
	modules{},
	note_messages{},
	cc_messages{}
{
}

Master& Master::instance() {
	static Master master{};
	return master;
}

void Master::generate_buf() {
	// check for messages here
	while (!note_messages.empty()) {
		midi::NoteMessage& note_message{ note_messages.front() };
		note_messages.pop();
		printf("Note message: function %d, channel %d, note %d, velocity %d\n", static_cast<int>(note_message.function), note_message.channel, note_message.note, note_message.velocity);
	}
	while (!cc_messages.empty()) {
		midi::CcMessage& cc_message{ cc_messages.front() };
		cc_messages.pop();
		printf("CC message: function %d, channel %d, value %d\n", cc_message.function, cc_message.channel, cc_message.value);
	}

	for (int i{ 0 }; i < modules.size(); i++) {
		modules[i]->generate_buf();
	}

	fill(out_buf, out_buf + config::buffer_size, 0.0f);
	for (const auto& in_buf : in_bufs) {
		accelerator::vec_add_float32_t(in_buf.second.data, out_buf, out_buf, config::buffer_size);
		// clip between -1.0 and 1.0
	}
}

Module* Master::add_module(unique_ptr<Module> module) {
	// must be adding master
	Module* ret_ptr{ module.get() };
	modules.emplace_back(move(module));
	return ret_ptr;
}

// point of this is to figure out which modules need to have their buffers generated first, because a module needs
// all input buffers generated before it can generate its own buffer
int Master::topo_sort() {
	const size_t len{ modules.size() };
	unordered_set<int> visiting{};
	unordered_set<int> visited{};
	unordered_map<Module*, int> indices{};
	vector<unique_ptr<Module>> sorted(len);
	stack<int> stack{};
	size_t sorted_index{ len - 1 };

	// take care of Master (make sure we don't consider it an output)
	visited.insert(-1);
	indices[this] = -1;

	for (int i{ 0 }; i < len; i++) {
		indices[modules[i].get()] = i;
	}
	for (int i{ 0 }; i < len; i++) {
		if (visited.count(i)) continue;
		stack.push(i);
		visiting.insert(i);
		while (!stack.empty()) {
			const int current{ stack.top() };
			if (visited.count(current)) continue;

			bool all_outputs_visited{ true };
			for (Module* output : modules[current]->outputs) {
				const int output_index{ indices[output] };
				if (!visited.count(output_index)) {
					if (visiting.count(output_index)) {
						// cycle detected
						return -1;
					}
					stack.push(output_index);
					visiting.insert(output_index);
					all_outputs_visited = false;
				}
			}
			
			if (all_outputs_visited) {
				visited.insert(current);
				visiting.erase(stack.top());
				stack.pop();
				sorted[sorted_index] = move(modules[current]);
				sorted_index--;
			}
		}

	}
	modules = move(sorted);
	return 0;
}