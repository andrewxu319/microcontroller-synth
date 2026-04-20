#pragma once

#include "synthesis/modules/fx/fx.h"
#include "utils/global.h"
#include "utils/math.h"

#ifdef TEENSY
	#include <arm_math.h>
#else
	#include <Eigen/Dense>
#endif

namespace synthesis {
	class Reverb : public Fx {
	public:
		Reverb();
		//virtual void generate_buf();
		void set_decay_time(double value_ms);

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

#ifdef TEENSY
		template <uint8_t N>
		void create_mixing_matrix(MixingMatrix type, arm_matrix_instance_f32* dest);
#else
		template <uint8_t N>
		constexpr Eigen::Matrix<float_s, N, N> create_mixing_matrix(MixingMatrix type);
#endif
	};
}

using namespace synthesis;

#ifdef TEENSY
template <uint8_t N>
void Reverb::create_mixing_matrix(MixingMatrix type, arm_matrix_instance_f32* dest) {
	memset(dest->pData, 0.0f, N * N * sizeof(float_s));

	using MatrixNxN = arm_matrix_instance_f32;

	float_s identity_data[N * N]{};
	for (size_t i{}; i < N; i++) {
    	identity_data[i * N + i] = 1.0f;
	}
	MatrixNxN identity{};
	arm_mat_init_f32(&identity, N, N, identity_data);

	constexpr float_s filled_data[N * N]{};
	math::vec_scal_add_float_s(filled_data, filled_data, -2.0 / N, N * N);
	MatrixNxN filled{};
	arm_mat_init_f32(&filled, N, N, filled_data);

	switch (type) {
	case MixingMatrix::Householder:
		arm_mat_add_f32(&identity, &filled, dest);
		return;
	default:
		arm_mat_add_f32(&identity, dest, dest);
		return;
	}
}
#else
template <uint8_t N>
constexpr Eigen::Matrix<float_s, N, N> Reverb::create_mixing_matrix(MixingMatrix type) {
	using MatrixNxN = Eigen::Matrix<float_s, N, N>;

	switch (type) {
	case MixingMatrix::Householder:
		return MatrixNxN::Identity() - (2.0f / N * MatrixNxN::Ones());
	default:
		return MatrixNxN::Identity();
	}
}
#endif