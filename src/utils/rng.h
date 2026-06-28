#pragma once

#include "global.h"

#include <random>

namespace utils {
	inline std::random_device rd{};
	inline std::seed_seq ss{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
	inline std::mt19937 rng_engine{ ss };

	template <typename T>
	inline T rng_uniform(const T min, const T max) {
		return std::uniform_real_distribution<T>{min, max}(rng_engine);
	}
	
	// inclusive
	template <typename T>
	inline T rng_uniform_int(const T min, const T max) {
		return std::uniform_int_distribution<T>{min, max}(rng_engine);
	}

	template <typename T>
	inline T rng_normal(const T mean, const T stdev) {
		return std::normal_distribution<T>{mean, stdev}(rng_engine);
	}
}