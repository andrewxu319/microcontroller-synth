#pragma once

#include "reverb.h"

#include "synthesis/modules/fx/components/delay_line.h"
#include "synthesis/modules/fx/components/schroeder_allpass.h"
#include "synthesis/modules/mixer.h"

#include <array>
#include <memory>

namespace synthesis {
	class Schroeder : public Reverb {
	public:
		Schroeder();
		void init() override;
		void generate_buf() override;

	private:
		std::array<std::unique_ptr<WetOnlyDelayLine>, 4> delay_lines;
		Mixer mixer;
		SchroederAllpass allpass_filters[2];
	};
}