#pragma once

#include "synthesis/modules/fx/fx.h"
#include "utils/includes.h"

namespace synthesis {
	class Reverb : public Fx {
	public:
		Reverb();
		//virtual void generate_buf();

		enum BufTypes {
			AUDIO,
			WET,
			DECAY
		};

	protected:
		vector<const float_s*> in_bufs[3];
		double decay;
		size_t decay_samples;
	};
}