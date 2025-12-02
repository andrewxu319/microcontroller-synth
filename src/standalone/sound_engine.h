#pragma once

#include "utils/config.h"
#include "synthesis/modules/master.h"

#include <portaudio/portaudio.h>

namespace standalone {
	class SoundEngine {
	public:
		static Master master;

		SoundEngine();
		~SoundEngine();
		void start_stream();
	private:
		typedef struct
		{
			;
		}
		BufferLoaderData;

		BufferLoaderData data;
		PaStream* stream;

		void pa_init();
		void const pa_check_error(const PaError& error) const;
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