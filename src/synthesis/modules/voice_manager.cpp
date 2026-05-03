#include "voice_manager.h"

#include "utils/config.h"
#include "synthesis/synthesizer.h"

#include <deque>
#include <iterator>
#include <algorithm>

using namespace synthesis;

VoiceManager::VoiceManager()
	: active_voices{},
	inactive_voices{},  // initialize queue with a vector
	legato{}
{
}

int VoiceManager::add_output(Voice* __restrict output, const uint8_t buf_type) {
	if (Module::add_output(static_cast<Module*>(output), buf_type) == -1) {
		return -1;
	}
	inactive_voices.emplace_back(output);
	return 0;
}

int VoiceManager::add_output(Module* __restrict output, const uint8_t buf_type) {
	if (Module::add_output(output, buf_type) == -1) {
		return -1;
	}
	inactive_voices.emplace_back(static_cast<Voice*>(output));
	return 0;
}

void VoiceManager::note_on(const uint8_t note, const uint8_t velocity) {
	Voice* voice_to_activate{};

	if (inactive_voices.empty()) {
		if (legato && config::num_voices == 1) {
			// remove <note, voice> pair from voices_with_each_note
			using iter = std::multimap<char, Voice*>::iterator;
			std::pair<iter, iter> iter_pair{ voices_with_each_note.equal_range(active_voices.front()->current_note) };
			iter it{ iter_pair.first };
			for (; it != iter_pair.second; it++) {
				if (it->second == active_voices.front()) {
					voices_with_each_note.erase(it);
					break;
				}
			}

			// add new pair to voices_with_each_note
			voices_with_each_note.insert({ note, active_voices.front() });

			active_voices.front()->change_note(note);
			return;
		}

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

void VoiceManager::set_legato(bool value) {
	legato = value;
}