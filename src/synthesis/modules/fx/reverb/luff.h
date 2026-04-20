#pragma once

#include "reverb.h"

#include "synthesis/modules/fx/components/multichannel_diffuser.h"
#include "synthesis/modules/fx/components/delay_line.h"
#include "synthesis/modules/mixer.h"

#ifdef TEENSY
	#include <arm_math.h>
#else
	#include <Eigen/Dense>
#endif
#include <vector>
#include <array>
#include <memory>
#include <initializer_list>

namespace synthesis {
	class Luff : public Reverb {
	public:
		Luff(uint8_t diffusion_steps_ = 5);
		void init() override;
		void generate_buf() override;
		void set_diffuser_delays(std::initializer_list<double> values_ms);
		void set_feedback(float_s value);
		void set_decay_time(double value_ms);
		void set_feedback_delay_range(double min_ms, double max_ms);
		void set_mixing_matrix(MixingMatrix type);

	private:
		static constexpr uint8_t NUM_CHANNELS{ 8 };
		std::vector<std::unique_ptr<MultichannelDiffuser>> diffusers;
		DelayLine delay_line; // multichannel
#ifdef TEENSY
		arm_matrix_instance_f32 mixing_matrix;
		float_s mixing_matrix_data[NUM_CHANNELS * NUM_CHANNELS];
		using BuffersMatrix = arm_matrix_instance_f32;
		BuffersMatrix buffers_matrix;
#else
		Eigen::Matrix<float_s, NUM_CHANNELS, NUM_CHANNELS> mixing_matrix;
		using BuffersMatrix = Eigen::Matrix<float_s, NUM_CHANNELS, config::buffer_size, Eigen::RowMajor>;
#endif
		BuffersMatrix output_channels;
		Mixer mixer;
		float_s avg_feedback;
	};
}