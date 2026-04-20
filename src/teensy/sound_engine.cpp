#ifdef TEENSY
#include "sound_engine.h"

#include "utils/accelerator.h"

#include <queue>
#include <limits>

namespace teensy::sound_engine {
	AudioPlayQueue queue{};
	AudioOutputI2S i2s{};
	AudioConnection patch_chord_1{ queue, 0, i2s, 0 };
	AudioConnection patch_chord_2{ queue, 0, i2s, 1 };
	AudioControlSGTL5000 sgtl5000{};
	int16_t* queue_buf{};
	
	Master& master{ Master::instance() };
	float_s float_out_buf[config::buffer_size]{};

	void init() {
		AudioMemory(100);
		sgtl5000.enable();
		if (sgtl5000.volume(0.80) == false) printf("Error: Volume too low.\n"); // setting volume above around 0.8125 will cause clipping
		sgtl5000.lineOutLevel(31); // 1.44V peak to peak. LM386 takes in 0.8V peak to peak =>
		// queue.setBehaviour(AudioPlayQueue::NON_STALLING);

		master.out_buf = float_out_buf;
		printf("Teensy sound engine initialized.\n");
	}

	void load_buffer() {
		if (queue.available() == 0) return;
		// auto start{micros()};
		if (!(queue_buf = queue.getBuffer())) return;
		
		// Serial.print(" getbuffer. ");
		// Serial.println(micros() - start);
		
		// start = micros();
		master.generate_buf();
		// Serial.print(" gen_buf. ");
		// Serial.println(micros() - start);

		// IMPLEMENT CLIPPING LATER
		// accelerator::vec_scal_mult_float_s(float_out_buf, float_out_buf, 0.01, config::buffer_size);
		arm_float_to_q15(float_out_buf, queue_buf, config::buffer_size);
		// start = micros();
		queue.playBuffer();
		// Serial.print(" playbuffer. ");
		// Serial.println(micros() - start);

	}

	void close() {}
}
#endif