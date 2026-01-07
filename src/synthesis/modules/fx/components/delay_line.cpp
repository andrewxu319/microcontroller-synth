#include "delay_line.h"

#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

DelayLine::DelayLine(size_t capacity)
	: Fx(in_bufs),
	memory_buffer{ capacity * config::buffer_size },
	delay{},
	feedback{},
	feedback_memory{}
{
	;
}

void DelayLine::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		memset(out_buf, 0.0f, config::buffer_size * sizeof(float_s));
	}
	else {
		memcpy(out_buf, audio_in_buf, config::buffer_size * sizeof(float_s));
	}

	if (wet <= 0.0) return;

	float_s delay_buf_sum[config::buffer_size];
	const bool delay_mods{ sum_bufs(BufTypes::DELAY, delay_buf_sum, delay) };
	float_s feedback_buf_sum[config::buffer_size];
	const bool feedback_mods{ sum_bufs(BufTypes::FEEDBACK, feedback_buf_sum, feedback) };

	if (delay_mods) {
		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			if (feedback_mods) {
				feedback = feedback_buf_sum[i];
			}

			memory_buffer.pop_start_with_pointer(1); // advance the start pointer
			memory_buffer.push_back(out_buf[i] + feedback_memory * feedback);

			const size_t effective_delay_floored{ static_cast<size_t>(delay_buf_sum[i]) };
			const float_s delayed_signal_next{ memory_buffer.get(memory_buffer.size - effective_delay_floored - 1) };
			const float_s delayed_signal_prev{ memory_buffer.get(memory_buffer.size - effective_delay_floored - 2) };
			const float_s decimal_part{ delay_buf_sum[i] - effective_delay_floored }; // + delay because truncation behaves differently for pos vs neg
			const float_s delayed_signal_interpolated{ delayed_signal_next * (1.0f - decimal_part) + delayed_signal_prev * decimal_part };
			out_buf[i] += delayed_signal_interpolated;

			feedback_memory = out_buf[i];
		}
	}
	else {
		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			if (feedback_mods) {
				feedback = feedback_buf_sum[i];
			}

			memory_buffer.pop_start_with_pointer(1); // advance the start pointer
			memory_buffer.push_back(out_buf[i] + feedback_memory * feedback);

			out_buf[i] += memory_buffer.get(memory_buffer.size - delay - 1);
			feedback_memory = out_buf[i];
		}
	}

	mix_dry_wet();
}

void DelayLine::set_delay(const double value_ms) {
	const size_t value_samples{ static_cast<size_t>(value_ms * 0.001 * config::sample_rate) };
	if (value_samples >= memory_buffer.size) {
		printf("Delay time exceeds memory buffer capacity!\n");
		return;
	}

	delay = value_samples;
}

void DelayLine::set_feedback(const float_s value) {
	assert(value > -1.0 && value < 1.0);
	feedback = value;
}

void DelayLine::resize(size_t capacity) {
	memory_buffer.resize(capacity * config::buffer_size);
}

WetOnlyDelayLine::WetOnlyDelayLine(size_t capacity) : DelayLine(capacity) {};

void WetOnlyDelayLine::generate_buf() {
	float_s delay_buf_sum[config::buffer_size];
	const bool delay_mods{ sum_bufs(BufTypes::DELAY, delay_buf_sum, delay) };
	float_s feedback_buf_sum[config::buffer_size];
	const bool feedback_mods{ sum_bufs(BufTypes::FEEDBACK, feedback_buf_sum, feedback) };

	if (delay_mods) {
		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			if (feedback_mods) {
				feedback = feedback_buf_sum[i];
			}

			memory_buffer.pop_start_with_pointer(1); // advance the start pointer
			memory_buffer.push_back(audio_in_buf[i] + feedback_memory * feedback);

			const size_t effective_delay_floored{ static_cast<size_t>(delay_buf_sum[i]) };
			const float_s delayed_signal_next{ memory_buffer.get(memory_buffer.size - effective_delay_floored - 1) };
			const float_s delayed_signal_prev{ memory_buffer.get(memory_buffer.size - effective_delay_floored - 2) };
			const float_s decimal_part{ delay_buf_sum[i] - effective_delay_floored }; // + delay because truncation behaves differently for pos vs neg
			const float_s delayed_signal_interpolated{ delayed_signal_next * (1.0f - decimal_part) + delayed_signal_prev * decimal_part };
			out_buf[i] += delayed_signal_interpolated;

			feedback_memory = out_buf[i];
		}
	}
	else {
		const size_t delayed_sample_index{ memory_buffer.size - delay - 1 };
		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			if (feedback_mods) {
				feedback = feedback_buf_sum[i];
			}

			memory_buffer.pop_start_with_pointer(1); // advance the start pointer
			memory_buffer.push_back(audio_in_buf[i] + feedback_memory * feedback);

			out_buf[i] = memory_buffer.get(delayed_sample_index);
			feedback_memory = out_buf[i];
		}
	}

	mix_dry_wet();
}