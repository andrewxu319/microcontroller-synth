#ifndef TEENSY
#pragma once

#include "utils/global.h"
#include "utils/config.h"

#include <string>

namespace standalone {
	namespace file_io {
		void read_wav(const std::string& path, float_s(&dest)[config::waveform_resolution]);
	}
}
#endif