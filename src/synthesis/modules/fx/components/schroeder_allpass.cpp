/*
https://hajim.rochester.edu/ece/sites/zduan/teaching/ece472/reading/Schroeder_1962.pdf
https://medium.com/the-seekers-project/coding-a-basic-reverb-algorithm-part-2-an-introduction-to-audio-programming-4db79dd4e325
*/

#include "schroeder_allpass.h"

using namespace synthesis;

SchroederAllpass::SchroederAllpass()
	: Fx(in_bufs),
	in_bufs{},
	delay_line{ 1, 1 }, // 8 IS ARBITRARY PLACEHOLDER (enough for about 4? second decay)
	adder{},
	feedback{},
	delay{}
{ }

void SchroederAllpass::init() {
	delay_line.add_buf(audio_in_buf, WetOnlyDelayLine::BufType::AUDIO);
	delay_line.set_feedback(feedback);

	adder.add_buf(audio_in_buf, Mixer::BufType::AUDIO);
	adder.set_in_buf_gain(audio_in_buf, -feedback);
	adder.add_buf(delay_line.get_out_bufs()[0].data(), Mixer::BufType::AUDIO);
	adder.set_in_buf_gain(delay_line.get_out_bufs()[0].data(), 1 - feedback * feedback);
}

void SchroederAllpass::generate_buf() {
	delay_line.generate_buf();
	adder.generate_buf();
	memcpy(out_buf, adder.get_out_buf(), config::buffer_size * sizeof(float_s));
}

void SchroederAllpass::set_delay(const double value_ms) {
	delay = static_cast<size_t>(value_ms * 0.001 * config::sample_rate);
	delay_line.set_delay(value_ms);
}

void SchroederAllpass::set_feedback(const float_s value) {
	assert(value > -1.0 && value < 1.0);
	feedback = value;
}

int SchroederAllpass::add_input(Module* __restrict input, uint8_t buf_type) {
	switch (buf_type) {
	case BufType::DELAY:
		delay_line.add_input(input, BufType::DELAY);
		break;
	default:
		break;
	}
	return Fx::add_input(input, buf_type);
}
int SchroederAllpass::add_input(MultichannelModule* __restrict input, uint8_t buf_type) {
	switch (buf_type) {
	case BufType::DELAY:
		delay_line.add_input(input, BufType::DELAY);
		break;
	default:
		break;
	}
	return Fx::add_input(input, buf_type);
}