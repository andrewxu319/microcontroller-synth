#include "delay_line.h"

#include "utils/math.h"



using namespace synthesis;

DelayLine::DelayLine(size_t capacity, uint8_t num_channels_)
	: MultichannelModule(in_bufs, num_channels_),
	num_channels{ num_channels_ },
	channels{ num_channels_, DelayLineChannel{} }
{
	for (DelayLineChannel& channel : channels) {
		channel.memory_buffer.resize(capacity * config::buffer_size);
	}
}

void DelayLine::generate_buf() {
	for (uint8_t i{ 0 }; i < num_channels; i++) {
		float_s* channel_out_buf{ out_bufs[i].data() };
		utils::CircularArray<float_s>& memory_buffer{ channels[i].memory_buffer };
		size_t& delay{ channels[i].delay };
		float_s& feedback{ channels[i].feedback };
		float_s& feedback_memory{ channels[i].feedback_memory };

		if (in_bufs[BufType::AUDIO][i][0] == EMPTY_BUF_MARKER) {
			memset(channel_out_buf, 0.0f, config::buffer_size * sizeof(float_s));
		}
		else {
			memcpy(channel_out_buf, in_bufs[BufType::AUDIO][i], config::buffer_size * sizeof(float_s));
		}

		float_s delay_buf_sum[config::buffer_size];
		const bool delay_mods{ sum_bufs(BufType::DELAY, delay_buf_sum, static_cast<float_s>(delay)) };
		float_s feedback_buf_sum[config::buffer_size];
		const bool feedback_mods{ sum_bufs(BufType::FEEDBACK, feedback_buf_sum, feedback) };

		if (delay_mods) {
			for (size_t j{ 0 }; j < config::buffer_size; j++) {
				if (feedback_mods) {
					feedback = feedback_buf_sum[j];
				}

				memory_buffer.pop_start_with_pointer(1); // advance the start pointer
				memory_buffer.push_back(channel_out_buf[j] + feedback_memory * feedback);

				const size_t effective_delay_floored{ static_cast<size_t>(delay_buf_sum[j]) };
				const float_s delayed_signal_next{ memory_buffer.get(memory_buffer.size - effective_delay_floored - 1) };
				const float_s delayed_signal_prev{ memory_buffer.get(memory_buffer.size - effective_delay_floored - 2) };
				const float_s decimal_part{ delay_buf_sum[j] - effective_delay_floored }; // + delay because truncation behaves differently for pos vs neg
				const float_s delayed_signal_interpolated{ delayed_signal_next * (1.0f - decimal_part) + delayed_signal_prev * decimal_part };
				channel_out_buf[j] += delayed_signal_interpolated;

				feedback_memory = channel_out_buf[j]; // changed 3/27 from [i] to [j]
			}
		}
		else {
			for (size_t j{ 0 }; j < config::buffer_size; j++) {
				if (feedback_mods) {
					feedback = feedback_buf_sum[j];
				}

				memory_buffer.pop_start_with_pointer(1); // advance the start pointer
				memory_buffer.push_back(channel_out_buf[j] + feedback_memory * feedback);

				channel_out_buf[j] += memory_buffer.get(memory_buffer.size - delay - 1);
				feedback_memory = channel_out_buf[j];
			}
		}
	}
}

void DelayLine::set_delay(const double value_ms, uint8_t channel) {
	const size_t value_samples{ static_cast<size_t>(value_ms * 0.001 * config::sample_rate) };
	if (value_samples >= channels[0].memory_buffer.size) {
		for (DelayLineChannel& channel : channels) {
			channel.memory_buffer.resize((value_samples / config::buffer_size) * 3 / 2 * config::buffer_size); // roughly 1.5x the requested capacity
		}
		return;
	}

	if (channel != 255) {
		channels[channel].delay = value_samples;
	}
	else {
		for (DelayLineChannel& ch : channels) {
			ch.delay = value_ms;
		}
	}
}

void DelayLine::set_feedback(const float_s value, uint8_t channel) {
	assert(value > -1.0 && value < 1.0);

	if (channel != 255) {
		channels[channel].feedback = value;
	}
	else {
		for (DelayLineChannel& ch : channels) {
			ch.feedback = value;
		}
	}
}

void DelayLine::resize(size_t capacity) {
	for (DelayLineChannel& channel : channels) {
		channel.memory_buffer.resize(capacity * config::buffer_size);
	}
}

WetOnlyDelayLine::WetOnlyDelayLine(size_t capacity, uint8_t num_channels_) : DelayLine(capacity, num_channels_) {};

void WetOnlyDelayLine::generate_buf() {
	for (uint8_t i{ 0 }; i < num_channels; i++) {
		float_s* channel_out_buf{ out_bufs[i].data() };
		utils::CircularArray<float_s>& memory_buffer{ channels[i].memory_buffer };
		size_t& delay{ channels[i].delay };
		float_s& feedback{ channels[i].feedback };
		float_s& feedback_memory{ channels[i].feedback_memory };

		if (in_bufs[BufType::AUDIO][i][0] == EMPTY_BUF_MARKER) {
			memset(channel_out_buf, 0.0f, config::buffer_size * sizeof(float_s));
		}
		else {
			memcpy(channel_out_buf, in_bufs[BufType::AUDIO][i], config::buffer_size * sizeof(float_s));
		}

		float_s delay_buf_sum[config::buffer_size]; // in samples
		const bool delay_mods{ sum_bufs(BufType::DELAY, delay_buf_sum, static_cast<float_s>(delay)) };
		float_s feedback_buf_sum[config::buffer_size];
		const bool feedback_mods{ sum_bufs(BufType::FEEDBACK, feedback_buf_sum, feedback) };

		if (delay_mods) {
			for (size_t j{ 0 }; j < config::buffer_size; j++) {
				if (feedback_mods) {
					feedback = feedback_buf_sum[j];
				}

				memory_buffer.pop_start_with_pointer(1); // advance the start pointer
				memory_buffer.push_back(in_bufs[BufType::AUDIO][i][j] + feedback_memory * feedback);

				const size_t effective_delay_floored{ static_cast<size_t>(delay_buf_sum[j]) };
				const float_s delayed_signal_next{ memory_buffer.get(memory_buffer.size - effective_delay_floored - 1) };
				const float_s delayed_signal_prev{ memory_buffer.get(memory_buffer.size - effective_delay_floored - 2) };
				const float_s decimal_part{ delay_buf_sum[j] - effective_delay_floored }; // + delay because truncation behaves differently for pos vs neg
				const float_s delayed_signal_interpolated{ delayed_signal_next * (1.0f - decimal_part) + delayed_signal_prev * decimal_part };
				channel_out_buf[j] += delayed_signal_interpolated;

				feedback_memory = channel_out_buf[j];
			}
		}
		else {
			const size_t delayed_sample_index{ memory_buffer.size - delay - 1 };
			for (size_t j{ 0 }; j < config::buffer_size; j++) {
				if (feedback_mods) {
					feedback = feedback_buf_sum[j];
				}

				memory_buffer.pop_start_with_pointer(1); // advance the start pointer
				memory_buffer.push_back(in_bufs[BufType::AUDIO][i][j] + feedback_memory * feedback);

				channel_out_buf[j] = memory_buffer.get(delayed_sample_index);
				feedback_memory = channel_out_buf[j];
			}
		}
	}
}