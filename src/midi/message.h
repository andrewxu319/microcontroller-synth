#pragma once

#include "utils/global.h"

namespace midi {
	struct NoteMessage {
		enum class NoteFunction : char {
			NOTE_OFF,
			NOTE_ON
		};

		NoteFunction function;
		uint8_t channel;
		uint8_t note;
		uint8_t velocity;
	};

	struct CcMessage {
		uint8_t function;
		uint8_t channel;
		uint8_t value;
	};
}