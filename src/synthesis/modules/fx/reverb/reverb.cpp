#include "reverb.h"

using namespace synthesis;

Reverb::Reverb()
	: Fx(in_bufs),
	decay{},
	decay_samples{}
{ }