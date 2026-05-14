
#pragma once

#include "shelf_filter.h"

namespace synthesis::RBJFilter {
	class LowShelf : public ShelfFilter<LowShelf> {
	public:
		void compute_coefficients();
	};
}