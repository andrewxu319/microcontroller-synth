#pragma once

#include "module.h"

using namespace synth;

class Oscillator : Module {
	int phase; // in samples
	int freq;  // in hz

public:
	Oscillator();

private:
	void generate_buf();
};