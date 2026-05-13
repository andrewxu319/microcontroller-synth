#pragma once

#include "synthesis/modules/fx/filter/RBJ/allpass.h"

namespace synthesis {
    class PhaserAllpass : public RBJFilter::Allpass {
    public:
        float_s generate_one_sample(float_s input);
        void set_cutoff(double value);
    };
}