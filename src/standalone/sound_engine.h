#ifndef TEENSY
#pragma once

#include "utils/config.h"
#include "synthesis/modules/master.h"
#include "midi/message.h"

#include <portaudio/portaudio.h>

using namespace synthesis;

namespace standalone {
	class SoundEngine {
	public:
		SoundEngine(Master& master);
		void close();
		void start_stream();

		void pa_check_error(const PaError& error);
		static int load_buffer(
			const void* __restrict in_buf_,
			void* __restrict out_buf_,
			unsigned long buffer_size,
			const PaStreamCallbackTimeInfo* time_info,
			PaStreamCallbackFlags status_flags,
			void* __restrict this_ptr
		);

	private:
		Master& master_;
		PaStream* stream;
	};
}
#endif