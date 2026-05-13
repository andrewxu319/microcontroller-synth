#pragma once

#include "synthesis/modules/module.h"
#include "utils/global.h"
#include "utils/utils.h"

namespace synthesis {
	class Fx : public Module {
	public:
		Fx(std::vector<const float_s*>* in_bufs_ = nullptr);
		int add_input(Module* __restrict input, uint8_t buf_type);
		int add_input(MultichannelModule* __restrict input, uint8_t buf_type);
		virtual void add_buf(const float_s* buf, uint8_t buf_type) override;
		// add method to change source
		void set_wet(float_s value);

		const float_s* audio_in_buf; // signal to be processed. there can be other buffers that provide supplementary info. ONLY ONE ALLOWED

	protected:
		void mix_dry_wet();		
		float_s wet;

	private:
		static constexpr uint8_t AUDIO{ 0 };
		static constexpr uint8_t WET{ 1 };
	};
}