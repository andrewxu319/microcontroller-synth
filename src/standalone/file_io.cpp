#include "file_io.h"
#include "utils/typedefs.h"

#include <AudioFile/AudioFile.h>

#include <fstream>
#include <cstdio>
#include <iterator>

void standalone::file_io::read_wav(const std::string& path, float32_t (&dest)[config::wavetable_resolution]) {
	AudioFile<float32_t> file{};
	file.load(path);

	if (file.getSampleRate() != config::sample_rate) {
		printf("Wrong wavetable sample rate!\n");
	}
	if (file.getBitDepth() != 32) {
		printf("Wrong wavetable float type!\n");
	}
	if (file.getNumSamplesPerChannel() != config::wavetable_resolution) {
		printf("Wrong wavetable resolution!\n");
	}
	if (file.isStereo()) {
		printf("No stereo!\n");
	}

	for (int i = 0; i < config::wavetable_resolution; i++) {
		dest[i] = file.samples[0][i];
	}
}