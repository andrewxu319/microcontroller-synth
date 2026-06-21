#include "master.h"

#include "utils/timer.h"
#include "utils/math.h"
#include "synthesis/synthesizer.h"

#include <algorithm>

using namespace synthesis;

Master::Master()
	: out_buf{}
{
}

Master& Master::instance() {
	static Master master{};
	return master;
}

void Master::generate_buf() {
	utils::timer::start();

	synthesis::read_messages();

	for (size_t i{ 0 }; i < modules.size(); i++) {
		modules[i]->generate_buf();
	}

	bool is_empty{ true };
	for (const float_s* in_buf : in_bufs[Mixer::BufType::AUDIO]) {
		if (in_buf[0] != EMPTY_BUF_MARKER) {
			if (is_empty) {
				memcpy(out_buf, in_buf, config::buffer_size * sizeof(float_s)); // first nonempty buffer, we copy it directly
				is_empty = false;
			}
			else {
				math::vec_add_float_s(in_buf, out_buf, out_buf, config::buffer_size);
			}
		}
	}
	if (is_empty) {
		memset(out_buf, 0, config::buffer_size * sizeof(float_s));
	}

	utils::timer::end("master");

	// clip between -1.0 and 1.0
}