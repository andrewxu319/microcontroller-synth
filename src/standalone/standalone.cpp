#include "sound_engine.h"
#include "synthesis/synthesizer.h"
#include "synthesis/modules/oscillator.h"
#include "synthesis/modules/voice.h"
#include "synthesis/modules/voice_manager.h"
#include "synthesis/modules/fx/filter.h"
#include "synthesis/modules/fx/delay.h"
#include "synthesis/modules//fx/phaser.h"
#include "synthesis/modules//fx/flanger.h"
#include "synthesis/modules//fx/chorus.h"
#include "synthesis/modules/modulator/envelope.h"
#include "standalone/midi_listener.h"

#include <cmath>

using namespace standalone;
using namespace synthesis;

int main() {
	// master, voice_manager, and module are initialized in synthesizer.cpp. maybe theres a better way to structure this?

	sound_engine::sound_engine_init();
	midi_listener::init();
	midi_listener::open_port(config::midi_port);

	synthesis::voice_manager = static_cast<VoiceManager*>(synthesis::add_module(make_unique<VoiceManager>()));
	//Delay* delay{ static_cast<Delay*>(synthesis::add_module(make_unique<Delay>())) };
	//delay->add_output(master, true);
	//delay->wet = 0.5;
	//delay->set_delay_time(0.5);
	//delay->set_feedback(0.5);

	//Filter<Dsp::RBJ::Design::LowPass, 1>* filter{ static_cast<Filter<Dsp::RBJ::Design::LowPass, 1>*>(synthesis::add_module(make_unique<Filter<Dsp::RBJ::Design::LowPass, 1>>())) };
	//filter->add_output(master, true);
	//filter->set_cutoff(20);
	//filter->set_resonance(1.25);
	////filter->set_band_width(100);
	//filter->wet = 1.0;
	////delay->set_audio_input(filter);

	//Oscillator* filter_lfo{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>("sine"))) };
	//filter_lfo->add_output(filter, true);
	//filter->attach_mod(filter_lfo, Filter<Dsp::RBJ::Design::LowPass, 1>::Mods::WET);
	//filter_lfo->set_freq(0.5);
	//filter_lfo->set_gain(0.5);

	//Phaser* phaser{ static_cast<Phaser*>(synthesis::add_module(make_unique<Phaser>())) };
	//phaser->add_output(master, true);
	//phaser->wet = 0.5;
	//phaser->set_center_freq(1000);
	//phaser->set_stages(12);
	//phaser->set_feedback(0.0);

	//Oscillator* phaser_lfo{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>("sine"))) };
	//phaser_lfo->load_waveform("sine");
	//phaser_lfo->set_freq(0.5);
	//phaser_lfo->set_gain(0.5);
	//phaser_lfo->add_output(phaser, true);
	//phaser->attach_mod(phaser_lfo, Phaser::Mods::WET);

	//Flanger* flanger{ static_cast<Flanger*>(synthesis::add_module(make_unique<Flanger>())) };
	//flanger->add_output(master, true);
	//flanger->wet = 1.0;
	//flanger->set_offset(6);

	//Oscillator* flanger_lfo{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>("sine"))) };
	//flanger_lfo->load_waveform("sine");
	//flanger_lfo->set_freq(0.5);
	//flanger_lfo->set_gain(5);
	//flanger_lfo->add_output(flanger, true);
	//flanger->attach_mod(flanger_lfo, Flanger::Mods::OFFSET);

	Chorus* chorus{ static_cast<Chorus*>(synthesis::add_module(make_unique<Chorus>())) };
	chorus->add_output(master, true);
	chorus->wet = 1.0;
	chorus->set_delay(300);
	chorus->set_voice_count(6);

	//Oscillator* chorus_lfo{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>("sine"))) };
	//chorus_lfo->load_waveform("sine");
	//chorus_lfo->set_freq(0.5);
	//chorus_lfo->set_gain(0);
	//chorus_lfo->add_output(chorus, true);
	//chorus->attach_mod(chorus_lfo, Chorus::Mods::DELAY);

	Mixer* mixer{ static_cast<Mixer*>(synthesis::add_module(make_unique<Mixer>())) };
	mixer->add_output(chorus, true);
	chorus->set_audio_input(mixer);

	for (int i{ 0 }; i < config::num_voices; i++) {
		//Oscillator* osc_sine{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>("sine"))) };
		//osc_sine->add_output(mixer, true);
		//osc_sine->set_gain(0);
		Oscillator* osc_sawtooth{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>("sawtooth"))) };
		osc_sawtooth->add_output(mixer, true);
		osc_sawtooth->set_gain(0);
		//Oscillator* osc_triangle{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>("triangle"))) };
		//osc_triangle->add_output(mixer, true);
		Envelope* envelope{ static_cast<Envelope*>(synthesis::add_module(make_unique<Envelope>())) };
		//envelope->add_output(osc_sine, true);
		envelope->add_output(osc_sawtooth, true);
		//envelope->add_output(osc_triangle, true);
		envelope->set_attack(0.3);
		envelope->set_decay(0.3);
		envelope->set_sustain(0.5);
		envelope->set_release(0.5);
		synthesis::attach_cc(14, 
			[target = envelope]
			(const uint8_t x) {
				target->set_attack(0.5 * pow(2, 0.0181102362 * x) - 0.499);
			}
		);
		synthesis::attach_cc(15,
			[target = envelope]
			(const uint8_t x) {
				target->set_decay(0.5 * pow(2, 0.0181102362 * x) - 0.499);
			}
		);
		synthesis::attach_cc(16,
			[target = envelope]
			(const uint8_t x) {
				target->set_sustain(x / 127.0);
			}
		);
		synthesis::attach_cc(17,
			[target = envelope]
			(const uint8_t x) {
				target->set_release(0.5 * pow(2, 0.0181102362 * x) - 0.499);
			}
		);
		//osc_sine->attach_mod(envelope, Oscillator::Mods::GAIN);
		osc_sawtooth->attach_mod(envelope, Oscillator::Mods::GAIN);
		//osc_triangle->attach_mod(envelope, Oscillator::Mods::GAIN);

		//Oscillator* pitch_lfo{ static_cast<Oscillator*>(synthesis::add_module(make_unique<Oscillator>("sine", true))) };
		//pitch_lfo->add_output(osc_sawtooth, true);
		//osc_sawtooth->attach_mod(pitch_lfo, Oscillator::Mods::PITCH);
		//pitch_lfo->set_freq(4.64);
		//pitch_lfo->set_gain(10);

		Voice* voice{ static_cast<Voice*>(synthesis::add_module(make_unique<Voice>())) };
		voice->add_output(envelope, false);
		synthesis::voice_manager->add_output(voice, false);
	}

	sound_engine::start_stream();

	while (true) {
		Pa_Sleep(10000);
	}

	midi_listener::close_port();
	sound_engine::sound_engine_close();

	return 0;
}