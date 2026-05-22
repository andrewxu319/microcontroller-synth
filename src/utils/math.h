#pragma once

#include "global.h"
#include "config.h"

namespace math {
	void vec_add_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, size_t len);
	void vec_sub_float_s(const float_s* const in_1, const float_s* const in_2, float_s* const out, size_t len);
	void vec_scal_mult_float_s(const float_s* const in, float_s* const out, float_s scalar, size_t len);
	#ifdef TEENSY
		static float_s temp_buf[config::buffer_size]{};
		void vec_scal_mult_float_s_to_int16(const float_s* const in, int16_t* const out, float_s scalar, size_t len);
	#endif
	void vec_scal_add_float_s(const float_s* const in, float_s* const out, float_s scalar, size_t len);
	void vec_entrywise_mult_float_s(const float_s* __restrict const in_1, const float_s* const in_2, float_s* const out, size_t len);
	void axpy(const float_s* const x, const float_s* const y, float_s* const out, float_s a, size_t len);
	void axpy(const float_s* const x, float_s y, float_s* const out, float_s a, size_t len);
	void abs(const float_s* const in, float_s* const out, size_t len);
	int compare_all(const float_s* __restrict const in, float_s threshold);
}