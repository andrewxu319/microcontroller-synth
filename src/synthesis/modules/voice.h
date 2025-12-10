#pragma once

#include "module.h"
#include "oscillator.h"

#include <vector>

namespace synthesis {
	class Voice : public Module {
	public:
		char current_note;

		Voice();
		void note_on(const uint8_t note, const uint8_t velocity);
		void note_off();

	private:
		bool active;
	};
}