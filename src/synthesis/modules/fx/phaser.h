#pragma once

#undef _MSC_VER // otherwise dspfilters has tr1 issues

#include "utils/includes.h"
#include "utils/config.h"
#include "synthesis/modules/fx/fx.h"

#include "dspfilters/Dsp.h"

namespace synthesis {
	class Phaser : public Fx {
	public:
		Phaser();
		void generate_buf() override;
		void set_stages(const uint8_t value);
		void set_center_freq(const double value);
		void set_feedback(const float_s value);

		enum Mods {
			WET,
			CENTER_FREQ,
			FEEDBACK
		};

	private:
		vector<float_s*> mods[3];
		vector<Dsp::FilterDesign<Dsp::RBJ::Design::AllPass, 1>> all_pass_filters; // MONO FOR NOW
		vector<Dsp::FilterDesign<Dsp::RBJ::Design::AllPass, 1>> feedback_filters; // MONO FOR NOW
		Dsp::Params params;
		uint8_t stages;
		uint16_t center;
		float_s feedback;
	};
}