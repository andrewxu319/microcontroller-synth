#pragma once

#include <immintrin.h>

namespace accelerator {
	inline void vec_add_float_s(const float_s* const in_1, const float_s* const in_2, float_s* const out, const size_t len) {
		// if standalone
		for (size_t i = 0; i < len; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_1{ _mm256_loadu_ps(&in_1[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_2{ _mm256_loadu_ps(&in_2[i]) };
			const __m256 avx_result{ _mm256_add_ps(avx_1, avx_2) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
	}

	inline __m256 scalar_reg{}; // preload scalar register

	inline void set_scalar(const float_s value) {
		scalar_reg = _mm256_set1_ps(value);
	}

	inline void vec_mult_float_s(const float_s* const in, float_s* const out, const size_t len) {
		// if standalone
		for (size_t i = 0; i < len; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in{ _mm256_loadu_ps(&in[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_result{ _mm256_mul_ps(avx_in, scalar_reg) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
	}

	inline void vec_mult_float_s(const float_s* const in, const float_s scalar, float_s* const out, const size_t len) {
		// if standalone
		const __m256 scalar_reg_{ _mm256_set1_ps(scalar) };
		for (size_t i = 0; i < len; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in{ _mm256_loadu_ps(&in[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_result{ _mm256_mul_ps(avx_in, scalar_reg_) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
	}
}