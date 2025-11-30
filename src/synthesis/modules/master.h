#pragma once

#include "mixer.h"
#include "module.h"

using namespace synth;

class Master : public Mixer {
public:
	buffer out_buf_to_sound_engine{};

	Master();
	void generate_buf();
};