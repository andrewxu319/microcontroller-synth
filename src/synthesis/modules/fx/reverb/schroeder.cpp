#include "schroeder.h"

using namespace synthesis;

Schroeder::Schroeder()
	: mixer{},
	allpass_filters{}
{
	// 500 value is temporary. tbd
	for (uint8_t i{ 0 }; i < 4; i++) {
		delay_lines[i] = std::make_unique<WetOnlyDelayLine>(500);
	}
}

void Schroeder::init() {
	//double decay_factor = 0.5;

	for (std::unique_ptr<WetOnlyDelayLine>& delay_line : delay_lines) {
		delay_line->add_buf(audio_in_buf, WetOnlyDelayLine::BufType::AUDIO);
		mixer.add_buf(delay_line->get_out_bufs()[0].data(), Mixer::BufType::AUDIO);
	}
	allpass_filters[0].add_buf(mixer.get_out_buf(), SchroederAllpass::BufType::AUDIO);
	allpass_filters[0].audio_in_buf = mixer.get_out_buf();
	allpass_filters[1].add_buf(allpass_filters[0].get_out_buf(), SchroederAllpass::BufType::AUDIO);
	allpass_filters[1].audio_in_buf = allpass_filters[0].get_out_buf();

	delay_lines[0]->set_feedback(0.8809f);
	delay_lines[1]->set_feedback(0.8455f);
	delay_lines[2]->set_feedback(0.8590f);
	delay_lines[3]->set_feedback(0.8692f);
	allpass_filters[0].set_feedback(0.707f);
	allpass_filters[1].set_feedback(0.707f);

	set_decay_time(1000.0f);

	allpass_filters[0].init();
	allpass_filters[1].init();
}

void Schroeder::generate_buf() {
	for (std::unique_ptr<WetOnlyDelayLine>& delay_line : delay_lines) {
		delay_line->generate_buf();
	}
	mixer.generate_buf();
	allpass_filters[0].generate_buf();
	allpass_filters[1].generate_buf();
	memcpy(out_buf, allpass_filters[1].get_out_buf(), config::buffer_size * sizeof(float_s));

	mix_dry_wet();
}

void Schroeder::set_decay_time(double value_ms) {
	decay = value_ms;

	delay_lines[0]->set_delay(0.367f * value_ms);
	delay_lines[1]->set_delay(0.486f * value_ms);
	delay_lines[2]->set_delay(0.440f * value_ms);
	delay_lines[3]->set_delay(0.406f * value_ms);
	allpass_filters[0].set_delay(0.0612f * value_ms);
	allpass_filters[1].set_delay(0.0212f * value_ms);
}