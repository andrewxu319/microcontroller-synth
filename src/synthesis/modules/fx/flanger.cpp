#include "flanger.h"

#include "utils/accelerator.h"

using namespace synthesis;

Flanger::Flanger()
	: DelayLine(5) { }

void Flanger::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		memory_buffer.reset();
		out_buf[0] = EMPTY_BUF_MARKER;
		feedback_memory = 0.0;
		return;
	}

	DelayLine::generate_buf();
}