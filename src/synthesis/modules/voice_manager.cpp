#include "voice_manager.h"

#include "utils/config.h"
#include "synthesis/synthesizer.h"

#include <deque>
#include <iterator>
#include <algorithm>

using namespace synthesis;

VoiceManager::VoiceManager()
	: active_voices{},
	inactive_voices{} {  // initialize queue with a vector
	for (Module* output : outputs) {
		inactive_voices.emplace_back(static_cast<Voice*>(output));
	}
}

void VoiceManager::add_output(Voice* output) {
	outputs.emplace_back(output);
	if (synthesis::topo_sort() == -1) {
		printf("Action invalid: circular in/out!\n");
		outputs.pop_back();
	}
	inactive_voices.emplace_back(output);
	if (outputs.size() == 1) {
		out_buf = outputs[0]->in_bufs[id].data; // store actual output buffer in the first output module. access it w a pointer & edit output module's "input" directly
	}
}

void VoiceManager::add_output(Module* output) {
	add_output(static_cast<Voice*>(output));
}

void VoiceManager::note_on(const uint8_t note, const uint8_t velocity) {
	Voice* voice_to_activate{};

	if (inactive_voices.empty()) {
		voice_to_activate = active_voices.front();
		note_off(voice_to_activate->current_note);
	}

	voice_to_activate = inactive_voices.back();
	inactive_voices.pop_back();
	active_voices.emplace_back(voice_to_activate);
	voices_with_each_note.insert({ note, voice_to_activate });
	voice_to_activate->note_on(note, velocity); // it's fine to note_on a voice without note_off first---it'll just switch to the new note
}

void VoiceManager::note_off(const uint8_t note) {
	// some potentially redundant input checks
	const auto match{ voices_with_each_note.find(note) };
	if (match == voices_with_each_note.end()) {
		return;
	}
	match->second->note_off();
	active_voices.erase(remove(active_voices.begin(), active_voices.end(), match->second));
	inactive_voices.emplace_back(match->second);
	voices_with_each_note.erase(match);
}