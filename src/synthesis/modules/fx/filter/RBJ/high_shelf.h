
#pragma once

#include "shelf_filter.h"

namespace synthesis::RBJFilter {
	class HighShelf : public ShelfFilter<HighShelf> {
	public:
		void compute_coefficients();
	};
}