#pragma once

#include "synthesis/modules/module.h"
#include "synthesis/modules/fx/components/delay_line.h"
#include "synthesis/modules/mixer.h"
#include "utils/accelerator.h"
#include "utils/rng.h"

#include <array>
#include <algorithm>
#include <cassert>

// for now, each channel is its own in_buf. if i end up doing stereo later i might make Module a class template that store multichannel buffers

namespace synthesis {
	template <uint8_t NumChannels>
	class MultichannelDiffuser : public Module {
	public:
		using Buffer = array<float_s, config::buffer_size>;
		using Buffers = array<Buffer, NumChannels>;

		MultichannelDiffuser(size_t capacity_ = 50);
		void init() override;
		void generate_buf() override;
		void resize(size_t capacity);
		const Buffers* get_out_bufs() const; // read-only pointer
		void set_delay_time(double value);

		enum BufTypes {
			AUDIO
		};

	private:
		vector<const float_s*> in_bufs[NumChannels];
		Buffers out_bufs;
		size_t capacity;
		double max_delay; // in ms

		array<WetOnlyDelayLine, NumChannels> delay_lines;
		array<int8_t, NumChannels> flip_polarities;

		void fast_hadamard_transform(Buffers& data);
	};
}

using namespace synthesis;

template <uint8_t NumChannels>
MultichannelDiffuser<NumChannels>::MultichannelDiffuser(size_t capacity_)
	: Module(in_bufs),
	in_bufs{}, capacity{ capacity_ }, max_delay{}, out_bufs{}, delay_lines{}, flip_polarities{ false }
{
	assert((NumChannels & (NumChannels - 1)) == 0); // check NumChannels is power of 2
	for (DelayLine& delay_line : delay_lines) {
		delay_line.resize(capacity_);
	}
}

template <uint8_t NumChannels>
void MultichannelDiffuser<NumChannels>::init() {
	shuffle(in_bufs[BufTypes::AUDIO].begin(), in_bufs[BufTypes::AUDIO].end(), utils::rng_engine);
	for (uint8_t i{ 0 }; i < NumChannels; i++) {
		delay_lines[i].resize(capacity);
		delay_lines[i].add_buf(in_bufs[BufTypes::AUDIO][i], WetOnlyDelayLine::BufTypes::AUDIO);

		if (utils::rng_uniform(0.0, 1.0) >= 0.5) { // 50% chance
			flip_polarities[i] = true;
		}
	}
}

template <uint8_t NumChannels>
void MultichannelDiffuser<NumChannels>::generate_buf() {
	for (uint8_t i{ 0 }; i < NumChannels; i++) {
		delay_lines[i].generate_buf();
		if (flip_polarities[i]) {
			accelerator::vec_scal_mult_float_s(delay_lines[i].get_out_buf(), out_bufs[i].data(), -1.0f, config::buffer_size);
		}
		else {
			memcpy(out_bufs[i].data(), delay_lines[i].get_out_buf(), config::buffer_size * sizeof(float_s));
		}
	}
	fast_hadamard_transform(out_bufs);
}

template <uint8_t NumChannels>
void MultichannelDiffuser<NumChannels>::resize(size_t capacity) {
	for (DelayLine& delay_line : delay_lines) {
		delay_line.resize(capacity);
	}
}

template <uint8_t NumChannels>
const typename MultichannelDiffuser<NumChannels>::Buffers* MultichannelDiffuser<NumChannels>::get_out_bufs() const {
	return &out_bufs;
}

template <uint8_t NumChannels>
void MultichannelDiffuser<NumChannels>::set_delay_time(double value) {
	max_delay = value;

	delay_lines[0].set_delay(value); // have one at the max
	for (uint8_t i{ 1 }; i < NumChannels; i++) {
		// split max delay window into NumChannels windows. pick random number in each channel's window
		double half_window_size{ max_delay / (NumChannels * 2 - 2) };
		delay_lines[i].set_delay(static_cast<size_t>(utils::rng_uniform((2 * i - 1) * half_window_size, (2 * i + 1) * half_window_size)));
	}
}

template <uint8_t NumChannels>
void MultichannelDiffuser<NumChannels>::fast_hadamard_transform(Buffers& data) {
	for (uint8_t h{ 1 }; h < NumChannels; h *= 2) {
		for (uint8_t i{ 0 }; i < NumChannels; i += 2 * h) {
			for (uint8_t j{ i }; j < i + h; j++) {
				float_s a[config::buffer_size];
				memcpy(a, data[j].data(), config::buffer_size * sizeof(float_s));
				accelerator::vec_add_float_s(data[j].data(), data[j + h].data(), data[j].data(), config::buffer_size);
				accelerator::vec_mult_add_float_s(a, data[j + h].data(), data[j + h].data(), -1.0f, config::buffer_size);
			}
		}
	}

	for (Buffer& buffer : data) {
		accelerator::vec_scal_mult_float_s(buffer.data(), buffer.data(), 1.0f / NumChannels, config::buffer_size);
	}
}