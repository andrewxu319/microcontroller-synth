#include "sound_engine.h"

#include <cstdio>  

using namespace standalone;

Master SoundEngine::master{};

SoundEngine::SoundEngine() {
	BufferLoaderData data{};
	PaStream* stream{};
	pa_init();
}

void SoundEngine::pa_init() {
	PaError error;

	error = Pa_Initialize();
	pa_check_error(error);

	error = Pa_OpenDefaultStream(
		&stream,
		0,
		Config::channels,
		paFloat32,
		Config::sample_rate,
		Config::actual_buffer_size,
        &SoundEngine::load_buffer,
        &data // communicate with load_buffer through this data structure. avoid sharing complex data structures that can be easily corrupted. avoid locks
	);
}

SoundEngine::~SoundEngine() {
	PaError error;

	error = Pa_StopStream(stream);
	pa_check_error(error);

	error = Pa_CloseStream(stream);
	pa_check_error(error);

	error = Pa_Terminate();
	pa_check_error(error);
}

void const SoundEngine::pa_check_error(const PaError& error) const {
	if (error != paNoError) {
		printf("PortAudio error: %s\n", Pa_GetErrorText(error));
	}
}

void SoundEngine::start_stream() {
	const PaError error { Pa_StartStream(stream) };
	pa_check_error(error);
}

int SoundEngine::load_buffer(
    const void* in_buf_,
    void* out_buf_,
    unsigned long buffer_size,
    const PaStreamCallbackTimeInfo* time_info,
    PaStreamCallbackFlags status_flags,
    void* data_
) {
	BufferLoaderData* data = (BufferLoaderData*)data_;

	float* out_buf{ (float*)out_buf_ };
	master.out_buf = out_buf;

	master.inputs[0]->generate_buf();
	master.generate_buf();

    return 0;
}