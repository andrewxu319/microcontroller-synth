#include "delay.h"

#include "utils/accelerator.h"

#include <algorithm>

using namespace synthesis;

Delay::Delay()
	: 
	feedback{},
	delay_time{},
	delay_frames{ static_cast<size_t>(delay_time * config::sample_rate) },
	delay_buffer{ delay_frames }
{
	;
}

void Delay::generate_buf() {
	// OPTIMIZE MAX_ELEMENT!!!!!
	if (in_buf->data[0] == EMPTY_BUF_MARKER || *max_element(out_buf, &out_buf[config::buffer_size - 1]) <= 0.0001) {
		memset(out_buf, 0.0f, config::buffer_size * sizeof(float_s));
	}
	else {
		memcpy(out_buf, in_buf->data, config::buffer_size * sizeof(float_s));
	}
	if (wet <= 0.0) return;

	const tuple<float_s*, int, float_s*, int> delay_buffer_segments{ move(delay_buffer.pop_start_with_pointer(config::buffer_size)) };
	accelerator::vec_mult_add_float_s(get<0>(delay_buffer_segments), out_buf, out_buf, feedback, get<1>(delay_buffer_segments));
	if (get<3>(delay_buffer_segments) > 0) {
		accelerator::vec_mult_add_float_s(get<2>(delay_buffer_segments), out_buf, out_buf, feedback, get<3>(delay_buffer_segments));
	}
	delay_buffer.push_back(out_buf, config::buffer_size);

	Fx::generate_buf();
}

void Delay::set_delay_time(double value) {
	delay_time = value;
	delay_frames = static_cast<int>(delay_time * config::sample_rate);
	delay_buffer.resize(delay_frames);
}

void Delay::set_feedback(float_s value) {
	feedback = value;
}