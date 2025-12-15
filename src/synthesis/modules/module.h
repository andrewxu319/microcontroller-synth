#pragma once

#include "utils/includes.h"
#include "utils/typedefs.h"
#include "utils/utils.h"
#include "utils/config.h"

#include <unordered_map>
#include <array>

namespace synthesis {
	class Module {
	public:
		const int id;
		vector<Module*> inputs;
		vector<Module*> outputs;
		unordered_map<int, utils::array_wrapper<float_s, config::buffer_size>> in_bufs{};
			// any other messages e.g. modulation bindings should be done in other ways (e.g. have modulation member variables / a map)
		float_s* out_buf;
		static constexpr float_s EMPTY_BUF_MARKER{ numeric_limits<float_s>::min()};

		Module();
		Module(size_t input_limit_);
		Module(const utils::NoBaseInit); // dummy constructor
		virtual void generate_buf();

		virtual int add_input(Module* __restrict input, bool add_buf);
		int add_output(Module* __restrict output, bool add_buf);
		// implement remove input/output

	protected:
		void update_destination_bufs() const;
		const size_t input_limit;

	private:
		static int last_id;
	};
}