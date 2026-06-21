#include "synthesizer.h"

#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <functional>

using namespace synthesis;

Synthesizer::Synthesizer()
	: master{ nullptr },
	voice_manager{ nullptr },
	modules{},
	note_messages{},
	cc_messages{},
	cc_mappings{} { }

Synthesizer& Synthesizer::instance() {
	static Synthesizer synthesizer{};
	return synthesizer;
}

void Synthesizer::init() {
	for (const std::unique_ptr<Module>& module : modules) {
		module->init();
	}
}

Module* Synthesizer::add_module(std::unique_ptr<Module> module) {
	// must be adding master
	Module* ret_ptr{ module.get() };
	modules.emplace_back(std::move(module));
	return ret_ptr;
}

// point of this is to figure out which modules need to have their buffers generated first, because a module needs
// all input buffers generated before it can generate its own buffer
int Synthesizer::topo_sort() {
	const size_t len{ modules.size() };
	std::unordered_set<int> visiting{};
	std::unordered_set<int> visited{};
	std::unordered_map<Module*, int> indices{};
	std::vector<std::unique_ptr<Module>> sorted(len);
	std::stack<int> stack{};
	size_t sorted_index{ len - 1 };

	// take care of Master (make sure we don't consider it an output)
	visited.insert(-1);
	indices[master] = -1;

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
					break;
				}
			}

			if (all_outputs_visited) {
				visited.insert(current);
				visiting.erase(stack.top());
				stack.pop();
				sorted[sorted_index] = std::move(modules[current]);
				sorted_index--;
			}
		}

	}
	modules = std::move(sorted);
	return 0;
}

void Synthesizer::read_messages() {
	// check for messages here
	while (!note_messages.empty()) {
		const midi::NoteMessage& note_message{ note_messages.front() };
		note_messages.pop();
		printf("Note message: function %d, channel %d, note %d, velocity %d\n", static_cast<int>(note_message.function), note_message.channel, note_message.note, note_message.velocity);

		switch (note_message.function) {
		case midi::NoteMessage::NoteFunction::NOTE_OFF:
			voice_manager->note_off(note_message.note);
			break;
		case midi::NoteMessage::NoteFunction::NOTE_ON:
			voice_manager->note_on(note_message.note, note_message.velocity);
			break;
		default:
			break;
		}
	}
	while (!cc_messages.empty()) {
		midi::CcMessage& cc_message{ cc_messages.front() };
		cc_messages.pop();
		printf("CC message: function %d, channel %d, value %d\n", cc_message.function, cc_message.channel, cc_message.value);

		if (!cc_mappings[cc_message.function].empty()) {
			for (std::function<void(uint8_t)> fn : cc_mappings[cc_message.function]) {
				fn(cc_message.value);
			}
		}
	}
}

void Synthesizer::reset_cc(const uint8_t cc) { // might need to add ways to remove specific attachments later
	cc_mappings[cc].clear();
}