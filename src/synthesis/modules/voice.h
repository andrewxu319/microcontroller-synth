#pragma once

#include "module.h"
#include "oscillator.h"

#include <vector>

namespace synthesis {
	class Voice : public Module {
	public:
		char current_note;

		Voice();
		void note_on(const char note);
		void note_off();

	private:
		bool active;
	};
}