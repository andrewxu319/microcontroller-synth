/*
difference between this implementation and delay_line:
delay is designed for fast processing at CONTROL RATE. delay time must be a multiple of buffer_size
long delay times
no feedback
cannot modulate delay time

might end up just using delay_line for this instead
*/

#include "delay.h"

#include "utils/math.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace synthesis;

Delay::Delay()
	: Fx(in_bufs),
	feedback{},
	delay_time{},
	delay_frames{}, // round to the nearest buffer_size
	delay_buffer{ 0 }, // delay_frames // 100000 / config::buffer_size * config::buffer_size
	half_life{},
	silent_in_buffers_elapsed{},
	silence_threshold{}
{
}

void Delay::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		memset(out_buf, 0, config::buffer_size * sizeof(float_s));
		if (silent_in_buffers_elapsed > silence_threshold) return; // need to adjust this stuff for when delay time is modulated
		if (silent_in_buffers_elapsed == silence_threshold) {
			delay_buffer.reset();
			silent_in_buffers_elapsed++;
			return;
		}
		silent_in_buffers_elapsed++;
	}
	else {
		silent_in_buffers_elapsed = 0;
		memcpy(out_buf, audio_in_buf, config::buffer_size * sizeof(float_s));
	}
	if (wet <= 0.0) return;

	float_s feedback_buf_sum[config::buffer_size];
	bool feedback_mods{ sum_bufs(BufType::FEEDBACK, feedback_buf_sum) };
	//const float_s* delay_time_mod_sum{ sum_mods(BufType::DELAY_TIME) }; // DO LATER

	if (feedback_mods) {
		for (size_t i{ 0 }; i < config::buffer_size; i += config::control_rate) {
			//if (delay_time_mod_sum) { // DO LATER
			//	set_delay(delay_time_mod_sum[i] + delay_time);
			//	//delay_buffer.fast_resize(static_cast<size_t>(round((delay_time_mod_sum[i] + delay_time) * config::sample_rate / config::buffer_size)) * config::buffer_size);
			//}
			const float_s* read_start_ptr{ delay_buffer.pop_start_with_pointer(config::control_rate) };
			math::vec_mult_add_float_s(read_start_ptr, out_buf + i, out_buf + i, feedback_buf_sum[i] + feedback, config::control_rate);
		}
	}
	else {
		//if (delay_time_mod_sum) { // DO LATER. also switch back to control rate
		//	set_delay(delay_time_mod_sum[i] + delay_time);
		//	//delay_buffer.fast_resize(static_cast<size_t>(round((delay_time_mod_sum[i] + delay_time) * config::sample_rate / config::buffer_size)) * config::buffer_size);
		//}
		const float_s* read_start_ptr{ delay_buffer.pop_start_with_pointer(config::buffer_size) };
		math::vec_mult_add_float_s(read_start_ptr, out_buf, out_buf, feedback, config::buffer_size);
	}

	delay_buffer.push_back(out_buf, config::buffer_size);

	mix_dry_wet();
}

void Delay::set_delay(double value) {
	delay_time = value;
	delay_frames = static_cast<size_t>(round(delay_time * config::sample_rate / config::buffer_size)) * config::buffer_size; // round to the nearest buffer_size
	delay_buffer.resize(delay_frames);
	half_life = delay_time * log(2.0) / log(1.0 / feedback);
	silence_threshold = half_life * 14 * config::sample_rate / config::buffer_size;
}

void Delay::set_feedback(float_s value) {
	feedback = value;
	if (value >= 1.0) {
		half_life = std::numeric_limits<double>::max();
	}
	half_life = delay_time * log(2.0) / log(1.0 / feedback);
	silence_threshold = static_cast<size_t>(half_life + 1) * 14 * config::sample_rate / config::buffer_size;
}