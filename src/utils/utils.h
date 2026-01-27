#pragma once

#include "global.h"

#include <string_view>

namespace utils {
	template <typename T, size_t L>
	struct array_wrapper {
		T data[L];
	};

	namespace timer {
		void start();

		void end(const string_view timer_name);
	}
}