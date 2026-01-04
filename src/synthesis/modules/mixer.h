#pragma once

#include "module.h"

#include <unordered_map>

namespace synthesis {
	class Mixer : public Module {
	public:
		Mixer();
		void generate_buf() override;
		void add_buf(const float_s* __restrict buf, uint8_t buf_type) override;
		void set_in_buf_gain(const float_s* __restrict in_buf, const float_s value);

		enum BufTypes {
			AUDIO
		};

	protected:
		vector<const float_s*> in_bufs[1];
		unordered_map<const float_s*, float_s> gains; // indexed by buffer pointer
	};
}