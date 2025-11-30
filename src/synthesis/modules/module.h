#pragma once

#include "utils/utils.h"
#include "utils/config.h"

#include <vector>
#include <array>
#include <unordered_map>

namespace synth {
	class Module {
	protected:
		struct InputData {
			buffer in_buf;
			// add later: a modulation relation (what does it change?)
		};

	public:
		const int id{};
		std::vector<Module*> inputs;
		std::vector<Module*> outputs;
		std::unordered_map<int, InputData> input_data{};
		buffer* out_buf;

		Module();
		Module(const NoBaseInit); // dummy constructor
		virtual void generate_buf();

	protected:
		void update_destination_bufs() const;
		void init();
	};
}