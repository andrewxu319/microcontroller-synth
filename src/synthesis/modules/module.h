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
		vector<Module*>* mods_ptr;

		Module(vector<Module*>* mods_ = nullptr, const uint8_t num_mods = 0);
		Module(const utils::NoBaseInit); // dummy constructor
		virtual void generate_buf();
		void update_destination_bufs() const;
		float_s* get_mod_sum(const uint8_t mod);

		virtual int add_input(Module* __restrict input, bool add_buf);
		virtual int add_output(Module* __restrict output, bool add_buf);
		// implement remove input/output
		void attach_mod(Module* __restrict mod, uint8_t target);
		// detach_mod

		virtual void note_on(const uint8_t note, const uint8_t velocity);
		virtual void note_off();

	private:
		static int last_id;
	};
}