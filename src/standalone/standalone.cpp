#include "sound_engine.h"
#include "synthesis/modules/oscillator.h"
#include "synthesis/modules/voice.h"
#include "synthesis/modules/voice_manager.h"
#include "standalone/midi_listener.h"

#include <cstdio>

using namespace standalone;
using namespace synthesis;

int main() {
	printf("ok");

	midi_listener::init();
	midi_listener::open_port(config::midi_port);

	sound_engine::sound_engine_init();
	Master& master{ Master::instance() };
	//master.add_module(unique_ptr<Master>(&master));
	VoiceManager* voice_manager{ static_cast<VoiceManager*>(master.add_module(make_unique<VoiceManager>())) };
	for (int i{ 0 }; i < config::num_voices; i++) {
		Mixer* mixer{ static_cast<Mixer*>(master.add_module(make_unique<Mixer>(Mixer{}))) };
		mixer->add_output(&master);
		Oscillator* osc_sine{ static_cast<Oscillator*>(master.add_module(make_unique<Oscillator>(Oscillator{ "sine" }))) };
		osc_sine->add_output(mixer);
		Oscillator* osc_sawtooth{ static_cast<Oscillator*>(master.add_module(make_unique<Oscillator>(Oscillator{ "sawtooth" }))) };
		osc_sawtooth->add_output(mixer);
		Voice* voice{ static_cast<Voice*>(master.add_module(make_unique<Voice>(Voice{}))) };
		voice->add_outputs(vector<Module*>{ osc_sine, osc_sawtooth});
		voice_manager->add_output(voice);
	}

	sound_engine::start_stream();

	voice_manager->note_on(70);
	Pa_Sleep(500);
	voice_manager->note_on(74);
	Pa_Sleep(500);
	voice_manager->note_on(77);
	Pa_Sleep(500);
	voice_manager->note_on(82);
	Pa_Sleep(500);
	voice_manager->note_off(70);
	voice_manager->note_off(74);
	voice_manager->note_off(77);
	voice_manager->note_off(82);

	Pa_Sleep(3000);

	midi_listener::close_port();
	sound_engine::sound_engine_close();

	return 0;
}