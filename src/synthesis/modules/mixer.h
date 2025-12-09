#pragma once

#include "module.h"

namespace synthesis {
	class Mixer : public Module {
	public:
		Mixer();
		Mixer(const utils::NoBaseInit); // dummy constructor
		void generate_buf();
	};
}