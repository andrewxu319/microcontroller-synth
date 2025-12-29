#pragma once

#include "module.h"

#include "utils/utils.h"

#include <string>

// idea for multiple voices: arrays of phase, freq, period, one for each voice

namespace synthesis {
	class NoiseGenerator : public Module {
	public:
		enum BufTypes {
			GAIN
		};

		NoiseGenerator(const bool unipolar = false);
		void generate_buf() override;

	protected:
		vector<const float_s*> in_bufs[1];

	private:
		float_s gain;
	};
}