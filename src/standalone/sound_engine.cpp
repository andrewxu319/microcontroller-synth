#include "sound_engine.h"

#include <cstdio>  

using namespace standalone;

Master SoundEngine::master{};

SoundEngine::SoundEngine() {
	pa_init();
}

void SoundEngine::pa_init() {
	PaError error;

	error = Pa_Initialize();
	pa_check_error(error);

	PaStream* stream;
	error = Pa_OpenDefaultStream(
		&stream,
		0,
		2,
		paFloat32,
		Config::sample_rate,
		Config::buffer_size,
        &SoundEngine::load_buffer,
        &data // communicate with load_buffer through this data structure. avoid sharing complex data structures that can be easily corrupted. avoid locks
	);

    error = Pa_StartStream(stream);
    pa_check_error(error);

    Pa_Sleep(3000);

    error = Pa_StopStream(stream);
    pa_check_error(error);

    error = Pa_CloseStream(stream);
    pa_check_error(error);

	error = Pa_Terminate();
	pa_check_error(error);
}

void SoundEngine::pa_check_error(const PaError& error) {
	if (error != paNoError) {
		printf("PortAudio error: %s\n", Pa_GetErrorText(error));
	}
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

	master.generate_buf();

	float* out_buf{ (float*)out_buf_ };
	std::copy(master.out_buf_to_sound_engine.begin(), master.out_buf_to_sound_engine.end(), out_buf);

    return 0;
}