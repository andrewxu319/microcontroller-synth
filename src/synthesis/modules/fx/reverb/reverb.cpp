#include "reverb.h"

using namespace synthesis;

Reverb::Reverb()
	: Fx(in_bufs),
	decay{},
	decay_samples{}
{ }

void Reverb::set_decay_time(double value_ms) {
	decay = value_ms;
}