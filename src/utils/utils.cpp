#include "utils.h"

#include <cstdio>

#ifdef TEENSY
    #include <Arduino.h>
#else
	#include <chrono>
#endif

using namespace utils::timer;

#ifdef TEENSY
static uint32_t start_time;

void start() {
    start_time = micros();
}
void end(const std::string_view timer_name) {
    printf("Timer %s: %d\n", timer_name, micros() - start_time);
}

#else
static std::chrono::steady_clock::time_point start_time{};
static long long count{ 0 };
static long long total{ 0 };

void start() {
    start_time = std::chrono::high_resolution_clock::now();
}

void end(const std::string_view timer_name) {
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
#endif