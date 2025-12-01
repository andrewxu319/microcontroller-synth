#pragma once

#include "mixer.h"
#include "module.h"

using namespace synth;

class Master : public Mixer {
public:
	float* out_buf;

	Master();
	void generate_buf();
};