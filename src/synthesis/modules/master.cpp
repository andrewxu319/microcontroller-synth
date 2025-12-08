#include "master.h"
#include "utils/utils.h"
#include "utils/accelerator.h"

#include <algorithm>

using namespace synthesis;

Master::Master()
	: Mixer(NO_BASE_INIT),
	out_buf{},
	modules{},
	note_messages{},
	cc_messages{}
{
}

Master& Master::instance() {
	static Master master{};
	return master;
}

void Master::generate_buf() {
	// check for messages here
	while (!note_messages.empty()) {
		midi::NoteMessage& note_message{ note_messages.front() };
		note_messages.pop();
		printf("Note message: function %d, channel %d, note %d, velocity %d\n", static_cast<int>(note_message.function), note_message.channel, note_message.note, note_message.velocity);
	}
	while (!cc_messages.empty()) {
		midi::CcMessage& cc_message{ cc_messages.front() };
		cc_messages.pop();
		printf("CC message: function %d, channel %d, value %d\n", cc_message.function, cc_message.channel, cc_message.value);
	}

	(*modules)[2]->generate_buf();
	(*modules)[3]->generate_buf();
	(*modules)[6]->generate_buf();
	(*modules)[7]->generate_buf();
	(*modules)[1]->generate_buf();
	(*modules)[5]->generate_buf();

	fill(out_buf, out_buf + config::buffer_size, 0.0f);
	for (const auto& in_buf : in_bufs) {
		accelerator::vec_add_float32_t(in_buf.second.data, out_buf, out_buf, config::buffer_size);
		// clip between -1.0 and 1.0
	}
}

