#pragma once

#include "utils/includes.h"
#include "utils/config.h"
#include "synthesis/modules/fx/fx.h"
#include "utils/circular_array.h"

namespace synthesis {
	class Flanger : public Fx {
	public:
		Flanger();
		void generate_buf() override;
		void set_offset(const double value);

		enum BufTypes {
			AUDIO,
			WET,
			OFFSET
		};

	protected:
		vector<const float_s*> in_bufs[3];

	private:
		utils::CircularArray<float_s> memory_buffer;
		size_t offset;
	};
}