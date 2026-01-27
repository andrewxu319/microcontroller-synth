#include "schroeder.h"

using namespace synthesis;

Schroeder::Schroeder()
	: mixer{},
	allpass_filters{}
{
	// value is temporary. tbd
	for (uint8_t i{ 0 }; i < 4; i++) {
		delay_lines[i] = std::make_unique<WetOnlyDelayLine>(5);
	}
}

void Schroeder::init() {
	decay = 100.0;
	double decay_factor = 0.5;

	for (std::unique_ptr<WetOnlyDelayLine>& delay_line : delay_lines) {
		delay_line->add_buf(audio_in_buf, WetOnlyDelayLine::BufType::AUDIO);
		mixer.add_buf(delay_line->get_out_bufs()[0].data(), Mixer::BufType::AUDIO);
	}
	allpass_filters[0].add_buf(mixer.get_out_buf(), SchroederAllpass::BufType::AUDIO);
	allpass_filters[0].audio_in_buf = mixer.get_out_buf();
	allpass_filters[1].add_buf(allpass_filters[0].get_out_buf(), SchroederAllpass::BufType::AUDIO);
	allpass_filters[1].audio_in_buf = allpass_filters[0].get_out_buf();

	delay_lines[0]->set_delay(36.7f);
	delay_lines[1]->set_delay(48.6f);
	delay_lines[2]->set_delay(44.0f);
	delay_lines[3]->set_delay(40.6f);
	allpass_filters[0].set_delay(6.12f);
	allpass_filters[1].set_delay(2.12f);
	delay_lines[0]->set_feedback(0.8809f);
	delay_lines[1]->set_feedback(0.8455f);
	delay_lines[2]->set_feedback(0.8590f);
	delay_lines[3]->set_feedback(0.8692f);
	allpass_filters[0].set_feedback(0.707f);
	allpass_filters[1].set_feedback(0.707f);
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