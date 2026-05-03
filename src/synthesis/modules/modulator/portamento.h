#pragma once

#include "synthesis/modules/module.h"
#include "utils/global.h"

namespace synthesis {
	class Portamento : public Module {
	public:
		Portamento();
		void generate_buf() override;
		void change_note(const uint8_t note) override;
		void set_time(double time_ms);

	private:
		double time;
	};
}