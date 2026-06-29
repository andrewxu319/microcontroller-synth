#ifndef TEENSY
#pragma once

#include "utils/config.h"
#include "midi/message.h"

#include <atomic>
#include <portaudio/portaudio.h>

namespace standalone {
	class SoundEngine {
	public:
		SoundEngine(std::atomic<float_s*>& out_buf, std::atomic<uint32_t>& buffer_counter, std::atomic<bool>& buffer_ready);
		void close();
		void start_stream();

		void pa_check_error(const PaError& error);
		static int callback(
			const void* __restrict in_buf,
			void* __restrict out_buf,
			unsigned long buffer_size,
			const PaStreamCallbackTimeInfo* time_info,
			PaStreamCallbackFlags status_flags,
			void* __restrict this_ptr
		);

	private:
		PaStream* stream;
		std::atomic<float_s*>& out_buf_;
		std::atomic<uint32_t>& buffer_counter_;
		std::atomic<bool>& buffer_ready_;
	};
}
#endif