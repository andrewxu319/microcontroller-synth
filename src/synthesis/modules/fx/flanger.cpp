#include "flanger.h"

#include "utils/math.h"

using namespace synthesis;

Flanger::Flanger()
	: DelayLine(5, 1) {
}

void Flanger::generate_buf() {
	if (in_bufs[BufType::AUDIO][0][0] == EMPTY_BUF_MARKER) {
		channels[0].memory_buffer.reset();
		out_bufs[0].data()[0] = EMPTY_BUF_MARKER;
		channels[0].feedback_memory = 0.0;
		return;
	}

	DelayLine::generate_buf();
}