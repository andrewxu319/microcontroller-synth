#include "voice_manager.h"

#include "utils/config.h"
#include "synthesis/synthesizer.h"

#include <deque>
#include <iterator>
#include <algorithm>

using namespace synthesis;

VoiceManager::VoiceManager()
	: active_voices{},
	inactive_voices{}  // initialize queue with a vector
{
}

int VoiceManager::add_output(Voice* output, bool add_buf) {
	if (Module::add_output(static_cast<Module*>(output), add_buf) == -1) {
		return -1;
	}
	inactive_voices.emplace_back(output);
	return 0;
}

int VoiceManager::add_output(Module* output, bool add_buf) {
	if (Module::add_output(output, add_buf) == -1) {
		return -1;
	}
	inactive_voices.emplace_back(static_cast<Voice*>(output));
	return 0;
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