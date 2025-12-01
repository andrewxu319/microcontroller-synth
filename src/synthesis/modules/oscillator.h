#pragma once

#include "module.h"

using namespace synth;

class Oscillator : public Module {
public:
	int phase; // in samples
	int freq;  // in hz

	Oscillator();

private:
	void generate_buf();
};