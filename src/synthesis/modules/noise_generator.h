#pragma once

#include "module.h"

#include "utils/utils.h"

#include <string>

// idea for multiple voices: arrays of phase, freq, period, one for each voice

namespace synthesis {
	class NoiseGenerator : public Module {
	public:
		enum BufType {
			GAIN
		};

		NoiseGenerator(const bool unipolar = false);
		void generate_buf() override;
		void set_gain(const float_s value);

	protected:
		std::vector<const float_s*> in_bufs[1];

	private:
		float_s gain;
	};
}