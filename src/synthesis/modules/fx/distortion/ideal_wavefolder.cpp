#include "ideal_wavefolder.h"

#include "utils/math.h"

using namespace synthesis;

IdealWavefolder::IdealWavefolder()
{
}

void IdealWavefolder::generate_buf() {
    Wavefolder::generate_buf();
    for (size_t i{}; i < config::buffer_size; i++) {
        if (out_buf[i] > 1.0f) {
            out_buf[i] = 2.0f - out_buf[i];
        }
        if (out_buf[i] < -1.0f) {
            out_buf[i] = -2.0f - out_buf[i];
        }
    }

    mix_dry_wet();
	Module::generate_buf();
}

