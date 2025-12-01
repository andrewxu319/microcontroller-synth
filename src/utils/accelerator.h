#pragma once

#include <array>
#include <immintrin.h>

#include <chrono>
using namespace std::chrono;

namespace accelerator {
	inline void vec_add_float(const float* const in_1, const float* const in_2, float* const out, const size_t len) {
		// if standalone

		//auto start = high_resolution_clock::now();

		for (size_t i = 0; i < len; i += 8) { // with regular avx, we can add 8 at once
			const __m256 avx_1{ _mm256_loadu_ps(&in_1[i]) }; // change type for int or double if needed. use std::conditional_t
			const __m256 avx_2{ _mm256_loadu_ps(&in_2[i]) };
			const __m256 avx_result{ _mm256_add_ps(avx_1, avx_2) };
			_mm256_storeu_ps(&out[i], avx_result);
		}

		//auto stop = high_resolution_clock::now();
		//auto duration = duration_cast<microseconds>(stop - start);
		//printf("%ld\n", duration.count());
	}
}