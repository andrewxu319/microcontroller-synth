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
		unordered_map<int, array_wrapper<float32_t, config::buffer_size>> in_bufs{};
			// any other messages e.g. modulation bindings should be done in other ways (e.g. have modulation member variables / a map)
		float32_t* out_buf;

		Module(const vector<Module*>& outputs);
		Module(const NoBaseInit); // dummy constructor
		virtual void generate_buf();

		void add_input(Module* input);
		//void add_inputs(vector<Module*> inputs);
		virtual void add_output(Module* output);
		void add_outputs(vector<Module*> outputs);
		// implement remove input/output

	protected:
		void update_destination_bufs() const;

	private:
		static int last_id;
	};
}