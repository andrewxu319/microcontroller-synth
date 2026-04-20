#include "accelerator.h"

#ifdef TEENSY
#include <arm_math.h>

namespace accelerator {
	// in_1 cannot be the same as out. in_2 can
	void vec_add_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, const int len) {
		arm_add_f32(const_cast<float_s* __restrict const>(in_1), const_cast<float_s* const>(in_2), out, len);
	}

	void vec_scal_mult_float_s(const float_s* const in, float_s* const out, float_s scalar, const int len) {
		arm_scale_f32(const_cast<float_s* const>(in), scalar, out, len);
	}

	void vec_scal_mult_float_s_to_int16(const float_s* const in, int16_t* const out, float_s scalar, const int len) {
		// ONLY WORKS IF LEN == BUFFER_SIZE
		arm_scale_f32(const_cast<float_s* const>(in), scalar, temp_buf, len);
		arm_float_to_q15(temp_buf, out, len);
	}

	void vec_scal_add_float_s(const float_s* const in, float_s* const out, float_s scalar, const int len) {
		arm_offset_f32(const_cast<float_s* const>(in), scalar, out, len);
	}

	// in_1 cannot be the same as out. in_2 can
	void vec_entrywise_mult_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, const int len) {
		arm_mult_f32(const_cast<float_s* __restrict const>(in_1), const_cast<float_s* const>(in_2), out, len);
	}

	// multiplies in_1 by scalar, adds to in_2
	void vec_mult_add_float_s(const float_s* const in_1, const float_s* const in_2, float_s* const out, float_s in_1_scalar, const int len) { // must be signed int here, otherwise "len - 8" breaks
		arm_scale_f32(const_cast<float_s* __restrict const>(in_1), in_1_scalar, out, len);
		arm_add_f32(out, const_cast<float_s* const>(in_2), out, len);
	}
}
#else
#include <immintrin.h>

namespace accelerator {
	// in_1 cannot be the same as out. in_2 can
	void vec_add_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, const int len) {
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

	void vec_scal_mult_float_s(const float_s* const in, float_s* const out, float_s scalar, const int len) {
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

	void vec_scal_add_float_s(const float_s* const in, float_s* const out, float_s scalar, const int len) {
		// if standalone
		const __m256 scalar_reg_{ _mm256_set1_ps(scalar) };
		int i{ 0 };
		for (; i < len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in{ _mm256_loadu_ps(&in[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_result{ _mm256_add_ps(avx_in, scalar_reg_) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in[i] + scalar;
		}
	}

	// in_1 cannot be the same as out. in_2 can
	void vec_entrywise_mult_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, const int len) {
		// if standalone
		int i{ 0 };
		for (; i < len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in_1{ _mm256_loadu_ps(&in_1[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_in_2{ _mm256_loadu_ps(&in_2[i]) };
			const __m256 avx_result{ _mm256_mul_ps(avx_in_1, avx_in_2) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in_1[i] * in_2[i];
		}
	}

	// multiplies in_1 by scalar, adds to in_2
	void vec_mult_add_float_s(const float_s* const in_1, const float_s* const in_2, float_s* const out, float_s in_1_scalar, const int len) { // must be signed int here, otherwise "len - 8" breaks
		const __m256 scalar_reg_{ _mm256_set1_ps(in_1_scalar) };
		int i{ 0 };
		for (; i < len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in_1{ _mm256_loadu_ps(&in_1[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_in_2{ _mm256_loadu_ps(&in_2[i]) };
			const __m256 avx_result{ _mm256_fmadd_ps(avx_in_1, scalar_reg_, avx_in_2) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in_1[i] * in_1_scalar + in_2[i];
		}
	}
}
#endif