#include "high_shelf.h"

#include <cmath>

using namespace synthesis::RBJFilter;

void HighShelf::compute_coefficients() {
    float_s c0{ (A + 1) - (A - 1) * cos_omega };
    float_s c1{ (A - 1) - (A + 1) * cos_omega };
    float_s c2{ 2 * std::sqrt(A) * alpha };

	float_s a0 = c0 + c2;
    a1 = 2.0f * c1 / a0;
    a2 = (c0 - c2) / a0;
	b0 = A;
	b1 = -a1 * A;
	b2 = a2 * A;
}