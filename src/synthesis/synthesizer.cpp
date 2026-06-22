#include "synthesizer.h"

#include "utils/timer.h"

#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <functional>

using namespace synthesis;

Synthesizer::Synthesizer()
	: master{},
	voice_manager{},
	modules{},
	depth_0_modules{},
	note_messages{},
	cc_messages{},
	cc_mappings{}
{
	add_module(std::make_unique<Master>());
	master = static_cast<Master*>(modules.back().get());
	
	add_module(std::make_unique<VoiceManager>());
	voice_manager = static_cast<VoiceManager*>(modules.back().get());
}

void Synthesizer::init() {
	if (topo_sort() != 0) {
		printf("Cycle detected!\n");
	}

	depth_0_modules.clear();
	depth_0_modules.resize(0);
	for (const std::unique_ptr<Module>& module : modules) {
		if (module->inputs.size() == 0) {
			depth_0_modules.emplace_back(module.get());
		}
		module->init(); // is this needed if dynamically updating the DAG?
	}
}

Module* Synthesizer::add_module(std::unique_ptr<Module> module) {
	// must be adding master
	Module* ret_ptr{ module.get() };
	modules.emplace_back(std::move(module));
	return ret_ptr;
}

// not really needed atp, but keeping it for cycle detection + for fun
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

#ifdef TEENSY
void Synthesizer::generate_buf() {
#else
void Synthesizer::generate_buf(float_s* out_buf) {
	master->out_buf = out_buf;
#endif
	read_messages();
	
	utils::timer::start();

	// for (std::unique_ptr<Module>& module : modules) {
	// 	module->num_dependencies_visited = 0;
	// }
	// master->num_dependencies_visited = 0;

	for (Module* module : depth_0_modules) {
		module->generate_buf();
	}

	utils::timer::end("generate_buf");
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