#pragma once

#include "wavefolder.h"

namespace synthesis {
	class IdealWavefolder : public Wavefolder {
	public:
		IdealWavefolder();
		void generate_buf();
	};
}