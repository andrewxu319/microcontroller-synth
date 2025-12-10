#include "sound_engine.h"
#include "synthesis/synthesizer.h"
#include "synthesis/modules/oscillator.h"
#include "synthesis/modules/voice.h"
#include "synthesis/modules/voice_manager.h"
#include "standalone/midi_listener.h"

using namespace standalone;
using namespace synthesis;

int main() {
	// master, voice_manager, and module are initialized in synthesizer.cpp. maybe theres a better way to structure this?

	midi_listener::init();
	midi_listener::open_port(config::midi_port);
	sound_engine::sound_engine_init();

	for (int i{ 0 }; i < config::num_voices; i++) {
		Mixer* mixer{ static_cast<Mixer*>(synthesis::add_module(make_unique<Mixer>(Mixer{}))) };
		mixer->add_output(master);
		Oscillator* osc_sine{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>(Oscillator{ "sine" }))) };
		osc_sine->add_output(mixer);
		Oscillator* osc_sawtooth{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>(Oscillator{ "sawtooth" }))) };
		osc_sawtooth->add_output(mixer);
		Voice* voice{ static_cast<Voice*>(synthesis::add_module(make_unique<Voice>(Voice{}))) };
		voice->add_outputs(vector<Module*>{ osc_sine, osc_sawtooth});
		voice_manager->add_output(voice);
	}

	sound_engine::start_stream();

	while (true) {
		Pa_Sleep(10);
	}

	midi_listener::close_port();
	sound_engine::sound_engine_close();

	return 0;
}