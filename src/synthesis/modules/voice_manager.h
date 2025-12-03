#pragma once

#include "module.h"
#include "Voice.h"

#include <queue>
#include <map>

namespace synthesis {
	class VoiceManager : public Module {
	public:
		VoiceManager();
		VoiceManager(const vector<Module*> outputs_);
		VoiceManager(const vector<Voice*> outputs_);
		void add_output(Voice* output);
		void add_output(Module* output);
		void note_on(const char note);
		void note_off(const char note);

	private:
		queue<Voice*> active_voices;
		queue<Voice*> inactive_voices;
		multimap<char, Voice*> voices_with_each_note; // high overhead with small voice count? use simple array instead?
	};
}