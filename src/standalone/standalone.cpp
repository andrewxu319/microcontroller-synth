#include "sound_engine.h"

#include <iostream>

namespace Standalone {};

int main() {
	std::cout << "ok";

	short buffer[Config::buffer_size]{};

	SoundEngine sound_engine{};
	//sound_engine.play_buffer(buffer);

	return 0;
}