#pragma once

#include "utils/includes.h"

#include <portaudio/portaudio.h>
#include <string>

#define STANDALONE

namespace config {
	constexpr PaHostApiTypeId host_api{ paWASAPI };
	constexpr int channels{ 1 }; // 2 for stereo. keep it 1 for now. to make it stereo we should make a buf struct
	inline int sample_rate{ 44100 };
	constexpr int actual_buffer_size{ 512 };
	constexpr int buffer_size{ actual_buffer_size * channels }; // doubled if stereo
	constexpr double latency{ 0.005 }; // seconds. now sure why but this is the lowest it'll go without sounding weird
	constexpr int num_voices{ 4 };
	//constexpr float master_vol{ 0.3 };

	inline string wavetable_path{ string("resources\\wavetables\\32_bit\\") + to_string(sample_rate) + string("\\") };
	constexpr int wavetable_resolution{ 2048 };

	constexpr uint8_t midi_port{ 0 };
}