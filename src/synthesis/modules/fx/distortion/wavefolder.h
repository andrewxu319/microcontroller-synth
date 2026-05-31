#pragma once

#include "synthesis/modules/fx/fx.h"

namespace synthesis {
    class Wavefolder : public Fx {
    public:
        Wavefolder();
        virtual void generate_buf() override;
        void set_gain(float_s value);
        void set_offset(float_s value);

		enum BufType {
			AUDIO,
			GAIN,
            OFFSET
		};

	protected:
		std::vector<const float_s*> in_bufs[3];
		float_s gain;
        float_s offset;
    };
}