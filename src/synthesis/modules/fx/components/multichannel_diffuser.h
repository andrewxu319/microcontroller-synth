#pragma once

#include "synthesis/modules/module.h"
#include "synthesis/modules/fx/components/delay_line.h"
#include "synthesis/modules/mixer.h"
#include "utils/accelerator.h"
#include "utils/rng.h"

#include <array>
#include <memory>
#include <algorithm>


// for now, each channel is its own in_buf. if i end up doing stereo later i might make Module a class template that store multichannel buffers

// TESTING NOTE: one diffuser, if mixed, doesn't do much (sounds like a delay line)

namespace synthesis {
	class MultichannelDiffuser : public MultichannelModule {
	public:
		MultichannelDiffuser(size_t capacity_ = 50, uint8_t num_channels_ = 8); // VALUE TBD
		void init() override;
		void generate_buf() override;
		void resize(size_t capacity);
		void set_num_channels(uint8_t value);
		void set_delay(double value_ms);

		enum BufType {
			AUDIO
		};

	private:
		vector<const float_s*> in_bufs[1];
		size_t capacity;
		uint8_t num_channels;
		float_s sqrt_num_channels;
		double max_delay; // in ms

		WetOnlyDelayLine delay_line;
		vector<int8_t> flip_polarities;

		void fast_hadamard_transform(vector<MultichannelModule::Buffer>& data);
	};
}