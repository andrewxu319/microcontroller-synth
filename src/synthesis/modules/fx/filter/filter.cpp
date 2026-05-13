#include "filter.h"

using namespace synthesis;

Filter::Filter()
	: Fx(in_bufs), in_bufs{}, cutoff{}, effective_omega{}, qbs{}, effective_qbs{}
{
}

void Filter::set_cutoff(double value) {
	assert(value > 0);
	cutoff = value;
	effective_omega = 2 * M_PI / static_cast<float_s>(config::sample_rate) * cutoff;
}

void Filter::set_qbs(double value) {
	assert(value > 0);
	qbs = value;
	effective_qbs = value;
}