#include "sound_engine.h"
#include "synthesis/synthesizer.h"
#include "synthesis/modules/oscillator.h"
#include "synthesis/modules/voice.h"
#include "synthesis/modules/voice_manager.h"
#include "synthesis/modules/fx/delay.h"
#include "synthesis/modules/modulator/envelope.h"
#include "standalone/midi_listener.h"

using namespace standalone;
using namespace synthesis;

int main() {
	// master, voice_manager, and module are initialized in synthesizer.cpp. maybe theres a better way to structure this?

	midi_listener::init();
	midi_listener::open_port(config::midi_port);
	sound_engine::sound_engine_init();

	synthesis::voice_manager = static_cast<VoiceManager*>(synthesis::add_module(make_unique<VoiceManager>()));
	//Delay* delay{ static_cast<Delay*>(synthesis::add_module(make_unique<Delay>(Delay{}))) };
	//delay->add_output(master, true);
	//delay->wet = 0.5;
	//delay->set_delay_time(0.5);
	//delay->set_feedback(0.5);
	Mixer* mixer{ static_cast<Mixer*>(synthesis::add_module(make_unique<Mixer>(Mixer{}))) };
	mixer->add_output(master, true);
	for (int i{ 0 }; i < config::num_voices; i++) {
		//Oscillator* osc_sine{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>(Oscillator{ "sine" }))) };
		//osc_sine->add_output(mixer, true);
		//Oscillator* osc_sawtooth{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>(Oscillator{ "sawtooth" }))) };
		//osc_sawtooth->add_output(mixer, true);
		Oscillator* osc_triangle{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>(Oscillator{ "triangle" }))) };
		osc_triangle->add_output(mixer, true);
		Envelope* envelope{ static_cast<Envelope*>(synthesis::add_module(make_unique<Envelope>(Envelope{}))) };
		envelope->add_output(osc_triangle, true);
		envelope->set_attack(0.3);
		envelope->set_decay(0.3);
		envelope->set_sustain(0.5);
		envelope->set_release(0.5);
		osc_triangle->gain_mod = envelope;
		Voice* voice{ static_cast<Voice*>(synthesis::add_module(make_unique<Voice>(Voice{}))) };
		//voice->add_output(osc_sine, true);
		//voice->add_output(osc_sawtooth, true);
		voice->add_output(osc_triangle, false);
		voice->add_output(envelope, false);
		voice_manager->add_output(voice, true);
	}

	sound_engine::start_stream();

	while (true) {
		Pa_Sleep(10);
	}

	midi_listener::close_port();
	sound_engine::sound_engine_close();

	return 0;
}