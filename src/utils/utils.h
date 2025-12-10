#pragma once

#include <chrono>
#include <cstdio>

typedef float float_s; // sample format

namespace utils {
	enum NoBaseInit {
		NO_BASE_INIT
	};

	template <typename T, size_t L>
	struct array_wrapper {
		T data[L];
	};

	namespace timer {
		inline std::chrono::steady_clock::time_point start_time{};

		inline void start() {
			start_time = std::chrono::high_resolution_clock::now();
		}

		inline void end() {
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_time);
			printf("%ld microseconds\n", static_cast<long>(duration.count()));
		}
	}
}