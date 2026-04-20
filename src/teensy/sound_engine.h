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

namespace teensy::sound_engine {
	extern Master& master;

	void init();
	void start_stream();
	void load_buffer();
	void close();
}
#endif