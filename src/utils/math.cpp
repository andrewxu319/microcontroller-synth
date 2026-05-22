#include "math.h"

#ifdef TEENSY
#include <arm_math.h>

namespace math {
	// in_1 cannot be the same as out. in_2 can
	void vec_add_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, size_t len) {
		arm_add_f32(const_cast<float_s* __restrict const>(in_1), const_cast<float_s* const>(in_2), out, len);
	}
	
	void vec_sub_float_s(const float_s* const in_1, const float_s* const in_2, float_s* const out, size_t len) {
		arm_sub_f32(const_cast<float_s* const>(in_1), const_cast<float_s* const>(in_2), out, len);
	}

	void vec_scal_mult_float_s(const float_s* const in, float_s* const out, float_s scalar, size_t len) {
		arm_scale_f32(const_cast<float_s* const>(in), scalar, out, len);
	}

	void vec_scal_mult_float_s_to_int16(const float_s* const in, int16_t* const out, float_s scalar, size_t len) {
		// ONLY WORKS IF LEN == BUFFER_SIZE
		arm_scale_f32(const_cast<float_s* const>(in), scalar, temp_buf, len);
		arm_float_to_q15(temp_buf, out, len);
	}

	void vec_scal_add_float_s(const float_s* const in, float_s* const out, float_s scalar, size_t len) {
		arm_offset_f32(const_cast<float_s* const>(in), scalar, out, len);
	}

	// in_1 cannot be the same as out. in_2 can
	void vec_entrywise_mult_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, size_t len) {
		arm_mult_f32(const_cast<float_s* __restrict const>(in_1), const_cast<float_s* const>(in_2), out, len);
	}

	// multiplies in_1 by scalar, adds to in_2
	void axpy(const float_s* const x, const float_s* const y, float_s* const out, float_s a, size_t len) { // must be signed int here, otherwise "len - 8" breaks
		arm_scale_f32(const_cast<float_s* __restrict const>(x), a, out, len);
		arm_add_f32(out, const_cast<float_s* const>(y), out, len);
	}

	void axpy(const float_s* const x, float y, float_s* const out, float_s a, size_t len) { // must be signed int here, otherwise "len - 8" breaks
		arm_scale_f32(const_cast<float_s* __restrict const>(x), a, out, len);
		arm_offset_f32(out, y, out, len);
	}
}
#else
#include <immintrin.h>
#include <cmath>

namespace math {
	// in_1 cannot be the same as out. in_2 can
	void vec_add_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, size_t len) {
		// if standalone
		size_t i{ 0 };
		for (; i <= len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in_1{ _mm256_loadu_ps(&in_1[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_in_2{ _mm256_loadu_ps(&in_2[i]) };
			const __m256 avx_result{ _mm256_add_ps(avx_in_1, avx_in_2) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in_1[i] + in_2[i];
		}
	}
	
	void vec_sub_float_s(const float_s* const in_1, const float_s* const in_2, float_s* const out, size_t len) {
		// if standalone
		size_t i{ 0 };
		for (; i <= len - 8; i += 8) { // with regular avx, we can sub 8 at once
			const __m256 avx_in_1{ _mm256_loadu_ps(&in_1[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_in_2{ _mm256_loadu_ps(&in_2[i]) };
			const __m256 avx_result{ _mm256_sub_ps(avx_in_1, avx_in_2) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in_1[i] - in_2[i];
		}
	}

	void vec_scal_mult_float_s(const float_s* const in, float_s* const out, float_s scalar, size_t len) {
		// if standalone
		const __m256 scalar_reg_{ _mm256_set1_ps(scalar) };
		size_t i{ 0 };
		for (; i <= len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in{ _mm256_loadu_ps(&in[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_result{ _mm256_mul_ps(avx_in, scalar_reg_) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in[i] * scalar;
		}
	}

	void vec_scal_add_float_s(const float_s* const in, float_s* const out, float_s scalar, size_t len) {
		// if standalone
		const __m256 scalar_reg_{ _mm256_set1_ps(scalar) };
		size_t i{ 0 };
		for (; i <= len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in{ _mm256_loadu_ps(&in[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_result{ _mm256_add_ps(avx_in, scalar_reg_) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = in[i] + scalar;
		}
	}

	// in_1 cannot be the same as out. in_2 can
	void vec_entrywise_mult_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, size_t len) {
		// if standalone
		size_t i{ 0 };
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

	// multiplies x by a, adds to y
	void axpy(const float_s* const x, const float_s* const y, float_s* const out, float_s a, size_t len) { // must be signed int here, otherwise "len - 8" breaks
		const __m256 avx_a{ _mm256_set1_ps(a) };
		size_t i{ 0 };
		for (; i <= len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_x{ _mm256_loadu_ps(&x[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_y{ _mm256_loadu_ps(&y[i]) };
			const __m256 avx_result{ _mm256_fmadd_ps(avx_x, avx_a, avx_y) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = x[i] * a + y[i];
		}
	}

	void axpy(const float_s* const x, float_s y, float_s* const out, float_s a, size_t len) { // must be signed int here, otherwise "len - 8" breaks
		const __m256 avx_a{ _mm256_set1_ps(a) };
		const __m256 avx_y{ _mm256_set1_ps(y) };
		size_t i{ 0 };
		for (; i <= len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_x{ _mm256_loadu_ps(&x[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_result{ _mm256_fmadd_ps(avx_x, avx_a, avx_y) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = x[i] * a + y;
		}
	}

	void abs(const float_s* const in, float_s* const out, size_t len) {
		const __m256 SIGN_BIT_MASK{ _mm256_set1_ps(-0.0f) };
		size_t i{ 0 };
		for (; i <= len - 8; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_in{ _mm256_loadu_ps(&in[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_result{ _mm256_andnot_ps(avx_in, SIGN_BIT_MASK) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
		for (; i < len; i++) {
			out[i] = std::abs(in[i]);
		}
	}

	/* len = 8
	* 0x00: all less than
	* 0xFF: all greater than
	* other: some greater, some less
	*/
	int compare_all(const float_s* __restrict const in, float_s threshold) {
		const __m256 thresh_reg{ _mm256_set1_ps(threshold) };
		const __m256 avx_in{ _mm256_loadu_ps(in) };
		const __m256 avx_result{ _mm256_cmp_ps(avx_in, thresh_reg, _CMP_GT_OQ ) };
		int cumulative_result{ _mm256_movemask_ps(avx_result) };
		return cumulative_result;
	}
}
#endif