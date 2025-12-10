#pragma once

#include "utils/includes.h"
#include "mixer.h"
#include "module.h"

namespace synthesis {
	class Master : public Mixer {
	public:
		float_s* out_buf;

		static Master& instance();
		void generate_buf();
		
	private:
		Master();
		Master(const Master&) = delete;
		Master& operator =(const Master&) = delete;
	};
}