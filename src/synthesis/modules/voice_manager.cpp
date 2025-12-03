#include "voice_manager.h"
#include "utils/config.h"

#include <deque>
#include <iterator>

using namespace synthesis;

VoiceManager::VoiceManager()
	: VoiceManager(vector<Module*>{}) {
}

VoiceManager::VoiceManager(const vector<Module*> outputs_)
	: Module(outputs_), // convert it to a vector of Module*'s
	active_voices{},
	inactive_voices{} { // initialize queue with a vector
	for (Module* output : outputs) {
		inactive_voices.emplace(static_cast<Voice*>(output));
	}
}

VoiceManager::VoiceManager(const vector<Voice*> outputs_)
	: VoiceManager(vector<Module*>{ outputs_.begin(), outputs_.end() }) {
}

void VoiceManager::add_output(Voice* output) {
	output->add_input(this);
	outputs.push_back(output);
	inactive_voices.emplace(output);
	if (outputs.size() == 1) {
		out_buf = outputs[0]->in_bufs[id].data; // store actual output buffer in the first output module. access it w a pointer & edit output module's "input" directly
	}
}

void VoiceManager::add_output(Module* output) {
	add_output(static_cast<Voice*>(output));
}

void VoiceManager::note_on(const char note) {
	Voice* voice_to_activate{};

	if (inactive_voices.empty()) {
		voice_to_activate = active_voices.front();
		active_voices.pop();
	}
	else {
		voice_to_activate = inactive_voices.front();
		inactive_voices.pop();
	}

	active_voices.emplace(voice_to_activate);
	voices_with_each_note.insert({ note, voice_to_activate });
	voice_to_activate->note_on(note);
}

void VoiceManager::note_off(const char note) {
	// some potentially redundant input checks
	const auto match{ voices_with_each_note.find(note) };
	if (match == voices_with_each_note.end()) {
		printf("Called note_off() on note not played");
		return;
	}
	match->second->note_off();
	voices_with_each_note.erase(match);
}