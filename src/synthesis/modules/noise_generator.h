#pragma once

#include "module.h"

#include "utils/utils.h"

#include <string>

// idea for multiple voices: arrays of phase, freq, period, one for each voice

namespace synthesis {
	class NoiseGenerator : public Module {
	public:
		enum Mods {
			GAIN
		};

		NoiseGenerator(const bool unipolar = false);
		void generate_buf() override;

	private:
		vector<float_s*> mods[1];
		float_s gain;
	};
}