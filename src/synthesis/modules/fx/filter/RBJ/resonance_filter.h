#pragma once

#include "RBJ.h"

namespace synthesis::RBJFilter{
    template <typename Derived>
	class ResonanceFilter : public RBJ<ResonanceFilter<Derived>> {
	public:
        ResonanceFilter();
		void set_resonance(double value);
		void compute_alpha();
        void compute_coefficients();
	};
}

using namespace synthesis::RBJFilter;

template<typename Derived>
ResonanceFilter<Derived>::ResonanceFilter() {
    Filter::set_qbs(1.0f);
}

template<typename Derived>
void ResonanceFilter<Derived>::set_resonance(double value) {
    this->set_qbs(value);
}

template<typename Derived>
void ResonanceFilter<Derived>::compute_alpha() {
    this->alpha = this->sin_omega / (2.0f * this->effective_qbs);
}

template<typename Derived>
void ResonanceFilter<Derived>::compute_coefficients() {
    static_cast<Derived*>(this)->compute_coefficients();
}