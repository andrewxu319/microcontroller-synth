#pragma once

#include "utils/global.h"

#include <portaudio/portaudio.h>
#include <string>

namespace config {
	constexpr PaHostApiTypeId host_api{ paWASAPI };
	constexpr uint8_t channels{ 1 }; // 2 for stereo. keep it 1 for now. to make it stereo we should make a buf struct
	inline int sample_rate{ 44100 };
	constexpr int channel_buffer_size{ 512 };
	constexpr int buffer_size{ channel_buffer_size * channels }; // doubled if stereo
	constexpr double latency{ 0.005 }; // seconds. now sure why but this is the lowest it'll go without sounding weird
	constexpr uint8_t control_rate{ 8 };
	constexpr uint8_t num_voices{ 1 };
	//constexpr float master_vol{ 0.3 };

	inline std::string waveform_path{ std::string("resources\\waveforms\\32_bit\\") + std::to_string(sample_rate) + std::string("\\") };
	constexpr int waveform_resolution{ 2048 };

	constexpr uint8_t midi_port{ 0 };
}