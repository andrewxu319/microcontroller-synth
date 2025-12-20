#include "delay.h"

#include "utils/accelerator.h"

#include <algorithm>
#include <cmath>

using namespace synthesis;

Delay::Delay()
	: Fx(),
	feedback{},
	delay_time{},
	delay_frames{ static_cast<size_t>(round(delay_time * config::sample_rate / config::buffer_size)) * config::buffer_size }, // round to the nearest buffer_size
	delay_buffer{ delay_frames },
	half_life{},
	silent_in_buffers_elapsed{},
	silence_threshold{}
{
}

void Delay::generate_buf() {
	if (in_buf->data[0] == EMPTY_BUF_MARKER) {
		memset(out_buf, 0.0f, config::buffer_size * sizeof(float_s));
		if (silent_in_buffers_elapsed > silence_threshold) return;
		if (silent_in_buffers_elapsed == silence_threshold) {
			delay_buffer.reset();
			silent_in_buffers_elapsed++;
			return;
		}
		silent_in_buffers_elapsed++;
	}
	else {
		silent_in_buffers_elapsed = 0;
		memcpy(out_buf, in_buf->data, config::buffer_size * sizeof(float_s));
	}
	if (wet <= 0.0) return;

	const float_s* read_start_ptr{ delay_buffer.pop_start_with_pointer(config::buffer_size) };
	accelerator::vec_mult_add_float_s(read_start_ptr, out_buf, out_buf, feedback, config::buffer_size);
	//if (get<3>(delay_buffer_segments) > 0) {
	//	accelerator::vec_mult_add_float_s(get<2>(delay_buffer_segments), out_buf, out_buf, feedback, get<3>(delay_buffer_segments));
	//}
	delay_buffer.push_back(out_buf, config::buffer_size);

	Fx::generate_buf();
}

void Delay::set_delay_time(double value) {
	delay_time = value;
	delay_frames = static_cast<int>(round(delay_time * config::sample_rate / config::buffer_size)) * config::buffer_size; // round to the nearest buffer_size
	delay_buffer.resize(delay_frames);
	half_life = delay_time * log(2.0) / log(1.0 / feedback);
	silence_threshold = half_life * 14 * config::sample_rate / config::buffer_size;
}

void Delay::set_feedback(float_s value) {
	feedback = value;
	half_life = delay_time * log(2.0) / log(1.0 / feedback);
	silence_threshold = half_life * 14 * config::sample_rate / config::buffer_size;
}