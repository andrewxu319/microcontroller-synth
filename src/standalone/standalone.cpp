#include "sound_engine.h"
#include "synthesis/modules/oscillator.h"
#include "synthesis/modules/voice.h"
#include "synthesis/modules/voice_manager.h"

#include <cstdio>

using namespace standalone;
using namespace synthesis;

int main() {
	printf("ok");

	vector<unique_ptr<Module>> modules{};

	sound_engine::sound_engine_init(&modules);
	modules.emplace_back(make_unique<VoiceManager>(VoiceManager{}));
	for (int i{ 0 }; i < config::num_voices; i++) {
		modules.emplace_back(make_unique<Mixer>(Mixer{ vector<Module*>{ &sound_engine::master } }));
		modules.emplace_back(make_unique<Oscillator>(Oscillator{ vector<Module*>{ modules.back().get() }, "sine"}));
		modules.emplace_back(make_unique<Oscillator>(Oscillator{ vector<Module*>{ modules[modules.size() - 2].get() }, "sawtooth"}));
		modules.emplace_back(make_unique<Voice>(Voice{ vector<Module*>{ modules.back().get(), modules[modules.size() - 2].get()}}));
		modules[0]->add_output(modules.back().get());
	}
	VoiceManager* voice_manager{ static_cast<VoiceManager*>(modules[0].get())};

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

	sound_engine::sound_engine_close();

	return 0;
}