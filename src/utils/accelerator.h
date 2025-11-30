#pragma once

#include <array>

namespace accelerator {
	template <typename T, size_t L>
	void vec_add(const std::array<T, L>* const in_1, const std::array<T, L>* const in_2, std::array<T, L>* const out) {
		// if standalone

		// SLOW IMPLEMENTATION. do more later
		// caller needs to fill out with 0
		for (size_t i = 0; i < L; i++) {
			(*out)[i] += (*in_1)[i] + (*in_2)[i];
		}
	}
}