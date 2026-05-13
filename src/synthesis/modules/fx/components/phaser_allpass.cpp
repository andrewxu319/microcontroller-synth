#include "phaser_allpass.h"

using namespace synthesis;

float_s PhaserAllpass::generate_one_sample(float_s input) {
    float_s output{};

    output = b0 * input + w1;
    w1 = b1 * input - a1 * output + w2;
    w2 = b2 * input - a2 * output;

	return output; // wet is always 100
}

void PhaserAllpass::set_cutoff(double value) {
    cutoff = value;
    effective_omega = 2 * M_PI / static_cast<float_s>(config::sample_rate) * cutoff;
	cos_omega = std::cos(effective_omega);
	sin_omega = std::sin(effective_omega);
    compute_alpha();
    compute_coefficients();
}