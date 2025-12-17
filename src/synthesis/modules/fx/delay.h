#pragma once

#include "synthesis/modules/fx/fx.h"
#include "utils/includes.h"
#include "utils/circular_array.h"

namespace synthesis {
	class Delay : public Fx {
	public:
		Delay();
		void generate_buf();
		void set_delay_time(double value);
		void set_feedback(float_s value);
		float_s feedback;

	private:
		double delay_time;
		size_t delay_frames;
		utils::CircularArray<float_s> delay_buffer;
		double half_life;
		size_t silent_in_buffers_elapsed;
		size_t silence_threshold;
	};
}