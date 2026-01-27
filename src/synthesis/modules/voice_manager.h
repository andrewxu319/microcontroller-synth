#pragma once

#include "module.h"
#include "voice.h"
#include "utils/global.h"

#include <map>
#include <unordered_map>
#include <array>

namespace synthesis {
	class VoiceManager : public Module {
	public:
		VoiceManager();
		int add_output(Voice* __restrict output, const uint8_t buf_type = -1);
		int add_output(Module* __restrict output, const uint8_t buf_type = -1);
		void note_on(const uint8_t note, const uint8_t velocity);
		void note_off(const uint8_t note);

	private:
		vector<Voice*> active_voices; // vectors are faster than queues here
		vector<Voice*> inactive_voices;
		multimap<char, Voice*> voices_with_each_note; // high overhead with small voice count? use simple array instead?
	};
}