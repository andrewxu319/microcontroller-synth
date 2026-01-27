#pragma once

#include "utils/config.h"
#include "synthesis/modules/master.h"
#include "midi/message.h"

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

using namespace synthesis;

namespace teensy::sound_engine {
	typedef struct {} BufferLoaderData;

	extern Master& master;
	extern const BufferLoaderData data;
	extern PaStream* stream;

	void init();
	void sound_engine_close();
	void start_stream();

	void pa_check_error(const PaError& error);
	int load_buffer(
		const void* __restrict in_buf_,
		void* __restrict out_buf_,
		unsigned long buffer_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flags,
		void* __restrict data_
	);
}