#pragma once

#include "global.h"

#include <string_view>

namespace utils::timer {
	void start();

	void end(const std::string_view timer_name);
}