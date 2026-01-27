#pragma once

#undef _MSC_VER // otherwise dspfilters has tr1 issues

#include "utils/global.h"
#include "utils/config.h"
#include "synthesis/modules/fx/fx.h"

#include "dspfilters/Dsp.h"

namespace synthesis {
	class Phaser : public Fx {
	public:
		Phaser();
		void generate_buf() override;
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

		std::vector<Dsp::FilterDesign<Dsp::RBJ::Design::AllPass, 1>> all_pass_filters; // MONO FOR NOW
		Dsp::Params params;
		uint8_t stages;
		uint16_t center;
		float_s feedback;
		float_s feedback_memory;
	};
}