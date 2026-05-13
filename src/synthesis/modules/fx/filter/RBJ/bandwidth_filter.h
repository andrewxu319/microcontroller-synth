#pragma once

#include "RBJ.h"

namespace synthesis::RBJFilter {
    template <typename Derived>
	class BandwidthFilter : public RBJ<BandwidthFilter<Derived>> {
	public:
        BandwidthFilter();
		void set_bandwidth(double value);
		void compute_alpha();
        void compute_coefficients();
	};
}

using namespace synthesis::RBJFilter;

template<typename Derived>
BandwidthFilter<Derived>::BandwidthFilter() {
    Filter::set_qbs(500.0f);
}

template<typename Derived>
void BandwidthFilter<Derived>::set_bandwidth(double value) {
    this->set_qbs(value);
}

template<typename Derived>
void BandwidthFilter<Derived>::compute_alpha() {
    this->alpha = this->sin_omega * std::sinh(std::log2(2.0) / 2.0 * this->effective_qbs * this->effective_omega / this->sin_omega);
}

template<typename Derived>
void BandwidthFilter<Derived>::compute_coefficients() {
    static_cast<Derived*>(this)->compute_coefficients();
}