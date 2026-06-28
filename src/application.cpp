#include "utils/global.h"
#include "synthesis/synthesizer.h"
#include "synthesis/modules/oscillator.h"
#include "synthesis/modules/performed_oscillator.h"
#include "synthesis/modules/noise_generator.h"
#include "synthesis/modules/voice.h"
#include "synthesis/modules/voice_manager.h"
#include "synthesis/modules/fx/filter/RBJ/lowpass.h"
#include "synthesis/modules/fx/filter/RBJ/highpass.h"
#include "synthesis/modules/fx/filter/RBJ/bandpass.h"
#include "synthesis/modules/fx/filter/RBJ/notch.h"
#include "synthesis/modules/fx/filter/RBJ/low_shelf.h"
#include "synthesis/modules/fx/filter/RBJ/high_shelf.h"
#include "synthesis/modules/fx/delay.h"
#include "synthesis/modules/fx/phaser.h"
//#include "synthesis/modules/fx/flanger.h"
#include "synthesis/modules/fx/chorus.h"
#include "synthesis/modules/fx/reverb/schroeder.h"
#include "synthesis/modules/fx/reverb/luff.h"
#include "synthesis/modules/fx/components/multichannel_diffuser.h"
#include "synthesis/modules/fx/distortion/soft_clip.h"
#include "synthesis/modules/fx/distortion/ideal_wavefolder.h"
#include "synthesis/modules/fx/distortion/serge_wavefolder/serge_wavefolder.h"
#include "synthesis/modules/modulator/envelope.h"
#include "synthesis/modules/modulator/portamento.h"
#ifdef TEENSY
	#include "teensy/midi_listener.h"
	#include "teensy/sound_engine.h"
#else
	#include "standalone/midi_listener.h"
#endif

#include <cmath>

using namespace synthesis;

#define ADD_MODULE(T, ...) static_cast<T*>(synthesizer.add_module(std::make_unique<T>(__VA_ARGS__)))

void application(Synthesizer& synthesizer) {
	// master, voice_manager, and module are initialized in synthesizer.cpp. maybe theres a better way to structure this?
	Master* master{ synthesizer.master };
	
	synthesizer.voice_manager->set_legato(true);

	// be careful---even slight gain causes significant distortion => sounds bad with polyphony
	SergeWavefolder* wavefolder{ ADD_MODULE(SergeWavefolder) };
	wavefolder->set_gain(0.06f);
	wavefolder->set_offset(0.0f);
	wavefolder->add_output(master, Master::BufType::AUDIO);

	Oscillator* wavefolder_lfo_0{ ADD_MODULE(Oscillator, "sine") };
	wavefolder_lfo_0->add_output(wavefolder, SergeWavefolder::BufType::GAIN);
	wavefolder_lfo_0->set_freq(1.0);
	wavefolder_lfo_0->set_gain(0.03);
	wavefolder_lfo_0->set_phase(0.3592);

	// Oscillator* wavefolder_lfo_1{ ADD_MODULE(Oscillator, "sine") };
	// wavefolder_lfo_1->add_output(wavefolder, SergeWavefolder::BufType::OFFSET);
	// wavefolder_lfo_1->set_freq(0.724925);
	// wavefolder_lfo_1->set_gain(0.03);
	// wavefolder_lfo_1->set_phase(0.129303);

	Schroeder* schroeder_reverb{ ADD_MODULE(Schroeder) };
	schroeder_reverb->add_output(wavefolder, Master::BufType::AUDIO);
	schroeder_reverb->set_decay_time(70);
	schroeder_reverb->set_wet(0.6);

	Oscillator* schroeder_reverb_lfo{ ADD_MODULE(Oscillator, "sine") };
	schroeder_reverb_lfo->set_freq(0.5);
	schroeder_reverb_lfo->set_gain(50);
	schroeder_reverb->add_buf(schroeder_reverb_lfo->get_out_buf(), Schroeder::BufType::DECAY);

	//  //Luff* luff_reverb{ ADD_MODULE(Luff, 4) };
	//  //luff_reverb->add_output(master, Master::BufType::AUDIO);
	//  //luff_reverb->wet = 0.5f;
	//  //luff_reverb->set_diffuser_delays({ 20, 40, 80, 160 });
	//  //luff_reverb->set_feedback(0.85f);
	//  //luff_reverb->set_feedback_delay_range(100, 200);
	//  //luff_reverb->set_decay_time(1000); // why is this in ms
	//  //luff_reverb->set_mixing_matrix(Reverb::MixingMatrix::Householder);

	// Delay* delay{ ADD_MODULE(Delay) };
	// delay->add_output(schroeder_reverb, Master::BufType::AUDIO);
	// delay->set_wet(0.5);
	// delay->set_delay(0.5);
	// delay->set_feedback(0.5);

	// Oscillator* delay_lfo{ ADD_MODULE(Oscillator, "sine") };
	// delay_lfo->add_output(delay, Delay::BufType::FEEDBACK);
	// delay_lfo->set_freq(1);
	// delay_lfo->set_gain(0.25);

	RBJFilter::LowShelf* filter{ ADD_MODULE(RBJFilter::LowShelf) };
	filter->add_output(schroeder_reverb, RBJFilter::LowShelf::BufType::AUDIO);
	filter->set_cutoff(5000);
	filter->set_gain(0.0);
	filter->set_slope(1.0);
	filter->set_wet(1.0f);

	// Phaser* phaser{ ADD_MODULE(Phaser) };
	// phaser->set_wet(0.5f);
	// phaser->set_center_freq(4000);
	// phaser->set_stages(4);
	// phaser->set_feedback(0.6);
	// phaser->add_output(filter, Master::BufType::AUDIO);

	// Oscillator* phaser_lfo_0{ ADD_MODULE(Oscillator, "sine") };
	// phaser_lfo_0->load_waveform("sine");
	// phaser_lfo_0->set_freq(0.5);
	// phaser_lfo_0->set_gain(0.3);
	// phaser_lfo_0->add_output(phaser, Phaser::BufType::WET);

	// Oscillator* phaser_lfo_1{ ADD_MODULE(Oscillator, "sine") };
	// phaser_lfo_1->load_waveform("sine");
	// phaser_lfo_1->set_freq(0.28);
	// phaser_lfo_1->set_gain(3000);
	// phaser_lfo_1->add_output(phaser, Phaser::BufType::CENTER_FREQ);

	// Oscillator* phaser_lfo_2{ ADD_MODULE(Oscillator, "sine") };
	// phaser_lfo_2->load_waveform("sine");
	// phaser_lfo_2->set_freq(0.82);
	// phaser_lfo_2->set_gain(0.3);
	// phaser_lfo_2->add_output(phaser, Phaser::BufType::FEEDBACK);

	//Flanger* flanger{ ADD_MODULE(Flanger) };
	//flanger->add_output(phaser_lfo_0, Master::BufType::AUDIO);
	//flanger->set_wet(1.0f);
	//flanger->set_delay(6);
	//synthesizer.attach_cc(18,
	//	[target = flanger]
	//	(const uint8_t x) {
	//		target->set_feedback((x * 2 / 127.0 - 1.0) * 0.99);
	//	}
	//);

	//Oscillator* flanger_lfo{ ADD_MODULE(Oscillator, "sine") };
	//flanger_lfo->load_waveform("sine");
	//flanger_lfo->set_freq(0.5);
	//flanger_lfo->set_gain(5);
	//flanger_lfo->add_output(flanger, true);
	//flanger->attach_mod(flanger_lfo->get_out_buf(), Flanger::BufType::OFFSET);

	// Chorus* chorus{ ADD_MODULE(Chorus) };
	// chorus->set_wet(1.0f);
	// chorus->set_delay(30);
	// chorus->set_voice_count(6);
	// chorus->add_output(phaser, Phaser::BufType::AUDIO);

	//Oscillator* chorus_lfo{ ADD_MODULE(Oscillator, "sine") };
	//chorus_lfo->load_waveform("sine");
	//chorus_lfo->set_freq(1);
	//chorus_lfo->set_gain(0.5);
	//chorus_lfo->add_output(chorus, Chorus::BufType::FREQ_RANGE);

	//SoftClip* soft_clip{ ADD_MODULE(SoftClip) };
	//soft_clip->set_drive(10.0f);
	//soft_clip->add_output(master, Master::BufType::AUDIO);

	//Oscillator* soft_clip_lfo{ ADD_MODULE(Oscillator, "sine") };
	//soft_clip_lfo->set_freq(1);
	//soft_clip_lfo->set_gain(9);
	//soft_clip_lfo->add_output(soft_clip, SoftClip::BufType::DRIVE);

	Mixer* mixer{ ADD_MODULE(Mixer) };
	mixer->add_output(filter, Master::BufType::AUDIO);
	// mixer->add_output(luff_reverb, -1);
	// for (uint8_t i{ 0 }; i < 8; i++) {
	// 	luff_reverb->add_buf(mixer->get_out_buf(), MultichannelDiffuser::BufType::AUDIO);
	// }

	for (int i{ 0 }; i < config::num_voices; i++) {
		PerformedOscillator* oscillator{ ADD_MODULE(PerformedOscillator, "sine") };
		oscillator->add_output(mixer, Mixer::BufType::AUDIO);
		oscillator->set_gain(0);

		Envelope* envelope{ ADD_MODULE(Envelope) };
		envelope->add_output(oscillator, PerformedOscillator::BufType::GAIN);
		envelope->set_attack(0.02f);
		envelope->set_decay(0.02f);
		envelope->set_sustain(1.0f);
		envelope->set_release(0.02f);

		// cannot set a/d/r to 0. add check later
		synthesizer.attach_cc(14,
			[target = envelope]
			(const uint8_t x) {
				target->set_attack(static_cast<float_s>(pow(2, 0.0181102362 * x - 1) - 0.490)); // at least 10ms to avoid popping
			}
		);
		synthesizer.attach_cc(15,
			[target = envelope]
			(const uint8_t x) {
				target->set_decay(static_cast<float_s>(pow(2, 0.0181102362 * x - 1) - 0.490)); // at least 10ms to avoid popping
			}
		);
		synthesizer.attach_cc(16,
			[target = envelope]
			(const uint8_t x) {
				target->set_sustain(x / 127.0f);
			}
		);
		synthesizer.attach_cc(17,
			[target = envelope]
			(const uint8_t x) {
				target->set_release(static_cast<float_s>(pow(2, 0.0181102362 * x - 1) - 0.490)); // at least 10ms to avoid popping
			}
		);

		//Portamento* portamento{ ADD_MODULE(Portamento) };
		//portamento->set_time(100);
		//portamento->add_output(oscillator, PerformedOscillator::BufType::PITCH);

		//Oscillator* pitch_lfo{ ADD_MODULE(Oscillator, 	"sine", true) };
		//pitch_lfo->add_output(oscillator, PerformedOscillator::BufType::PITCH);
		//pitch_lfo->set_freq(4.64);
		//pitch_lfo->set_gain(100);

		Voice* voice{ ADD_MODULE(Voice) };
		voice->add_output(oscillator);
		voice->add_output(envelope);
		//voice->add_output(portamento);
		synthesizer.voice_manager->add_output(voice);
	}

	synthesizer.init();

	// #ifdef TEENSY
	// while (true) {
	// 	synthesis::voice_manager->note_on(60, 127);
	// 	teensy::sound_engine::load_buffer();
	// } 
	// #endif
}