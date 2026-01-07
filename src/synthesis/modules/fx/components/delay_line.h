#pragma once

#include "utils/includes.h"
#include "utils/config.h"
#include "synthesis/modules/fx/fx.h"
#include "utils/circular_array.h"

namespace synthesis {
	class DelayLine : public Fx {
	public:
		DelayLine(size_t capacity = 0); // in multiples of buffer_size
		void generate_buf() override;
		void set_delay(const double value_ms);
		void set_feedback(const float_s value);
		void resize(size_t capacity); // in multiples of buffer_size

		enum BufTypes {
			AUDIO,
			WET,
			DELAY,
			FEEDBACK
		};

	protected:
		vector<const float_s*> in_bufs[4];
		utils::CircularArray<float_s> memory_buffer;
		size_t delay;
		float_s feedback;
		float_s feedback_memory;
	};

	class WetOnlyDelayLine : public DelayLine {
	public:
		WetOnlyDelayLine(size_t capacity = 0);
		void generate_buf() override;
	};
}