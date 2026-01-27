#pragma once

#include "global.h"

namespace accelerator {
	void vec_add_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, const int len);
	void vec_scal_mult_float_s(const float_s* const in, float_s* const out, const float_s scalar, const int len);
	void vec_scal_add_float_s(const float_s* const in, float_s* const out, const float_s scalar, const int len);
	void vec_entrywise_mult_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, const int len);
	void vec_mult_add_float_s(const float* const in_1, const float* const in_2, float_s* const out, const float_s in_1_scalar, const int len);
}