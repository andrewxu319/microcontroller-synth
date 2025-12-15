#pragma once

#include "module.h"
#include "voice.h"
#include "utils/includes.h"

#include <map>
#include <unordered_map>
#include <array>

namespace synthesis {
	class VoiceManager : public Module {
	public:
		VoiceManager();
		int add_output(Voice* output, bool add_buf);
		int add_output(Module* output, bool add_buf);
		void note_on(const uint8_t note, const uint8_t velocity);
		void note_off(const uint8_t note);

	private:
		vector<Voice*> active_voices; // vectors are faster than queues here
		vector<Voice*> inactive_voices;
		multimap<char, Voice*> voices_with_each_note; // high overhead with small voice count? use simple array instead?
	};
}