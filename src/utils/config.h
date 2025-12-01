#pragma once

namespace Config {
	constexpr int channels{ 2 }; // 2 for stereo
	constexpr int sample_rate{ 44100 };
	constexpr int actual_buffer_size{ 256 };
	constexpr int buffer_size{ actual_buffer_size * channels }; // doubled if stereo
}