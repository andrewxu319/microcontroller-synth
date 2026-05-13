#pragma once

#include "bandwidth_filter.h"

namespace synthesis::RBJFilter {
	class Bandpass : public BandwidthFilter<Bandpass> {
	public:
		void compute_coefficients();
	};
}