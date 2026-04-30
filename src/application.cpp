#include "utils/global.h"
#include "synthesis/synthesizer.h"
#include "synthesis/modules/oscillator.h"
#include "synthesis/modules/noise_generator.h"
#include "synthesis/modules/voice.h"
#include "synthesis/modules/voice_manager.h"
#include "synthesis/modules/fx/filter.h"
#include "synthesis/modules/fx/delay.h"
#include "synthesis/modules/fx/phaser.h"
#include "synthesis/modules/fx/flanger.h"
#include "synthesis/modules/fx/chorus.h"
#include "synthesis/modules/fx/reverb/schroeder.h"
#include "synthesis/modules/fx/reverb/luff.h"
#include "synthesis/modules/fx/components/multichannel_diffuser.h"
#include "synthesis/modules/modulator/envelope.h"
#ifdef TEENSY
	#include "teensy/midi_listener.h"
	#include "teensy/sound_engine.h"
#else
	#include "standalone/midi_listener.h"
#endif

#include <cmath>

using namespace synthesis;

void application() {
	// master, voice_manager, and module are initialized in synthesizer.cpp. maybe theres a better way to structure this?
	synthesis::voice_manager = static_cast<VoiceManager*>(synthesis::add_module(std::make_unique<VoiceManager>()));
	synthesis::voice_manager->set_legato(true);

	Schroeder* schroeder_reverb{ static_cast<Schroeder*>(synthesis::add_module(std::make_unique<Schroeder>())) };
	schroeder_reverb->add_output(master, Master::BufType::AUDIO);
	schroeder_reverb->set_decay_time(10);
	schroeder_reverb->wet = 0.0;

	//Oscillator* schroeder_reverb_lfo{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("sine"))) };
	//schroeder_reverb_lfo->set_freq(0.5);
	//schroeder_reverb_lfo->set_gain(50);
	//schroeder_reverb->add_buf(schroeder_reverb_lfo->get_out_buf(), Schroeder::BufType::DECAY);

	 //Luff* luff_reverb{ static_cast<Luff*>(synthesis::add_module(std::make_unique<Luff>(4))) };
	 //luff_reverb->add_output(master, Master::BufType::AUDIO);
	 //luff_reverb->wet = 0.5f;
	 //luff_reverb->set_diffuser_delays({ 20, 40, 80, 160 });
	 //luff_reverb->set_feedback(0.85f);
	 //luff_reverb->set_feedback_delay_range(100, 200);
	 //luff_reverb->set_decay_time(1000); // why is this in ms
	 //luff_reverb->set_mixing_matrix(Reverb::MixingMatrix::Householder);

	//Delay* delay{ static_cast<Delay*>(synthesis::add_module(std::make_unique<Delay>())) };
	//delay->add_output(master, Master::BufType::AUDIO);
	//delay->wet = 0.5;
	//delay->set_delay(0.5);
	//delay->set_feedback(0.5);

	//Oscillator* delay_lfo{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("sine"))) };
	//delay_lfo->add_output(delay, true);
	//delay->attach_mod(delay_lfo->get_out_buf(), Delay::BufType::FEEDBACK);
	//delay_lfo->set_freq(1);
	//delay_lfo->set_gain(0.25);

	//Filter<Dsp::RBJ::Design::LowPass, 1>* filter{ static_cast<Filter<Dsp::RBJ::Design::LowPass, 1>*>(synthesis::add_module(std::make_unique<Filter<Dsp::RBJ::Design::LowPass, 1>>())) };
	//filter->add_output(delay, Delay::BufType::AUDIO);
	//filter->set_cutoff(2000);
	//filter->set_resonance(1.25);
	////filter->set_band_width(100);
	//filter->wet = 1.0;

	//Oscillator* filter_lfo{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("sine"))) };
	//filter_lfo->add_output(filter, true);
	//filter->attach_mod(filter_lfo->get_out_buf(), Filter<Dsp::RBJ::Design::LowPass, 1>::BufType::WET);
	//filter_lfo->set_freq(0.5);
	//filter_lfo->set_gain(0.5);

	//Phaser* phaser{ static_cast<Phaser*>(synthesis::add_module(std::make_unique<Phaser>())) };
	//phaser->wet = 0.5;
	//phaser->set_center_freq(1000);
	//phaser->set_stages(4);
	//phaser->set_feedback(0.7);
	//phaser->add_output(master, Filter<Dsp::RBJ::Design::LowPass, 1>::BufType::AUDIO);

	//Oscillator* phaser_lfo{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("sine"))) };
	//phaser_lfo->load_waveform("sine");
	//phaser_lfo->set_freq(0.5);
	//phaser_lfo->set_gain(0.5);
	//phaser_lfo->add_output(phaser, Phaser::BufType::WET);

	//Flanger* flanger{ static_cast<Flanger*>(synthesis::add_module(std::make_unique<Flanger>())) };
	//flanger->add_output(master, Master::BufType::AUDIO);
	//flanger->wet = 1.0;
	//flanger->set_delay(6);
	//synthesis::attach_cc(18,
	//	[target = flanger]
	//	(const uint8_t x) {
	//		target->set_feedback((x * 2 / 127.0 - 1.0) * 0.99);
	//	}
	//);

	//Oscillator* flanger_lfo{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("sine"))) };
	//flanger_lfo->load_waveform("sine");
	//flanger_lfo->set_freq(0.5);
	//flanger_lfo->set_gain(5);
	//flanger_lfo->add_output(flanger, true);
	//flanger->attach_mod(flanger_lfo->get_out_buf(), Flanger::BufType::OFFSET);

	//Chorus* chorus{ static_cast<Chorus*>(synthesis::add_module(std::make_unique<Chorus>())) };
	//chorus->wet = 1.0;
	//chorus->set_delay(30);
	//chorus->set_voice_count(6);
	//chorus->add_output(master, Phaser::BufType::AUDIO);

	//Oscillator* chorus_lfo{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("sine"))) };
	//chorus_lfo->load_waveform("sine");
	//chorus_lfo->set_freq(1);
	//chorus_lfo->set_gain(0.5);
	//chorus_lfo->add_output(chorus, Chorus::BufType::FREQ_RANGE);

	Mixer* mixer{ static_cast<Mixer*>(synthesis::add_module(std::make_unique<Mixer>())) };
	mixer->add_output(schroeder_reverb, Master::BufType::AUDIO);
	// mixer->add_output(luff_reverb, -1);
	// for (uint8_t i{ 0 }; i < 8; i++) {
	// 	luff_reverb->add_buf(mixer->get_out_buf(), MultichannelDiffuser::BufType::AUDIO);
	// }

	for (int i{ 0 }; i < config::num_voices; i++) {
		//NoiseGenerator* noise_generator{ static_cast<NoiseGenerator*>(synthesis::add_module(std::make_unique<NoiseGenerator>())) };
		//noise_generator->add_output(mixer, Mixer::BufType::AUDIO);
		//noise_generator->set_gain(0);
		//Oscillator* osc_sine{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("sine"))) };
		//osc_sine->add_output(mixer, true);
		//osc_sine->set_gain(0);
		Oscillator* osc_sawtooth{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("sine"))) };
		osc_sawtooth->add_output(mixer, Mixer::BufType::AUDIO);
		osc_sawtooth->set_gain(0);
		//Oscillator* osc_triangle{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("triangle"))) };
		//osc_triangle->add_output(mixer, true);
		Envelope* envelope{ static_cast<Envelope*>(synthesis::add_module(std::make_unique<Envelope>())) };
		//envelope->add_output(osc_sine, true);
		envelope->add_output(osc_sawtooth, NoiseGenerator::BufType::GAIN);
		//envelope->add_output(osc_triangle, true);
		envelope->set_attack(0.02f);
		envelope->set_decay(0.02f);
		envelope->set_sustain(1.0f);
		envelope->set_release(0.02f);

		// cannot set a/d/r to 0. add check later
		synthesis::attach_cc(14,
			[target = envelope]
			(const uint8_t x) {
				target->set_attack(static_cast<float_s>(pow(2, 0.0181102362 * x - 1) - 0.490)); // at least 10ms to avoid popping
			}
		);
		synthesis::attach_cc(15,
			[target = envelope]
			(const uint8_t x) {
				target->set_decay(static_cast<float_s>(pow(2, 0.0181102362 * x - 1) - 0.490)); // at least 10ms to avoid popping
			}
		);
		synthesis::attach_cc(16,
			[target = envelope]
			(const uint8_t x) {
				target->set_sustain(x / 127.0f);
			}
		);
		synthesis::attach_cc(17,
			[target = envelope]
			(const uint8_t x) {
				target->set_release(static_cast<float_s>(pow(2, 0.0181102362 * x - 1) - 0.490)); // at least 10ms to avoid popping
			}
		);

		//Oscillator* pitch_lfo{ static_cast<Oscillator*>(synthesis::add_module(std::make_unique<Oscillator>("sine", true))) };
		//pitch_lfo->add_output(osc_sawtooth, true);
		//osc_sawtooth->attach_mod(pitch_lfo->get_out_buf(), Oscillator::BufType::PITCH);
		//pitch_lfo->set_freq(4.64);
		//pitch_lfo->set_gain(10);

		Voice* voice{ static_cast<Voice*>(synthesis::add_module(std::make_unique<Voice>())) };
		voice->add_output(envelope);
		synthesis::voice_manager->add_output(voice);
	}

	synthesis::init();
	// synthesis::voice_manager->note_on(60, 127);
	// #ifdef TEENSY
	// while (true) {
	// 	synthesis::voice_manager->note_on(60, 127);
	// 	teensy::sound_engine::load_buffer();
	// }
	// #endif
}