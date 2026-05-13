#pragma once

#include "utils/global.h"
#include "utils/config.h"
#include "synthesis/modules/fx/fx.h"
#include "synthesis/modules/fx/components/phaser_allpass.h"

namespace synthesis {
	class Phaser : public Fx {
	public:
		Phaser();
		void generate_buf() override;
		void add_buf(const float_s* buf, uint8_t buf_type) override;
		void set_stages(const uint8_t value);
		void set_center_freq(const uint16_t value);
		void set_feedback(const float_s value);

		enum BufType {
			AUDIO,
			WET,
			CENTER_FREQ,
			FEEDBACK
		};

	protected:
		std::vector<const float_s*> in_bufs[4];

		std::vector<std::unique_ptr<PhaserAllpass>> allpass_filters; // MONO FOR NOW
		uint8_t stages;
		uint16_t center;
		float_s feedback;
		float_s feedback_memory;
	};
}