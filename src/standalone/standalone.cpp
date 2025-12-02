#include "sound_engine.h"
#include "synthesis/modules/oscillator.h"
#include "file_io.h"

#include <cstdio>

using namespace standalone;

int main() {
	SoundEngine sound_engine{};

	printf("ok");
	Oscillator osc{};
	osc.set_freq(800);
	file_io::read_wav("resources\\wavetables\\32_bit\\sawtooth.wav", osc.wavetable);

	sound_engine.master.add_input(&osc);
	osc.add_output(&sound_engine.master);

	sound_engine.start_stream();

	Pa_Sleep(3000);

	return 0;
}