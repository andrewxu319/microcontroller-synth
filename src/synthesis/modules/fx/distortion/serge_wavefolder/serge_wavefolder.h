#pragma once

#include "synthesis/modules/fx/distortion/wavefolder.h"
#include "serge_stage.h"

namespace synthesis {
    // based on https://www.mdpi.com/2076-3417/7/12/1328
	class SergeWavefolder : public Wavefolder {
	public:
		SergeWavefolder();
		void init();
		void generate_buf() override;

    private:
        static constexpr float_s V_in_scal = 0.3308173308173308f; //  input scaling factor. see /other/wright_omega_approx.ipynb for derivation
        static constexpr float_s V_out_scal = 4.163372493981558f; // output scaling factor. see /other/wright_omega_approx.ipynb for derivation
        std::array<std::unique_ptr<SergeStage>, 6> stages;
	};
}