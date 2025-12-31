#include "flanger.h"

#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

Flanger::Flanger()
	: Fx(in_bufs),
	memory_buffer{ 5 * config::buffer_size },
	offset{},
	feedback{},
	feedback_memory{}
{
	;
}

void Flanger::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		memory_buffer.reset();
		out_buf[0] = EMPTY_BUF_MARKER;
		feedback_memory = 0.0;
		return;
	}

	memcpy(out_buf, audio_in_buf, config::buffer_size * sizeof(float_s));

	if (wet <= 0.0) return;

	float_s offset_buf_sum[config::buffer_size];
	const bool offset_mods{ sum_bufs(BufTypes::OFFSET, offset_buf_sum, offset) };
	float_s feedback_buf_sum[config::buffer_size];
	const bool feedback_mods{ sum_bufs(BufTypes::FEEDBACK, feedback_buf_sum, feedback) };

	if (offset_mods) {
		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			if (feedback_mods) {
				feedback = feedback_buf_sum[i];
			}
			out_buf[i] += feedback_memory * feedback;

			const size_t effective_offset_floored{ static_cast<size_t>(offset_buf_sum[i]) };
			const float_s delayed_signal_low{ memory_buffer.get(memory_buffer.size + i - effective_offset_floored) };
			const float_s delayed_signal_high{ memory_buffer.get(memory_buffer.size + i - effective_offset_floored - 1) };
			const float_s decimal_part{ (offset_buf_sum[i]) - effective_offset_floored }; // + offset because truncation behaves differently for pos vs neg
			const float_s delayed_signal_interpolated{ delayed_signal_low * (1.0f - decimal_part) + delayed_signal_high * decimal_part };
			out_buf[i] += delayed_signal_interpolated;

			feedback_memory = out_buf[i];
		}
	}
	else {
		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			if (feedback_mods) {
				feedback = feedback_buf_sum[i];
			}
			out_buf[i] += feedback_memory * feedback;

			memory_buffer.get(memory_buffer.size + i - offset);
			feedback_memory = out_buf[i];
		}
	}

	memory_buffer.pop_start_with_pointer(config::buffer_size); // advance the start pointer
	memory_buffer.push_back(out_buf, config::buffer_size);

	mix_dry_wet();
}

void Flanger::set_offset(const double value) { // in ms
	offset = value * 0.001 * config::sample_rate;
}

void Flanger::set_feedback(const float_s value) {
	assert(value > -1.0 && value < 1.0);
	feedback = value;
}