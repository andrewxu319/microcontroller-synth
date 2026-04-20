#pragma once

#include "utils/global.h"
#include "utils/config.h"
#include "synthesis/modules/fx/fx.h"
#include "utils/circular_array.h"

#include <vector>

namespace synthesis {
	class DelayLine : public MultichannelModule {
	public:
		DelayLine(size_t capacity = 0, uint8_t num_channels_ = 1); // in multiples of buffer_size
		void generate_buf() override;
		void set_delay(const double value_ms, uint8_t channel = 255);
		void set_feedback(const float_s value, uint8_t channel = 255);
		void resize(size_t capacity); // in multiples of buffer_size

		enum BufType {
			AUDIO,
			DELAY,
			FEEDBACK
		};

	protected:
		std::vector<const float_s*> in_bufs[3];

		struct DelayLineChannel {
			utils::CircularArray<float_s> memory_buffer;
			size_t delay;
			float_s feedback;
			float_s feedback_memory;
		};
		std::vector<DelayLineChannel> channels;
		uint8_t num_channels; // should this stuff be a template class instead
	};

	class WetOnlyDelayLine : public DelayLine {
	public:
		WetOnlyDelayLine(size_t capacity = 0, uint8_t num_channels_ = 1);
		void generate_buf() override;
	};
}