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
	constexpr double latency{ 0.015 }; // seconds. now sure why but this is the lowest it'll go without sounding weird
	constexpr int num_voices{ 4 };

	const string wavetable_path{ "resources\\wavetables\\32_bit\\" };
	constexpr int wavetable_resolution{ 2048 };

	constexpr uint8_t audio_device{ 0 }; // todo: check for available devices
	constexpr uint8_t midi_port{ 0 };
}