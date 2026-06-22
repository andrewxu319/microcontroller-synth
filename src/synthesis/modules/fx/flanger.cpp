// #include "flanger.h"

// #include "utils/math.h"

// using namespace synthesis;

// Flanger::Flanger()
// 	: delay_line(5, 1) {
// 	in_bufs_ptr = delay_line.in_bufs_ptr;
// }

// void Flanger::generate_buf() {
// 	if (in_bufs_ptr[DelayLine::BufType::AUDIO][0][0] == EMPTY_BUF_MARKER) {
// 		delay_line.channels[0].memory_buffer.reset();
// 		out_bufs[0].data()[0] = EMPTY_BUF_MARKER;
// 		channels[0].feedback_memory = 0.0;
//	    Module::generate_buf();
// 		return;
// 	}

// 	delay_line.generate_buf();
// }

// int Flanger::add_output(Module* __restrict output, uint8_t buf_type) {
// 	return Fx::add_output(output, buf_type);
// }