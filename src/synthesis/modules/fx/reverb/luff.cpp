#include "luff.h"

// from https://signalsmith-audio.co.uk/writing/2021/lets-write-a-reverb/#diffusion-a-multi-channel-diffuser-choosing-delay-times

using namespace synthesis;

Luff::Luff(uint8_t diffusion_steps_)
	: diffusers{ diffusion_steps_ },
	delay_line{ 200, NUM_CHANNELS }, // 5 is temp value
	mixing_matrix{},
	output_channels{},
	mixer{},
	avg_feedback{}
{
	assert(diffusion_steps_ >= 2);
	static_assert(NUM_CHANNELS >= 2);
	for (uint8_t i{ 0 }; i < diffusion_steps_; i++) {
		diffusers[i] = make_unique<MultichannelDiffuser>(5, NUM_CHANNELS); // 5 is temporary
		diffusers[i]->resize(20);
	}
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
	for (unique_ptr<MultichannelDiffuser>& diffuser : diffusers) {
		diffuser->generate_buf();
	}
	delay_line.generate_buf();

	using BuffersMatrix = Eigen::Matrix<float_s, NUM_CHANNELS, config::buffer_size, Eigen::RowMajor>;
	Eigen::Map<const BuffersMatrix> buffers_matrix{ delay_line.get_out_bufs()[0].data() };
	output_channels = mixing_matrix * buffers_matrix;

	mixer.generate_buf();
}

// redirect out_buf
const float_s* Luff::get_out_buf() const {
	return mixer.get_out_buf();
}

void Luff::set_diffuser_delays(initializer_list<double> values_ms) {
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
		delay_line.set_feedback(utils::rng_uniform(next_window_lower_bound, next_window_lower_bound += window_size), i);
	}
	set_decay_time(decay); // DECAY TIME
}

void Luff::set_decay_time(double value_s) {
	// "decay time" = t_60 (time to decay by 60dB)
	assert(avg_feedback > 0.0f);
	double avg_feedback_delay{ value_s * log10(avg_feedback) / -3 }; // strictly pretty sure we can't use the simple mean of feedbacks
	constexpr double half_range{ 1.0f / 3.0f }; // temporary magic number. luff's example article used 100-200 => centered at 150ms
	set_feedback_delay_range(avg_feedback_delay * (1.0f + half_range));
}

// randomize, but each channel has its own interval. similar to multichannel diffuser
void Luff::set_feedback_delay_range(double max_ms) {
	delay_line.set_delay(max_ms, 0);

	double window_size{ (max_ms) / (NUM_CHANNELS - 1) };
	double next_window_lower_bound{ window_size / 2};
	for (uint8_t i{ 1 }; i < NUM_CHANNELS; i++) {
		delay_line.set_delay(utils::rng_uniform(next_window_lower_bound, next_window_lower_bound += window_size), i);
	}
}

void Luff::set_mixing_matrix(MixingMatrix type) {
	mixing_matrix = create_mixing_matrix<NUM_CHANNELS>(MixingMatrix::Householder);
}