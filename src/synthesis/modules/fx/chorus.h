#pragma once

#include "synthesis/modules/fx/fx.h"
#include "synthesis/modules/oscillator.h"
#include "utils/includes.h"
#include "utils/config.h"
#include "utils/circular_array.h"

namespace synthesis {
	class Chorus : public Fx {
	public:
		Chorus();
		void generate_buf() override;
		void set_delay(const double value);
		void set_voice_count(const uint8_t value);
		void attach_mod(float_s* __restrict mod, uint8_t target);

		enum Mods {
			WET,
			DELAY,
			DELAY_LFO_CENTER,
			DELAY_LFO_RANGE,
			FREQ,
			FREQ_RANGE
		};

	private:
		struct ChorusVoice {
			unique_ptr<Oscillator> lfo;
			float_s* effective_delay;
			float_s lfo_gain_offset[config::buffer_size];
			float_s lfo_freq_offset[config::buffer_size];
		};

		vector<float_s*> mods[6];
		utils::CircularArray<float_s> memory_buffer;
		vector<ChorusVoice> voices;
		uint8_t num_voices;
		double range_proportion_to_increment;
	public:
		double delay_center; // lfo amplitude
		double delay_lfo_center;
		double delay_lfo_range;
		float_s freq_center;
		float_s freq_range;
	};
}