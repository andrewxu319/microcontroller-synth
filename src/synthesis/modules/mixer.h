#pragma once

#include "module.h"

using namespace synth;

class Mixer : public Module {
public:
	Mixer();
	Mixer(const NoBaseInit); // dummy constructor
	void generate_buf();
};