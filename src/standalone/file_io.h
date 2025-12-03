#pragma once

#include "utils/includes.h"
#include "utils/typedefs.h"
#include "utils/config.h"

#include <string>

namespace standalone {
	namespace file_io {
		void read_wav(const string& path, float32_t(&dest)[config::wavetable_resolution]);
	}
}