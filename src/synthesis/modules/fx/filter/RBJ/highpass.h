#pragma once

#include "resonance_filter.h"

namespace synthesis::RBJFilter {
	class Highpass : public ResonanceFilter<Highpass> {
	public:
		void compute_coefficients();
	};
}