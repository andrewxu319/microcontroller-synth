#pragma once

#include "reverb.h"

#include "synthesis/modules/fx/components/multichannel_diffuser.h"
#include "synthesis/modules/fx/components/delay_line.h"
#include "synthesis/modules/mixer.h"

#include <Eigen/Dense>
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
		const float_s* get_out_buf() const override;
		void set_diffuser_delays(std::initializer_list<double> values_ms);
		void set_feedback(float_s value);
		void set_decay_time(double value_s);
		void set_feedback_delay_range(double max_ms);
		void set_mixing_matrix(MixingMatrix type);

	private:
		static constexpr uint8_t NUM_CHANNELS{ 8 };
		std::vector<std::unique_ptr<MultichannelDiffuser>> diffusers;
		DelayLine delay_line; // multichannel
		Eigen::Matrix<float_s, NUM_CHANNELS, NUM_CHANNELS> mixing_matrix;
		using BuffersMatrix = Eigen::Matrix<float_s, NUM_CHANNELS, config::buffer_size, Eigen::RowMajor>;
		BuffersMatrix output_channels;
		Mixer mixer;
		float_s avg_feedback;
	};
}