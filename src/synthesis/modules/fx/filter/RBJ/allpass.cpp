#include "allpass.h"

#include <cmath>

void synthesis::RBJFilter::Allpass::compute_coefficients() {
	float_s a0 = 1 + alpha;
	b0 = (1.0f - alpha) / a0;
	b1 = (-2.0f * cos_omega) / a0;
	b2 = 1.0f;
	a1 = b1;
	a2 = b0;
}