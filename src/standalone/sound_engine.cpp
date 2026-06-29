#ifndef TEENSY
#include "sound_engine.h"

#include <chrono>
#include <cassert>

using namespace standalone;

void SoundEngine::pa_check_error(const PaError& error) {
	if (error != paNoError) {
		printf("PortAudio error: %s\n", Pa_GetErrorText(error));
	}
}

SoundEngine::SoundEngine(std::atomic<float_s*>& out_buf, std::atomic<uint32_t>& buffer_counter, std::atomic<bool>& buffer_ready)
	: out_buf_{ out_buf },
	buffer_counter_{ buffer_counter },
	buffer_ready_{ buffer_ready },
	stream{}
{
	PaError error;

	error = Pa_Initialize();
	pa_check_error(error);

	PaDeviceIndex device{ Pa_GetDefaultOutputDevice() };
	for (PaDeviceIndex i{ 0 }; i < Pa_GetDeviceCount(); i++)
	{
		printf("device %d: type %s, name %s\n", i, Pa_GetHostApiInfo(Pa_GetDeviceInfo(i)->hostApi)->name, Pa_GetDeviceInfo(i)->name);
		//if (Pa_GetHostApiInfo(Pa_GetDeviceInfo(i)->hostApi)->type == paWASAPI) {
		//	device = i;
		//}
	}
	device = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(config::host_api))->defaultOutputDevice;
	//device = 14;
	const PaDeviceInfo* device_info{ Pa_GetDeviceInfo(device) };
	config::sample_rate = static_cast<int>(device_info->defaultSampleRate);
	config::waveform_path = std::string("resources\\waveforms\\32_bit\\") + std::to_string(config::sample_rate) + std::string("\\");
	printf("Using audio device %s: index %d, host API %d, max channels %d, default latency %f, sample rate %d\n",
		device_info->name,
		device,
		Pa_GetHostApiInfo(device_info->hostApi)->type,
		device_info->maxOutputChannels,
		device_info->defaultLowOutputLatency,
		device_info->defaultSampleRate
	);

	const PaStreamParameters output_parameters{
		device, // config::audio_device
		config::channels,
		paFloat32, // make switch case or something

		config::latency
	};
	error = Pa_OpenStream(
		&stream,
		nullptr,
		&output_parameters,
		device_info->defaultSampleRate,
		config::channel_buffer_size,
		0, // flags. clipping on by default
		&SoundEngine::callback,
		static_cast<void*>(this) // communicate with load_buffer through this data structure. avoid sharing complex data structures that can be easily corrupted. avoid locks
	);
}

int SoundEngine::callback(
	const void* __restrict in_buf,
	void* __restrict out_buf,
	unsigned long buffer_size,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flags,
	void* __restrict this_ptr
) {
#ifdef _DEBUG
	if (status_flags) {
		printf("PortAudio status %x\n", status_flags);
	}
#endif

	SoundEngine* this_ptr_{ static_cast<SoundEngine*>(this_ptr) };

	if (!this_ptr_->buffer_ready_.load(std::memory_order_acquire)) {
		printf("Buffer underflow!\n");
	}

	this_ptr_->out_buf_.store(static_cast<float_s*>(out_buf), std::memory_order_release);
	this_ptr_->buffer_counter_.fetch_add(1, std::memory_order_release);
	this_ptr_->buffer_counter_.notify_one();

	return 0;
}

void SoundEngine::close() {
	PaError error;

	error = Pa_StopStream(stream);
	pa_check_error(error);

	error = Pa_CloseStream(stream);
	pa_check_error(error);

	error = Pa_Terminate();
	pa_check_error(error);
}

void SoundEngine::start_stream() {
	const PaError error{ Pa_StartStream(stream) };
	pa_check_error(error);
	Pa_Sleep(50);
}
#endif