#pragma once

#include "utils/includes.h"
#include "mixer.h"
#include "module.h"
#include "midi/message.h"

#include <queue>

namespace synthesis {
	class Master : public Mixer {
	public:
		float32_t* out_buf;
		const vector<unique_ptr<Module>>* modules;
		queue<midi::NoteMessage> note_messages;
		queue<midi::CcMessage> cc_messages;

		static Master& instance();
		void generate_buf();
		
	private:
		Master();
		Master(const Master&) = delete;
		Master& operator =(const Master&) = delete;
	};
}