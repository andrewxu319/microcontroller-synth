#include "schroeder.h"

using namespace synthesis;

Schroeder::Schroeder()
	: delay_lines{
		WetOnlyDelayLine{ 5 },
		WetOnlyDelayLine{ 5 },
		WetOnlyDelayLine{ 5 },
		WetOnlyDelayLine{ 5 }
	}, // value is temporary. tbd
	mixer{},
	allpass_filters{}
{
}

int Schroeder::add_input(Module* __restrict input, const uint8_t buf_type) {
	const int ret{ Fx::add_input(input, buf_type) };

	if (buf_type == BufTypes::AUDIO && in_bufs[BufTypes::AUDIO].size() == 1) {
		decay = 100.0;
		double decay_factor = 0.5;

		for (WetOnlyDelayLine& delay_line : delay_lines) {
			delay_line.add_buf(audio_in_buf, WetOnlyDelayLine::BufTypes::AUDIO);
			delay_line.audio_in_buf = audio_in_buf;
			mixer.add_buf(delay_line.get_out_buf(), Mixer::BufTypes::AUDIO);
		}
		allpass_filters[0].add_buf(mixer.get_out_buf(), SchroederAllpass::BufTypes::AUDIO);
		allpass_filters[0].audio_in_buf = mixer.get_out_buf();
		allpass_filters[1].add_buf(allpass_filters[0].get_out_buf(), SchroederAllpass::BufTypes::AUDIO);
		allpass_filters[1].audio_in_buf = allpass_filters[0].get_out_buf();
		
		delay_lines[0].set_delay(36.7);
		delay_lines[1].set_delay(48.6);
		delay_lines[2].set_delay(44.0);
		delay_lines[3].set_delay(40.6);
		allpass_filters[0].set_delay(6.12);
		allpass_filters[1].set_delay(2.12);
		delay_lines[0].set_feedback(0.8809);
		delay_lines[1].set_feedback(0.8455);
		delay_lines[2].set_feedback(0.8590);
		delay_lines[3].set_feedback(0.8692);
		allpass_filters[0].set_feedback(0.707);
		allpass_filters[1].set_feedback(0.707);
	}

	return ret;
}

void Schroeder::generate_buf() {
	for (WetOnlyDelayLine& delay_line : delay_lines) {
		delay_line.generate_buf();
	}
	mixer.generate_buf();
	allpass_filters[0].generate_buf();
	allpass_filters[1].generate_buf();
	memcpy(out_buf, allpass_filters[1].get_out_buf(), config::buffer_size * sizeof(float_s));

	mix_dry_wet();
}