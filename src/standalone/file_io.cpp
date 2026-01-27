#include "file_io.h"

#include "utils/accelerator.h"

#include <AudioFile/AudioFile.h>

#include <fstream>
#include <cstdio>
#include <iterator>
#include <algorithm>

void standalone::file_io::read_wav(const std::string& path, float_s (&dest)[config::waveform_resolution]) {
	AudioFile<float_s> file{};
	file.load(path);

	if (file.getSampleRate() != config::sample_rate) {
		printf("Wrong waveform sample rate!\n");
	}
	if (file.getBitDepth() != 32) {
		printf("Wrong waveform float type!\n");
	}
	if (file.getNumSamplesPerChannel() != config::waveform_resolution) {
		printf("Wrong waveform resolution!\n");
	}
	if (file.isStereo()) {
		printf("No stereo!\n");
	}

	std::copy(std::begin(file.samples[0]), std::end(file.samples[0]), std::begin(dest)); // [0] because mono waveforms only. 1 channel

	const float_s max{ *std::max_element(std::begin(dest), std::end(dest)) };
	if (max != 1.0f) {
		accelerator::vec_scal_mult_float_s(dest, dest, 1 / max, config::buffer_size);
	}
}