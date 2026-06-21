#include "utils/global.h"

#include "modules/module.h"
#include "modules/master.h"
#include "modules/voice_manager.h"
#include "midi/message.h"

#include <queue>
#include <functional>

using namespace synthesis;

class Synthesizer {
public:
	Master* master;
	VoiceManager* voice_manager;
	std::queue<midi::NoteMessage> note_messages;
	std::queue<midi::CcMessage> cc_messages;
	std::array<std::vector<std::function<void(uint8_t)>>, 128> cc_mappings;
	std::vector<std::unique_ptr<Module>> modules;

	void init();
	Module* add_module(std::unique_ptr<Module> module);
	int topo_sort();
	void read_messages();

	template <typename Fn>
	void attach_cc(const uint8_t cc, Fn&& fn) {
		if (cc_mappings[cc].empty()) {
			cc_mappings[cc] = std::vector<std::function<void(uint8_t)>>{};
		}
		cc_mappings[cc].emplace_back(std::function<void(uint8_t)>(std::forward<Fn>(fn)));
	}
	void reset_cc(const uint8_t cc);

	static Synthesizer& instance();

private:
	Synthesizer();
	Synthesizer(const Synthesizer&) = delete;
	Synthesizer& operator =(const Synthesizer&) = delete;
};