#include "flanger.h"

#include "utils/accelerator.h"

using namespace synthesis;

Flanger::Flanger()
	: Fx(mods, sizeof(mods) / sizeof(vector<Module*>)),
	memory_buffer{ 5 * config::buffer_size },
	offset{}
{
	;
}

void Flanger::generate_buf() {
	if (audio_in_buf->data[0] == EMPTY_BUF_MARKER) {
		memory_buffer.reset();
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	silent_in_buffers_elapsed = 0;
	memcpy(out_buf, audio_in_buf->data, config::buffer_size * sizeof(float_s));

	if (wet <= 0.0) return;

	memory_buffer.pop_start_with_pointer(config::buffer_size); // advance the start pointer
	memory_buffer.push_back(out_buf, config::buffer_size); // now most recent sample is at index size - 1

	if (mods[Mods::OFFSET].empty()) {
		const auto subarray_ptrs{ memory_buffer.get_subarray_ptrs(memory_buffer.size - config::buffer_size - offset, config::buffer_size) };
		accelerator::vec_add_float_s(get<0>(subarray_ptrs), out_buf, out_buf, get<1>(subarray_ptrs));
		accelerator::vec_add_float_s(get<2>(subarray_ptrs), out_buf + get<1>(subarray_ptrs), out_buf + get<1>(subarray_ptrs), get<3>(subarray_ptrs));
	}
	else {
		float_s* effective_offset{ in_bufs[mods[Mods::OFFSET][0]->id].data };
		for (size_t i{ 1 }; i < mods[Mods::OFFSET].size(); i++) {
			accelerator::vec_add_float_s(in_bufs[mods[Mods::OFFSET][i]->id].data, effective_offset, effective_offset, config::buffer_size);
		}
		accelerator::vec_scal_add_float_s(effective_offset, effective_offset, offset, config::buffer_size);
		//for (size_t i{ 0 }; i < config::buffer_size; i++) {
		//	out_buf[i] += memory_buffer.get(memory_buffer.size - config::buffer_size + i - effective_offset[i]);
		//}
		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			const float_s delayed_signal_low{ memory_buffer.get(memory_buffer.size - config::buffer_size + i - static_cast<size_t>(effective_offset[i])) };
			const float_s delayed_signal_high{ memory_buffer.get(memory_buffer.size - config::buffer_size + i - static_cast<size_t>(effective_offset[i]) - 1) };
			const float_s decimal_part{ effective_offset[i] - static_cast<size_t>(effective_offset[i]) };
			const float_s delayed_signal_interpolated{ delayed_signal_low * (1.0f - decimal_part) + delayed_signal_high * decimal_part };
			out_buf[i] += delayed_signal_interpolated;
		}
	}

	mix_dry_wet(Mods::WET);
}

void Flanger::set_offset(const double value) { // in ms
	offset = value * 0.001 * config::sample_rate;
}