#pragma once

#include "synthesis/modules/fx/distortion/wavefolder.h"

namespace synthesis {
    // based on https://www.mdpi.com/2076-3417/7/12/1328
    class SergeStage : public Fx {
    public:
        SergeStage();
        void generate_buf();

        enum BufTypes {
            AUDIO
        };

    private:
        static constexpr float_s anti_aliasing_thresh{ 1e-5f }; // TEMPORARY
        static constexpr float_s lambert_W_thresh{ 10e-12f };
        static constexpr float_s I_S = 2.52e-9f;
        static constexpr float_s eta = 1.752f;
        static constexpr float_s R_1 = 33000.0f;
        static constexpr float_s V_T = 25.864e-3f;
        float_s last_sample;
        std::vector<const float_s*> in_bufs[1];

        float_s f_(float_s x);
        float_s F_(float_s x);
        float_s W_(float_s x);
    };
}