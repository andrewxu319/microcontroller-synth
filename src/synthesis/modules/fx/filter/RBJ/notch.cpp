#include "notch.h"

using namespace synthesis::RBJFilter;

void Notch::compute_coefficients() {
	float_s a0 = 1 + alpha;
	b0 = 1.0f / a0;
	b1 = (-2.0f * cos_omega) / a0;
	b2 = b0;
	a1 = b1;
	a2 = (1 - alpha) / a0;
}