#pragma once

#include "global.h"
#include "config.h"

namespace accelerator {
	void vec_add_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, const int len);
	void vec_scal_mult_float_s(const float_s* const in, float_s* const out, float_s scalar, const int len);
	#ifdef TEENSY
		static float_s temp_buf[config::buffer_size]{};
		void vec_scal_mult_float_s_to_int16(const float_s* const in, int16_t* const out, float_s scalar, const int len);
	#endif
	void vec_scal_add_float_s(const float_s* const in, float_s* const out, float_s scalar, const int len);
	void vec_entrywise_mult_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, const int len);
	void vec_mult_add_float_s(const float_s* const in_1, const float_s* const in_2, float_s* const out, float_s in_1_scalar, const int len);
}