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
		int add_input(Module* __restrict input, const uint8_t buf_type);

		enum BufTypes {
			AUDIO,
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

		vector<const float_s*> in_bufs[7];
		utils::CircularArray<float_s> memory_buffer;
		vector<ChorusVoice> voices;
		uint8_t num_voices;
		float_s range_proportion_to_increment;
	public:
		float_s delay_center; // lfo amplitude
		float_s delay_lfo_center;
		float_s delay_lfo_range;
		float_s freq_center;
		float_s freq_range;
	};
}