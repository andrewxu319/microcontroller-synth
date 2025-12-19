#pragma once

#include "synthesis/modules/module.h"
#include "utils/includes.h"
#include "utils/utils.h"

namespace synthesis {
	class Fx : public Module {
	public:
		float_s wet;

		Fx();
		void generate_buf() override;
		int add_input(Module* __restrict input, bool add_buf) override;

	protected:
		utils::array_wrapper<float_s, config::buffer_size>* in_buf; // signal to be processed. there can be other buffers that provide supplementary info. ONLY ONE ALLOWED
		// add method to change source
	};
}