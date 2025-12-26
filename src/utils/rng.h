#pragma once

#include "includes.h"
#include "utils.h"

#include <random>

namespace utils {
	inline random_device rd{};
	inline seed_seq ss{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
	inline mt19937 rng_engine{ ss };

	template <typename T>
	inline T rng_uniform(const T min, const T max) {
		return uniform_real_distribution<T>{min, max}(rng_engine);
	}

	template <typename T>
	inline T rng_normal(const T mean, const T stdev) {
		return normal_distribution<T>{mean, stdev}(rng_engine);
	}
}