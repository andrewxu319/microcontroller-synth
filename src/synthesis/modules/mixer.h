#pragma once

#include "module.h"

namespace synthesis {
	class Mixer : public Module {
	public:
		Mixer();
		void generate_buf();

		enum BufTypes {
			AUDIO
		};

	protected:
		vector<const float_s*> in_bufs[1];
	};
}