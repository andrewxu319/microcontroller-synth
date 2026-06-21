#ifdef TEENSY
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

namespace teensy {
class SoundEngine {
	public:
		SoundEngine(Master& master);
		void start_stream();
		void load_buffer();
		void close();

	private:
		Master& master;
		AudioPlayQueue queue;
		AudioOutputI2S i2s;
		AudioConnection patch_chord_1;
		AudioConnection patch_chord_2;
		AudioControlSGTL5000 sgtl5000;
		int16_t* queue_buf;
	};
}
#endif