#pragma once

#include "synthesis/modules/fx/distortion/wavefolder.h"
#include "serge_stage.h"

namespace synthesis {
	class SergeWavefolder : public Wavefolder {
	public:
		SergeWavefolder();
		void init();
		void generate_buf() override;

    private:
        std::array<std::unique_ptr<SergeStage>, 6> stages;
	};
}