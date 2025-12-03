#pragma once

#include "utils/includes.h"
#include "mixer.h"
#include "module.h"

namespace synthesis {
	class Master : public Mixer {
	public:
		float32_t* out_buf;

		Master();
		void generate_buf();
	};
}