#pragma once

#include "utils/global.h"

#include <string>
#ifdef TEENSY
	#include <MIDI.h>
#else
	#include <portaudio/portaudio.h>
#endif

namespace config {
#ifndef TEENSY
	constexpr PaHostApiTypeId host_api{ paWASAPI };
#endif
	constexpr uint8_t channels{ 1 }; // 2 for stereo. keep it 1 for now. to make it stereo we should make a buf struct
#ifdef TEENSY
	inline int sample_rate{ 44100 };
	constexpr int channel_buffer_size{ 128 };
	constexpr int buffer_size{ channel_buffer_size * channels }; // doubled if stereo
#else
	inline int sample_rate{ 48000 };
	constexpr int channel_buffer_size{ 512 };
	constexpr int buffer_size{ channel_buffer_size * channels }; // doubled if stereo
	constexpr double latency{ 0.005 }; // seconds. now sure why but this is the lowest it'll go without sounding weird
#endif
	constexpr uint8_t control_rate{ 8 };
	constexpr uint8_t num_voices{ 64 };
	//constexpr float master_vol{ 0.3 };

	inline std::string waveform_path{ std::string("resources\\waveforms\\32_bit\\") + std::to_string(sample_rate) + std::string("\\") };
	constexpr int waveform_resolution{ 2048 };
	constexpr uint8_t midi_port{ 0 }; // 1 for external midi controller (temp)

#ifdef TEENSY
	constexpr int teensy_baud{};
	constexpr int serial_monitor_baud{};
	inline HardwareSerialIMXRT& teensy_serial{ Serial6 };
#endif
}