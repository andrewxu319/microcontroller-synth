#pragma once

#include <immintrin.h>

namespace accelerator {
	inline void vec_add_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, const int len) { // in_1 must not equal out
		// if standalone
		int i{ 0 };
		for (; i < len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in_1{ _mm256_loadu_ps(&in_1[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_in_2{ _mm256_loadu_ps(&in_2[i]) };
			const __m256 avx_result{ _mm256_add_ps(avx_in_1, avx_in_2) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in_1[i] + in_2[i];
		}
	}

	inline __m256 scalar_reg{}; // preload scalar register

	inline void set_scalar(const float_s value) {
		scalar_reg = _mm256_set1_ps(value);
	}

	inline void vec_mult_float_s(const float_s* const in, float_s* const out, const int len) {
		// if standalone
		int i{ 0 };
		for (; i < len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in{ _mm256_loadu_ps(&in[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_result{ _mm256_mul_ps(avx_in, scalar_reg) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in[i] * scalar_reg.m256_f32[0];
		}
	}

	inline void vec_mult_float_s(const float_s* const in, float_s* const out, const float_s scalar, const int len) {
		// if standalone
		const __m256 scalar_reg_{ _mm256_set1_ps(scalar) };
		int i{ 0 };
		for (; i < len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in{ _mm256_loadu_ps(&in[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_result{ _mm256_mul_ps(avx_in, scalar_reg_) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in[i] * scalar;
		}
	}

	// multiplies in_1 by scalar, adds to in_2
	inline void vec_mult_add_float_s(const float* const in_1, const float* const in_2, float_s* const out, const float_s in_1_scalar, const int len) { // must be signed int here, otherwise "len - 8" breaks
		const __m256 scalar_reg_{ _mm256_set1_ps(in_1_scalar) };
		int i{ 0 };
		for (; i < len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in_1{ _mm256_loadu_ps(&in_1[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_in_2{ _mm256_loadu_ps(&in_2[i]) };
			const __m256 avx_result{ _mm256_add_ps(_mm256_mul_ps(avx_in_1, scalar_reg_), avx_in_2) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in_1[i] * in_1_scalar + in_2[i];
		}
	}
}