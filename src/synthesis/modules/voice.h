#pragma once

#include "module.h"
#include "oscillator.h"

#include <vector>

namespace synthesis {
	class Voice : public Module {
	public:
		char current_note;

		Voice(const vector<Module*> outputs_);
		Voice(const vector<Oscillator*> outputs_);
		void note_on(const char note);
		void note_off();

	private:
		bool active;
	};
}