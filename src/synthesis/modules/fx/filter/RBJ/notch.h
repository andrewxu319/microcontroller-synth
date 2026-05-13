#pragma once

#include "bandwidth_filter.h"

namespace synthesis::RBJFilter {
	class Notch : public BandwidthFilter<Notch> {
	public:
		void compute_coefficients();
	};
}