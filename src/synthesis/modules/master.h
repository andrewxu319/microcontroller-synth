#pragma once

#include "utils/includes.h"
#include "mixer.h"
#include "module.h"

using namespace synth;

class Master : public Mixer {
public:
	float32_t* out_buf;

	Master();
	void generate_buf();
};