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
		void set_decay_time(double value_ms);

	private:
		std::array<std::unique_ptr<WetOnlyDelayLine>, 4> delay_lines;
		Mixer mixer;
		SchroederAllpass allpass_filters[2];
		std::array<std::array<float_s, config::buffer_size>, 6> decay_bufs;
	};
}