#pragma once

#include "utils/config.h"
#include "synthesis/modules/master.h"

#include <portaudio/portaudio.h>

#include <cstdint>

namespace standalone {
	class SoundEngine {
	public:
		SoundEngine();
	private:
		typedef struct
		{
			;
		}
		BufferLoaderData;

		BufferLoaderData data{};
		static Master master;

		void pa_init();
		void pa_check_error(const PaError& error);
		static int load_buffer(
			const void* in_buf_,
			void* out_buf_,
			unsigned long buffer_size,
			const PaStreamCallbackTimeInfo* time_info,
			PaStreamCallbackFlags status_flags,
			void* data_
		);
	};

}