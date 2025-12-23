#pragma once

#include "module.h"

#include "utils/utils.h"

#include <string>
#include <random>

// idea for multiple voices: arrays of phase, freq, period, one for each voice

namespace synthesis {
	class NoiseGenerator : public Module {
	public:
		float_s waveform[config::waveform_resolution];
		enum Mods {
			GAIN
		};

		NoiseGenerator(const bool unipolar = false);
		void generate_buf() override;
		void load_waveform(const string& path, const bool unipolar = false);

	private:
		vector<Module*> mods[1];
		float_s gain;
		static inline mt19937 rng_engine{ 0 };
		static inline normal_distribution<float_s> rng_dist{ 0.0, 0.333 };

	};
}