#pragma once

#include "resonance_filter.h"

namespace synthesis::RBJFilter {
	class Allpass : public ResonanceFilter<Allpass> {
	public:
		void compute_coefficients();
	};
}