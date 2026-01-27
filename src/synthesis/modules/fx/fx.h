#pragma once

#include "synthesis/modules/module.h"
#include "utils/global.h"
#include "utils/utils.h"

namespace synthesis {
	class Fx : public Module {
	public:
		float_s wet;

		Fx(std::vector<const float_s*>* in_bufs_ = nullptr);
		void mix_dry_wet();
		int add_input(Module* __restrict input, uint8_t buf_type);
		int add_input(MultichannelModule* __restrict input, uint8_t buf_type);
		virtual void add_buf(const float_s* __restrict buf, uint8_t buf_type) override;

		const float_s* audio_in_buf; // signal to be processed. there can be other buffers that provide supplementary info. ONLY ONE ALLOWED
		// add method to change source

	private:
		static constexpr uint8_t AUDIO{ 0 };
		static constexpr uint8_t WET{ 1 };
	};
}