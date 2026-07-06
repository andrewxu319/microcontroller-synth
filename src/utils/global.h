#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <vector>
#include <memory>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

using float_s = float; // sample format

constexpr double M_PI{ 3.14159265358979323846 };