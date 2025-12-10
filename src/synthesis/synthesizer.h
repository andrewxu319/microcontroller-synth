#include "utils/includes.h"

#include "modules/module.h"
#include "modules/master.h"
#include "modules/voice_manager.h"
#include "midi/message.h"

#include <queue>

namespace synthesis {
	extern Master* master;
	extern VoiceManager* voice_manager;
	extern vector<unique_ptr<Module>> modules; // unique_ptr to preserve derived classes
	extern queue<midi::NoteMessage> note_messages;
	extern queue<midi::CcMessage> cc_messages;

	extern void read_messages();
	extern Module* add_module(unique_ptr<Module> module);
	extern int topo_sort();
}