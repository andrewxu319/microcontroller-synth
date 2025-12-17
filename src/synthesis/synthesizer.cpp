#include "synthesizer.h"

#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace synthesis {
	Master* master{ &Master::instance() };
	VoiceManager* voice_manager{};
	vector<unique_ptr<Module>> modules{};
	extern queue<midi::NoteMessage> note_messages{};
	extern queue<midi::CcMessage> cc_messages{};
	array<vector<function<void(uint8_t)>>, 128> cc_mappings{};

	Module* add_module(unique_ptr<Module> module) {
		// must be adding master
		Module* ret_ptr{ module.get() };
		modules.emplace_back(move(module));
		return ret_ptr;
	}

	// point of this is to figure out which modules need to have their buffers generated first, because a module needs
	// all input buffers generated before it can generate its own buffer
	int topo_sort() {
		const size_t len{ modules.size() };
		unordered_set<int> visiting{};
		unordered_set<int> visited{};
		unordered_map<Module*, int> indices{};
		vector<unique_ptr<Module>> sorted(len);
		stack<int> stack{};
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
					sorted[sorted_index] = move(modules[current]);
					sorted_index--;
				}
			}

		}
		modules = move(sorted);
		return 0;
	}

	void read_messages() {
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
				for (function<void(uint8_t)> fn : cc_mappings[cc_message.function]) {
					fn(cc_message.value);
				}
			}
		}
	}

	void reset_cc(const uint8_t cc) { // might need to add ways to remove specific attachments later
		cc_mappings[cc].clear();
	}
}
