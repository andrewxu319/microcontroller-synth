#include "master.h"

#include "utils/utils.h"
#include "utils/accelerator.h"
#include "synthesis/synthesizer.h"

#include <algorithm>

using namespace synthesis;

Master::Master()
	: Mixer(utils::NO_BASE_INIT),
	out_buf{}
{
}

Master& Master::instance() {
	static Master master{};
	return master;
}

void Master::generate_buf() {
	//utils::timer::start();

	synthesis::read_messages();

	for (size_t i{ 0 }; i < modules.size(); i++) {
		modules[i]->generate_buf();
		modules[i]->update_destination_bufs();
	}

	bool is_empty{ true };
	for (const auto& in_buf : in_bufs) {
		if (in_buf.second.data[0] != EMPTY_BUF_MARKER) {
			if (is_empty) {
				memcpy(out_buf, in_buf.second.data, config::buffer_size * sizeof(float_s)); // first nonempty buffer, we copy it directly
				is_empty = false;
			}
			else {
				accelerator::vec_add_float_s(in_buf.second.data, out_buf, out_buf, config::buffer_size);
			}
		}
	}
	if (is_empty) {
		memset(out_buf, 0, config::buffer_size * sizeof(float_s));
	}

	//utils::timer::end();

	// clip between -1.0 and 1.0
}