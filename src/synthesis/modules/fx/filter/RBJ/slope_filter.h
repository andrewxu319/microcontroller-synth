#pragma once

#include "RBJ.h"

namespace synthesis::RBJFilter {
    template <typename Derived>
	class SlopeFilter : public RBJ<SlopeFilter> {
	public:
		void compute_alpha();
        void compute_coefficients();
	};
}

using namespace synthesis::RBJFilter;

template<typename Derived>
void SlopeFilter<Derived>::compute_alpha() {
    // alpha = sin_omega / 2.0 * std::sqrt(())
}

template<typename Derived>
void SlopeFilter<Derived>::compute_coefficients() {
    static_cast<Derived*>(this)->compute_coefficients();
}