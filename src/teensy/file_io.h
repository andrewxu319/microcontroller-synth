#ifdef TEENSY
#pragma once

#include "utils/global.h"
#include "utils/utils.h"
#include "utils/config.h"

#include <string>

namespace teensy::file_io {
	void read_wav(const std::string& path, float_s(&dest)[config::waveform_resolution]);
}
#endif