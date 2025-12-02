#pragma once

#include "module.h"

using namespace synth;

class Oscillator : public Module {
public:
	int phase; // in samples
	int freq;  // in hz
	int period; // in samples
	float32_t wavetable[config::wavetable_resolution];

	Oscillator();
	void set_freq(const int val);

private:
	void generate_buf();
};