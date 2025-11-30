#pragma once

#include "config.h"

#include <array>

typedef std::array<float, Config::buffer_size> buffer;

enum NoBaseInit
{
	NO_BASE_INIT
};