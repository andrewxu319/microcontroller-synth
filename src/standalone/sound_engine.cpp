#include "sound_engine.h"

#include <queue>

namespace standalone::sound_engine {
	Master& master{ Master::instance() };
	const BufferLoaderData data{};
	PaStream* stream{};

	void pa_check_error(const PaError& error) {
		if (error != paNoError) {
			printf("PortAudio error: %s\n", Pa_GetErrorText(error));
		}
	}

	int load_buffer(
		const void* __restrict in_buf_,
		void* __restrict out_buf_,
		unsigned long buffer_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flags,
		void* __restrict data_
	) {
//		if (status_flags) {
//			printf("%x\n", status_flags);
//		}

		//utils::timer::start();
		BufferLoaderData* data = (BufferLoaderData*)data_;
		float_s* out_buf{ (float_s*)out_buf_ };
		master.out_buf = out_buf;
		master.generate_buf();
		//utils::timer::end();

		return 0;
	}

	void init()
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
		device = 14;
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
			&load_buffer,
			(void*)(&data) // communicate with load_buffer through this data structure. avoid sharing complex data structures that can be easily corrupted. avoid locks
		);
	}

	void sound_engine_close() {
		PaError error;

		error = Pa_StopStream(stream);
		pa_check_error(error);

		error = Pa_CloseStream(stream);
		pa_check_error(error);

		error = Pa_Terminate();
		pa_check_error(error);
	}

	void start_stream() {
		const PaError error{ Pa_StartStream(stream) };
		pa_check_error(error);
		Pa_Sleep(50);
	}
}