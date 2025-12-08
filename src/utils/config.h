#pragma once

#include "utils/includes.h"

#include <string>

#define STANDALONE

namespace config {
	//constexpr float master_vol{ 0.3 };
	constexpr int channels{ 2 }; // 2 for stereo
	constexpr int sample_rate{ 44100 };
	constexpr int actual_buffer_size{ 256 };
	constexpr int buffer_size{ actual_buffer_size * channels }; // doubled if stereo
	constexpr int num_voices{ 2 };

	const string wavetable_path{ "resources\\wavetables\\32_bit\\" };
	constexpr int wavetable_resolution{ 2048 };

	constexpr char midi_port{ 0 };
}