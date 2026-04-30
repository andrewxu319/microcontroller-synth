#include "luff.h"

// from https://signalsmith-audio.co.uk/writing/2021/lets-write-a-reverb/#diffusion-a-multi-channel-diffuser-choosing-delay-times

using namespace synthesis;

Luff::Luff(uint8_t diffusion_steps_)
	: diffusers{ diffusion_steps_ },
	delay_line{ 200, NUM_CHANNELS }, // 5 is temp value
	mixing_matrix{},
#ifdef TEENSY
	buffers_matrix{},
#endif
	output_channels{},
	mixer{},
	avg_feedback{}
{
	assert(diffusion_steps_ >= 2);
	static_assert(NUM_CHANNELS >= 2);
	for (uint8_t i{ 0 }; i < diffusion_steps_; i++) {
		diffusers[i] = std::make_unique<MultichannelDiffuser>(5, NUM_CHANNELS); // 5 is temporary
		diffusers[i]->resize(20);
	}
#ifdef TEENSY
	arm_mat_init_f32(&mixing_matrix, NUM_CHANNELS, NUM_CHANNELS, nullptr);
	arm_mat_init_f32(&buffers_matrix, NUM_CHANNELS, config::buffer_size, nullptr);
	arm_mat_init_f32(&output_channels, NUM_CHANNELS, config::buffer_size, nullptr);
#endif
}

void Luff::init() {
	{
		for (uint8_t j{ 0 }; j < NUM_CHANNELS; j++) {
			diffusers[0]->add_buf(audio_in_buf, MultichannelDiffuser::BufType::AUDIO);
		}
		diffusers[0]->init();
	}
	for (uint8_t i{ 1 }; i < diffusers.size(); i++) {
		for (uint8_t j{ 0 }; j < NUM_CHANNELS; j++) {
			diffusers[i]->add_buf(diffusers[i - 1]->get_out_bufs()[j].data(), MultichannelDiffuser::BufType::AUDIO);
		}
		diffusers[i]->init();
	}

	mixer.add_buf(audio_in_buf, Mixer::BufType::AUDIO);
	for (uint8_t i{ 0 }; i < NUM_CHANNELS; i++) {
		delay_line.add_buf(diffusers.back()->get_out_bufs()[i].data(), DelayLine::BufType::AUDIO);
		mixer.add_buf(delay_line.get_out_bufs()[i].data(), Mixer::BufType::AUDIO);
	}
	delay_line.init();
	mixer.init();
}

void Luff::generate_buf() {
	float_s decay_buf_sum[config::buffer_size];
	const bool decay_mods{ sum_bufs(BufType::DECAY, decay_buf_sum, decay) };
	// wip

	for (std::unique_ptr<MultichannelDiffuser>& diffuser : diffusers) {
		diffuser->generate_buf();
	}
	delay_line.generate_buf();

#ifdef TEENSY
	// stupid const_cast!!!
	buffers_matrix.pData = const_cast<float_s*>(delay_line.get_out_bufs()[0].data()); // pointer to array of arrays (matrix)
	arm_mat_mult_f32(&mixing_matrix, &buffers_matrix, &output_channels);
#else
	// using BuffersMatrix = Eigen::Matrix<float_s, NUM_CHANNELS, config::buffer_size, Eigen::RowMajor>;
	Eigen::Map<const BuffersMatrix> buffers_matrix{ delay_line.get_out_bufs()[0].data() }; // pointer to array of arrays (matrix)
	output_channels = mixing_matrix * buffers_matrix;
#endif

	mixer.generate_buf();

	memcpy(out_buf, mixer.get_out_buf(), config::buffer_size * sizeof(float_s));

	mix_dry_wet();
}

void Luff::set_diffuser_delays(std::initializer_list<double> values_ms) {
	assert(values_ms.size() == diffusers.size());
	for (uint8_t i{ 0 }; i < diffusers.size(); i++) {
		diffusers[i]->set_delay(*(values_ms.begin() + i));
	}
}

void Luff::set_feedback(float_s value) {
	avg_feedback = value;
	constexpr float_s feedback_half_range{ 0.05f }; // temporary magic number
	float_s upper_bound{ value - feedback_half_range };
	float_s lower_bound{ value + feedback_half_range };

	delay_line.set_feedback(upper_bound, 0);
	delay_line.set_feedback(lower_bound, 1);

	double window_size{ (upper_bound - lower_bound) / (NUM_CHANNELS - 1) };
	double next_window_lower_bound{ lower_bound + window_size / 2 };
	for (uint8_t i{ 2 }; i < NUM_CHANNELS; i++) {
		delay_line.set_feedback(utils::rng_uniform<float_s>(next_window_lower_bound, next_window_lower_bound += window_size), i);
	}
	set_decay_time(decay); // DECAY TIME
}

void Luff::set_decay_time(double value_ms) {
	// "decay time" = t_60 (time to decay by 60dB)
	decay = value_ms;
	assert(avg_feedback > 0.0f);
	double avg_feedback_delay{ value_ms * log10(avg_feedback) / -3 }; // strictly pretty sure we can't use the simple mean of feedbacks
	constexpr double half_range{ 1.0f / 3.0f }; // temporary magic number. luff's example article used 100-200 => centered at 150ms
	double max{ avg_feedback_delay * (1.0f + half_range) };
	set_feedback_delay_range(max / 2, max);
}

// randomize, but each channel has its own interval. similar to multichannel diffuser
void Luff::set_feedback_delay_range(double min_ms, double max_ms) {
	delay_line.set_delay(max_ms, 0);

	double window_size{ (max_ms - min_ms) / (NUM_CHANNELS - 1) };
	double next_window_lower_bound{ min_ms };
	for (uint8_t i{ 1 }; i < NUM_CHANNELS; i++) {
		delay_line.set_delay(utils::rng_uniform(next_window_lower_bound, next_window_lower_bound + window_size), i);
		next_window_lower_bound += window_size;
	}
}

void Luff::set_mixing_matrix(MixingMatrix type) {
#ifdef TEENSY
	create_mixing_matrix<NUM_CHANNELS>(MixingMatrix::Householder, &mixing_matrix);
#else
	mixing_matrix = create_mixing_matrix<NUM_CHANNELS>(MixingMatrix::Householder);
#endif
}