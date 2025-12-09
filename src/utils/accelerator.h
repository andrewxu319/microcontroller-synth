#pragma once

#include <immintrin.h>

namespace accelerator {
	inline void vec_add_float32_t(const float32_t* const in_1, const float32_t* const in_2, float32_t* const out, const size_t len) {
		// if standalone
		for (size_t i = 0; i < len; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_1{ _mm256_loadu_ps(&in_1[i]) }; // change type for int or double if needed. use conditional_t
			const __m256 avx_2{ _mm256_loadu_ps(&in_2[i]) };
			const __m256 avx_result{ _mm256_add_ps(avx_1, avx_2) };
			_mm256_storeu_ps(&out[i], avx_result);
		}
	}
}