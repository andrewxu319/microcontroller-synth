#pragma once

#include "reverb.h"

#include "synthesis/modules/fx/components/delay_line.h"
#include "synthesis/modules/fx/components/schroeder_allpass.h"
#include "synthesis/modules/mixer.h"

#include <array>

namespace synthesis {
	class Schroeder : public Reverb {
	public:
		Schroeder();
		void generate_buf() override;
		int add_input(Module* __restrict input, const uint8_t buf_type) override;

	private:
		array<WetOnlyDelayLine, 4> delay_lines;
		Mixer mixer;
		SchroederAllpass allpass_filters[2];
	};
}