#pragma once

#include "resonance_filter.h"

namespace synthesis::RBJFilter {
	class Lowpass : public ResonanceFilter<Lowpass> {
	public:
		void compute_coefficients();
	};
}