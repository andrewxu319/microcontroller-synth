#pragma once

#include "utils/includes.h"
#include "utils/utils.h"
#include "utils/config.h"

#include <string>

namespace standalone {
	namespace file_io {
		void read_wav(const string& path, float_s(&dest)[config::waveform_resolution]);
	}
}