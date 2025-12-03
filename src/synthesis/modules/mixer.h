#pragma once

#include "module.h"

namespace synthesis {
	class Mixer : public Module {
	public:
		Mixer(const vector<Module*> outputs_);
		Mixer(const NoBaseInit); // dummy constructor
		void generate_buf();
	};
}