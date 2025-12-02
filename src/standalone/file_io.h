#pragma once

#include "utils/typedefs.h"
#include "utils/config.h"

#include <string>

namespace standalone {
	namespace file_io {
		void read_wav(const std::string& path, float32_t(&dest)[config::wavetable_resolution]);
	}
}