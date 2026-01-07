#pragma once

#include "reverb.h"

#include "synthesis/modules/fx/components/delay_line.h"
#include "synthesis/modules/fx/components/multichannel_diffuser.h"
#include "synthesis/modules/mixer.h"

#include <array>

namespace synthesis {
	class Luff : public Reverb {
	public:
		Luff();
		void init() override;
		void generate_buf() override;

	private:
		MultichannelDiffuser<8> diffusers[3];
	};
}