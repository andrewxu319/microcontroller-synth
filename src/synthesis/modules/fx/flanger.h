#pragma once

#include "components/delay_line.h"

namespace synthesis {
	class Flanger : DelayLine {
	public:
		Flanger();
		void generate_buf() override;
	};
}