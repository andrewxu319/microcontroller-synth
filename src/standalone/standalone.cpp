#include "sound_engine.h"
#include "synthesis/modules/oscillator.h"

#include <cstdio>
#include <vector>

using namespace standalone;

int main() {
	SoundEngine sound_engine{};

	printf("ok");
	Oscillator osc{};
	osc.freq = 400;
	sound_engine.master.add_input(&osc);
	osc.add_output(&sound_engine.master);

	sound_engine.start_stream();

	Pa_Sleep(3000);

	return 0;
}