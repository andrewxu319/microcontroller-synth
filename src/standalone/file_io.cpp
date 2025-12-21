#include "file_io.h"

#include "utils/accelerator.h"

#include <AudioFile/AudioFile.h>

#include <fstream>
#include <cstdio>
#include <iterator>
#include <algorithm>

void standalone::file_io::read_wav(const string& path, float_s (&dest)[config::wavetable_resolution]) {
	AudioFile<float_s> file{};
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

	copy(begin(file.samples[0]), end(file.samples[0]), begin(dest)); // [0] because mono wavetables only. 1 channel

	const float_s max{ *max_element(begin(dest), end(dest)) };
	if (max != 1.0f) {
		accelerator::vec_scal_mult_float_s(dest, dest, 1 / max, config::buffer_size);
	}
}