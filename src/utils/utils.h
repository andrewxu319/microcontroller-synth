#pragma once

#include <chrono>
#include <cstdio>
#include <cassert>
#include <stdexcept>

using float_s = float; // sample format

namespace utils {
	template <typename T, size_t L>
	struct array_wrapper {
		T data[L];
	};

	namespace timer {
		static std::chrono::steady_clock::time_point start_time{};
		static long long count{ 0 };
		static long long total{ 0 };

		inline void start() {
			start_time = std::chrono::high_resolution_clock::now();
		}

		inline void end(const string_view timer_name) {
			if (count < 100) {
				count++;
				return;
			}
			if (count < 2100) {
				auto stop = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_time);
				count++;
				total += duration.count();
				//printf("%ld microseconds. count: %d\n", static_cast<long>(duration.count()), count);
			}
			if (count == 2100) {
				printf("%s average: %f\n", timer_name.data(), static_cast<double>(total) / 2000);
				count++;
			}
		}
	}
}