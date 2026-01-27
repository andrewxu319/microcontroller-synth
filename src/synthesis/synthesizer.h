#include "utils/global.h"

#include "modules/module.h"
#include "modules/master.h"
#include "modules/voice_manager.h"
#include "midi/message.h"

#include <queue>
#include <functional>

namespace synthesis {
	extern Master* master;
	extern VoiceManager* voice_manager;
	extern std::vector<std::unique_ptr<Module>> modules; // unique_ptr to preserve derived classes
	extern std::queue<midi::NoteMessage> note_messages;
	extern std::queue<midi::CcMessage> cc_messages;
	extern std::array<std::vector<std::function<void(uint8_t)>>, 128> cc_mappings;

	extern void init();
	extern Module* add_module(std::unique_ptr<Module> module);
	extern int topo_sort();
	extern void read_messages();

	template <typename Fn>
	inline void attach_cc(const uint8_t cc, Fn&& fn) {
		if (cc_mappings[cc].empty()) {
			cc_mappings[cc] = std::vector<std::function<void(uint8_t)>>{};
		}
		cc_mappings[cc].emplace_back(std::function<void(uint8_t)>(std::forward<Fn>(fn)));
	}
	extern void reset_cc(const uint8_t cc);
}