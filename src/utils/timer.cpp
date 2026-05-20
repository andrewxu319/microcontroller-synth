#include "timer.h"

#include <cstdio>

#ifdef TEENSY
    #include <Arduino.h>
#else
	#include <chrono>
#endif

namespace utils::timer {
#ifdef TEENSY
    static uint32_t start_time;

    void start() {
        start_time = micros();
    }
    void end(const std::string_view timer_name) {
        printf("Timer %s: %d\n", timer_name.data(), micros() - start_time);
    }

#else
    static std::chrono::high_resolution_clock::time_point start_time{};

    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void end(const std::string_view timer_name) {
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start_time);
        // printf("timer name: %s. %ld nanoseconds\n", timer_name.data(), static_cast<long>(duration.count()));
        printf("%ld\n", static_cast<long>(duration.count()));
    }
#endif
}