#pragma once

#include "synthesis/modules/fx/fx.h"
#include "synthesis/modules/fx/components/delay_line.h"
#include "synthesis/modules/mixer.h"

namespace synthesis {
	class SchroederAllpass : public Fx {
	public:
		SchroederAllpass();
		void add_buf(const float_s* __restrict buf, uint8_t buf_type);
		void generate_buf() override;

		void set_feedback(const float_s value);
		void set_delay(const double value_ms);

		enum BufTypes {
			AUDIO,
			WET
		};

	private:
		vector<const float_s*> in_bufs[2];
		WetOnlyDelayLine delay_line;
		Mixer adder;
		float_s feedback;
		size_t delay;
	};
}