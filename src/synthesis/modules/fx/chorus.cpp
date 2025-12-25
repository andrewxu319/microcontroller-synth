#include "chorus.h"

#include "utils/accelerator.h"

using namespace synthesis;

Chorus::Chorus()
	: Fx(mods, sizeof(mods) / sizeof(vector<Module*>)),
	memory_buffer{ 120 * config::buffer_size }, // later: make it adjust based on sample rate & buffer size
	num_voices{},
	range_proportion_to_increment{},
	lfos{},
	delay_center{},
	delay_lfo_center{ 100 },
	delay_lfo_range{ 50 },
	freq_center{ 1 },
	freq_range{ 0.5}
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

	for (Oscillator& lfo : lfos) {
		lfo.generate_buf();

		float_s* effective_delay{ lfo.out_buf };
		for (Module* mod : mods[Mods::DELAY]) {
			accelerator::vec_add_float_s(in_bufs[mod->id].data, effective_delay, effective_delay, config::buffer_size);
		}
		accelerator::vec_scal_add_float_s(effective_delay, effective_delay, delay_center, config::buffer_size);

		for (size_t i{ 0 }; i < config::buffer_size; i++) {
			const float_s delayed_signal_low{ memory_buffer.get(memory_buffer.size - config::buffer_size + i - static_cast<size_t>(effective_delay[i])) };
			const float_s delayed_signal_high{ memory_buffer.get(memory_buffer.size - config::buffer_size + i - static_cast<size_t>(effective_delay[i]) - 1) };
			const float_s decimal_part{ effective_delay[i] - static_cast<size_t>(effective_delay[i]) };
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
	if (value > num_voices) {
		lfos.reserve(value);
		for (uint8_t i{ num_voices }; i < value; i++) {
			lfos.emplace_back(Oscillator{"sine"});
			lfos.back().add_output(this, true);
		}
	}
	else {
		lfos.resize(value);
	}

	num_voices = value;
	
	double delay_counter{ delay_lfo_center - delay_lfo_range }; // 5, 10
	float_s freq_counter{ freq_center - freq_range };
	range_proportion_to_increment = 2.0 / (num_voices - 1);
	for (uint8_t i{ 0 }; i < num_voices; i++) {
		lfos[i].set_gain(delay_counter);
		lfos[i].set_freq(freq_counter);
		delay_counter += delay_lfo_range * range_proportion_to_increment;
		freq_counter += freq_range * range_proportion_to_increment;
	}
}

void Chorus::attach_mod(Module* __restrict mod, uint8_t target) {
	Module::attach_mod(mod, target);

	switch (target) {
	case Mods::DELAY_LFO_CENTER:
		for (Oscillator& lfo : lfos) {
			lfo.attach_mod(mod, Oscillator::Mods::GAIN);
		}
		break;
	case Mods::DELAY_LFO_RANGE:
		float_s mod_buffer_temp[config::buffer_size];
		for (uint8_t i{ 0 }; i < num_voices; i++) {
			// we might need to change "mods" so that it stores array pointers, not module pointers
		}
	case Mods::FREQ:
		for (Oscillator& lfo : lfos) {
			lfo.attach_mod(mod, Oscillator::Mods::PITCH);
		}
		break;
	default:
		break;
	}
}