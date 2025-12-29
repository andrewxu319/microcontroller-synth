#include "flanger.h"

#include "utils/accelerator.h"

using namespace synthesis;

Flanger::Flanger()
	: Fx(in_bufs),
	memory_buffer{ 5 * config::buffer_size },
	offset{}
{
	;
}

void Flanger::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		memory_buffer.reset();
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	memcpy(out_buf, audio_in_buf, config::buffer_size * sizeof(float_s));

	if (wet <= 0.0) return;

	memory_buffer.pop_start_with_pointer(config::buffer_size); // advance the start pointer
	memory_buffer.push_back(out_buf, config::buffer_size); // now most recent sample is at index size - 1

	if (in_bufs[BufTypes::OFFSET].empty()) {
		const auto subarray_ptrs{ memory_buffer.get_subarray_ptrs(memory_buffer.size - config::buffer_size - offset, config::buffer_size) };
		accelerator::vec_add_float_s(get<0>(subarray_ptrs), out_buf, out_buf, get<1>(subarray_ptrs));
		accelerator::vec_add_float_s(get<2>(subarray_ptrs), out_buf + get<1>(subarray_ptrs), out_buf + get<1>(subarray_ptrs), get<3>(subarray_ptrs));
	}
	else {
		float_s offset_buf_sum[config::buffer_size];
		if (sum_bufs(BufTypes::OFFSET, offset_buf_sum)) {
			for (size_t i{ 0 }; i < config::buffer_size; i++) {
				const float_s delayed_signal_low{ memory_buffer.get(memory_buffer.size - config::buffer_size + i - static_cast<size_t>(offset_buf_sum[i]) + offset) };
				const float_s delayed_signal_high{ memory_buffer.get(memory_buffer.size - config::buffer_size + i - static_cast<size_t>(offset_buf_sum[i] + offset) - 1) };
				const float_s decimal_part{ (offset_buf_sum[i] + offset) - static_cast<size_t>(offset_buf_sum[i] + offset) }; // + offset because truncation behaves differently for pos vs neg
				const float_s delayed_signal_interpolated{ delayed_signal_low * (1.0f - decimal_part) + delayed_signal_high * decimal_part };
				out_buf[i] += delayed_signal_interpolated;
			}
		}
		else {
			for (size_t i{ 0 }; i < config::buffer_size; i++) {
				memory_buffer.get(memory_buffer.size - config::buffer_size + i - offset);
			}
		}
	}

	mix_dry_wet();
}

void Flanger::set_offset(const double value) { // in ms
	offset = value * 0.001 * config::sample_rate;
}