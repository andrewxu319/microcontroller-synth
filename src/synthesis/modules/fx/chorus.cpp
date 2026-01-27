#include "chorus.h"

#include "utils/accelerator.h"



using namespace synthesis;

Chorus::Chorus()
	: Fx(in_bufs),
	memory_buffer{ 120 * config::buffer_size }, // later: make it adjust based on sample rate & buffer size
	num_voices{},
	range_proportion_to_increment{},
	voices{},
	delay_center{},
	delay_lfo_center{ 100 },
	delay_lfo_range{ 50 },
	freq_center{ 1 },
	freq_range{ 0.5 }
{
	;
}

void Chorus::generate_buf() {
	// // how do we detect when the note dies down? can't use delay time because it gets modulated
	// // check common delay times
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		memory_buffer.reset();
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	memcpy(out_buf, audio_in_buf, config::buffer_size * sizeof(float_s));

	if (wet <= 0.0) return;

	memory_buffer.pop_start_with_pointer(config::buffer_size); // advance the start pointer
	memory_buffer.push_back(out_buf, config::buffer_size); // now most recent sample is at index size - 1

	// put these here to avoid repeatedly checking if statement
	float_s delay_lfo_range_buf_sum[config::buffer_size];
	if (sum_bufs(BufType::DELAY_LFO_RANGE, delay_lfo_range_buf_sum)) {
		for (uint8_t i{ 0 }; i < num_voices; i++) {
			accelerator::vec_scal_mult_float_s(delay_lfo_range_buf_sum, voices[i].lfo_gain_offset, i * range_proportion_to_increment - 1, config::buffer_size);
		}
	}

	float_s freq_range_buf_sum[config::buffer_size];
	if (sum_bufs(BufType::FREQ_RANGE, freq_range_buf_sum)) {
		for (uint8_t i{ 0 }; i < num_voices; i++) {
			accelerator::vec_scal_mult_float_s(freq_range_buf_sum, voices[i].lfo_freq_offset, i * range_proportion_to_increment - 1, config::buffer_size);
		}
	}

	for (ChorusVoice& voice : voices) {
		voice.lfo->generate_buf();
	}

	float_s delay_buf_sum[config::buffer_size];
	const bool delay_mods{ sum_bufs(BufType::DELAY, delay_buf_sum, delay_center) };
	for (ChorusVoice& voice : voices) {
		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			const float_s effective_delay{ (delay_mods ? delay_buf_sum[i] : 0.0f) + voice.lfo->get_out_buf()[i] };
			const size_t effective_delay_floored{ static_cast<size_t>(effective_delay) };
			const size_t index_ceiled{ memory_buffer.size - config::buffer_size + i - effective_delay_floored };
			const float_s delayed_signal_prev{ memory_buffer.get(index_ceiled) };
			const float_s delayed_signal_next{ memory_buffer.get(index_ceiled - 1) };
			const float_s interpolation_ratio{ effective_delay - effective_delay_floored };
			const float_s delayed_signal_interpolated{ delayed_signal_prev * (1.0f - interpolation_ratio) + delayed_signal_next * interpolation_ratio };
			out_buf[i] += delayed_signal_interpolated;
		}
	}

	mix_dry_wet();
}

void Chorus::set_delay(const double value) { // in ms
	delay_center = value * 0.001 * config::sample_rate; // in samples
}

void Chorus::set_voice_count(const uint8_t value) {
	assert(value > 1);

	if (value > num_voices) {
		voices.reserve(value);
		for (uint8_t i{ num_voices }; i < value; i++) {
			voices.emplace_back(ChorusVoice{ make_unique<Oscillator>("sine") });
			voices.back().lfo->add_output(this);
			//voices.back().lfo->set_phase(utils::rng_uniform(0.0, 1.0)); // not needed?
		}
	}
	else {
		voices.resize(value);
	}

	num_voices = value;
	
	float_s delay_counter{ delay_lfo_center - delay_lfo_range }; // 5, 10
	float_s freq_counter{ freq_center - freq_range };
	range_proportion_to_increment = 2.0f / (num_voices - 1);
	for (uint8_t i{ 0 }; i < num_voices; i++) {
		voices[i].lfo->set_gain(delay_counter);
		voices[i].lfo->set_freq(freq_counter);
		delay_counter += delay_lfo_range * range_proportion_to_increment;
		freq_counter += freq_range * range_proportion_to_increment;
	}
}

int Chorus::add_input(Module* __restrict input, const uint8_t buf_type = -1) {
	switch (buf_type) {
	case BufType::DELAY_LFO_CENTER:
		for (ChorusVoice& voice : voices) {
			voice.lfo->add_input(input, Oscillator::BufType::GAIN);
		}
		break;
	case BufType::DELAY_LFO_RANGE:
		for (uint8_t i{ 0 }; i < num_voices; i++) {
			voices[i].lfo->add_buf(voices[i].lfo_gain_offset, Oscillator::BufType::GAIN);
		}
		break;
	case BufType::FREQ:
		for (ChorusVoice& voice : voices) {
			voice.lfo->add_input(input, Oscillator::BufType::PITCH);
		}
		break;
	case BufType::FREQ_RANGE:
		for (uint8_t i{ 0 }; i < num_voices; i++) {
			voices[i].lfo->add_buf(voices[i].lfo_freq_offset, Oscillator::BufType::PITCH);
		}
		break;
	default:
		break;
	}

	return Fx::add_input(input, buf_type);
}