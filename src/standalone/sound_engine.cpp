#include "sound_engine.h"

#include <cstdio>  

namespace standalone::sound_engine {
	Master& master{ Master::instance() };
	const BufferLoaderData data{};
	PaStream* stream{};
	queue<midi::NoteMessage> note_messages{};
	queue<midi::CcMessage> cc_messages{};

	void pa_check_error(const PaError& error) {
		if (error != paNoError) {
			printf("PortAudio error: %s\n", Pa_GetErrorText(error));
		}
	}

	int load_buffer(
		const void* in_buf_,
		void* out_buf_,
		unsigned long buffer_size,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flags,
		void* data_
	) {
		BufferLoaderData* data = (BufferLoaderData*)data_;
		float32_t* out_buf{ (float32_t*)out_buf_ };
		master.out_buf = out_buf;
		master.generate_buf();

		return 0;
	}

	void sound_engine_init(vector<unique_ptr<Module>>* const modules_)
	{
		master.instance().modules = modules_;

		PaError error;

		error = Pa_Initialize();
		pa_check_error(error);

		error = Pa_OpenDefaultStream(
			&stream,
			0,
			config::channels,
			paFloat32,
			config::sample_rate,
			config::actual_buffer_size,
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