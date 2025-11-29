#include "sound_engine.h"

#include <math.h>
#include <iostream>

#include <thread> // Required for std::this_thread::sleep_for
#include <chrono>

SoundEngine::SoundEngine() {
	BASS_Init(
		-1,
		Config::sample_rate,
		Config::channels == 1 ? BASS_DEVICE_MONO : 0,
		0,
		NULL
	);

	HSTREAM stream{};
	stream = BASS_StreamCreate(
		Config::sample_rate,
		Config::channels,
		0,
		(STREAMPROC*)get_next_buffer,
		0
	);
	BASS_ChannelSetAttribute(stream, BASS_ATTRIB_BUFFER, 0); // change if needed
	BASS_ChannelPlay(stream, FALSE);

	while (true) {
		;
	}
}

DWORD CALLBACK SoundEngine::get_next_buffer(HSTREAM handle, int16_t* buffer, DWORD length, void* user) {
	int a;
	for (a = 0; a < Config::buffer_size; a++) {
		buffer[a] = (int16_t)(32767.0 * sin(a * 6.283185 / 64)); // sine wave
	}

	return length;
}

void SoundEngine::play_buffer(int16_t* buffer) {
	HSAMPLE sample = BASS_SampleCreate(
		Config::buffer_size,
		Config::sample_rate,
		Config::channels,
		1,
		0
	); // create sample
	int16_t data[128]; // data buffer
	int a;
	for (a = 0; a < 128; a++) {
		data[a] = (int16_t)(32767.0 * sin(a * 6.283185 / 64)); // sine wave
	}
	BASS_SampleSetData(sample, data); // set the sample's data
}

SoundEngine::~SoundEngine() {
	BASS_Free();
}