// biquad, fir, lader, state variable

#pragma once

#undef _MSC_VER // otherwise dspfilters has tr1 issues

#include "utils/global.h"
#include "utils/config.h"
#include "utils/math.h"
#include "synthesis/modules/fx/fx.h"

#include "dspfilters/Dsp.h"



namespace synthesis {
	class Filter : public Fx {
	public:
		Filter();
		void set_cutoff(double value);

	protected:
		void set_qbs(double value);
		
		std::vector<const float_s*> in_bufs[5];
		float_s effective_omega;
		float_s cutoff;
		float_s qbs; // resonance/bandwidth/slope. resonance is centered at 1.0. bandwidth is in octaves
		float_s effective_qbs;
	};
}

