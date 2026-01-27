#pragma once

#include "synthesis/modules/fx/fx.h"
#include "utils/global.h"

#include <Eigen/Dense>

namespace synthesis {
	class Reverb : public Fx {
	public:
		Reverb();
		//virtual void generate_buf();

		enum MixingMatrix {
			Householder
		};
		enum BufType {
			AUDIO,
			WET,
			DECAY
		};

	protected:
		std::vector<const float_s*> in_bufs[3];
		double decay;
		size_t decay_samples;

		template <uint8_t N>
		constexpr Eigen::Matrix<float_s, N, N> create_mixing_matrix(MixingMatrix type);
	};
}

using namespace synthesis;

template <uint8_t N>
constexpr Eigen::Matrix<float_s, N, N> Reverb::create_mixing_matrix(MixingMatrix type) {
	using MatrixNxN = Eigen::Matrix<float_s, N, N>;

	switch (type) {
	case MixingMatrix::Householder:
		return MatrixNxN::Identity() - (2.0f / N * MatrixNxN::Ones());
	}
}