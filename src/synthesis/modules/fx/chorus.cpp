#include "chorus.h"

#include "utils/accelerator.h"

#include <cassert>

using namespace synthesis;

Chorus::Chorus()
	: Fx(mods, sizeof(mods) / sizeof(vector<float_s*>)),
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
	if (audio_in_buf->data[0] == EMPTY_BUF_MARKER) {
		memory_buffer.reset();
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	memcpy(out_buf, audio_in_buf->data, config::buffer_size * sizeof(float_s));

	if (wet <= 0.0) return;

	memory_buffer.pop_start_with_pointer(config::buffer_size); // advance the start pointer
	memory_buffer.push_back(out_buf, config::buffer_size); // now most recent sample is at index size - 1

	// put these here to avoid repeatedly checking if statement
	float_s* effective_delay_lfo_range{ sum_mods(Mods::DELAY_LFO_RANGE) };
	if (effective_delay_lfo_range) {
		for (uint8_t i{ 0 }; i < num_voices; i++) {
			accelerator::vec_scal_mult_float_s(effective_delay_lfo_range, voices[i].lfo_gain_offset, i * range_proportion_to_increment - 1, config::buffer_size);
		}
	}
	float_s* effective_freq_range{ sum_mods(Mods::FREQ_RANGE) };
	if (effective_freq_range) {
		for (uint8_t i{ 0 }; i < num_voices; i++) {
			accelerator::vec_scal_mult_float_s(effective_freq_range, voices[i].lfo_freq_offset, i * range_proportion_to_increment - 1, config::buffer_size);
		}
	}

	for (ChorusVoice& voice : voices) {
		voice.lfo->generate_buf();
	}

	float_s* delay_mod_sum{ sum_mods(Mods::DELAY) };
	if (delay_mod_sum) {
		for (ChorusVoice& voice : voices) {
			voice.effective_delay = voice.lfo->out_buf; // reusing memory of voice.lfo->out_buf because we don't use it again. doing this for readability
			accelerator::vec_add_float_s(voice.lfo->out_buf, delay_mod_sum, voice.effective_delay, config::buffer_size);
			accelerator::vec_scal_add_float_s(voice.effective_delay, voice.effective_delay, delay_center, config::buffer_size);
		}
	}
	else {
		for (ChorusVoice& voice : voices) {
			voice.effective_delay = voice.lfo->out_buf; // reusing memory of voice.lfo->out_buf because we don't use it again. doing this for readability
			// maybe even delete this pointer reassignment stuff if needed and just use voice.lfo->out_buf
			accelerator::vec_scal_add_float_s(voice.effective_delay, voice.effective_delay, delay_center, config::buffer_size);
		}
	}

	for (ChorusVoice& voice : voices) {
		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			const float_s delayed_signal_low{ memory_buffer.get(memory_buffer.size - config::buffer_size + i - static_cast<size_t>(voice.effective_delay[i])) };
			const float_s delayed_signal_high{ memory_buffer.get(memory_buffer.size - config::buffer_size + i - static_cast<size_t>(voice.effective_delay[i]) - 1) };
			const float_s decimal_part{ voice.effective_delay[i] - static_cast<size_t>(voice.effective_delay[i]) };
			const float_s delayed_signal_interpolated{ delayed_signal_low * (1.0f - decimal_part) + delayed_signal_high * decimal_part };
			out_buf[i] += delayed_signal_interpolated;
		}
	}

	mix_dry_wet(Mods::WET);
}

void Chorus::set_delay(const double value) { // in ms
	delay_center = value * 0.001 * config::sample_rate;
}

void Chorus::set_voice_count(const uint8_t value) {
	assert(value > 1);

	if (value > num_voices) {
		voices.reserve(value);
		for (uint8_t i{ num_voices }; i < value; i++) {
			voices.emplace_back(ChorusVoice{ make_unique<Oscillator>("sine") });
			voices.back().lfo->add_output(this, true);
			//voices.back().lfo->set_phase(utils::rng_uniform(0.0, 1.0)); // not needed?
		}
	}
	else {
		voices.resize(value);
	}

	num_voices = value;
	
	double delay_counter{ delay_lfo_center - delay_lfo_range }; // 5, 10
	float_s freq_counter{ freq_center - freq_range };
	range_proportion_to_increment = 2.0 / (num_voices - 1);
	for (uint8_t i{ 0 }; i < num_voices; i++) {
		voices[i].lfo->set_gain(delay_counter);
		voices[i].lfo->set_freq(freq_counter);
		delay_counter += delay_lfo_range * range_proportion_to_increment;
		freq_counter += freq_range * range_proportion_to_increment;
	}
}

void Chorus::attach_mod(float_s* __restrict mod, uint8_t target) {
	Module::attach_mod(mod, target);

	switch (target) {
	case Mods::DELAY_LFO_CENTER:
		for (ChorusVoice& voice : voices) {
			voice.lfo->attach_mod(mod, Oscillator::Mods::GAIN);
		}
		break;
	case Mods::DELAY_LFO_RANGE:
		for (uint8_t i{ 0 }; i < num_voices; i++) {
			voices[i].lfo->attach_mod(voices[i].lfo_gain_offset, Oscillator::Mods::GAIN);
		}
		break;
	case Mods::FREQ:
		for (ChorusVoice& voice : voices) {
			voice.lfo->attach_mod(mod, Oscillator::Mods::PITCH);
		}
		break;
	case Mods::FREQ_RANGE:
		for (uint8_t i{ 0 }; i < num_voices; i++) {
			voices[i].lfo->attach_mod(voices[i].lfo_freq_offset, Oscillator::Mods::PITCH);
		}
		break;
	default:
		break;
	}
}