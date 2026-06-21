#pragma once

#include "utils/global.h"
#include "mixer.h"
#include "module.h"

namespace synthesis {
	class Master : public Mixer {
	public:
		float_s* out_buf;

		Master();
		void generate_buf();
	};
}