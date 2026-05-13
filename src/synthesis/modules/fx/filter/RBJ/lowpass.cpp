#include "lowpass.h"

#include <cmath>

using namespace synthesis::RBJFilter;

void Lowpass::compute_coefficients() {
	float_s a0 = 1.0f + alpha;
	b1 = (1.0f - cos_omega) / a0;
	b0 = b1 / 2.0f;
	b2 = b0;
	a1 = (-2.0f * cos_omega) / a0;
	a2 = (1.0f - alpha) / a0;
}