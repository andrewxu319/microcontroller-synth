#pragma once

#include "utils/global.h"
#include "utils/utils.h"
#include "utils/config.h"

#include <unordered_map>
#include <array>

namespace synthesis {
	class MultichannelModule;

	class Module {
	public:
		const int id;
		vector<Module*> inputs;
		vector<Module*> outputs;
		static constexpr float_s EMPTY_BUF_MARKER{ numeric_limits<float_s>::min()};
		vector<const float_s*>* in_bufs_ptr;
		static const float_s empty_buf[config::buffer_size];

		Module(vector<const float*>* in_bufs_ptr_ = nullptr);
		int add_input(Module* __restrict input, uint8_t buf_type = -1);
		int add_input(MultichannelModule* __restrict input, uint8_t buf_type = -1);
		int add_output(Module* __restrict output, uint8_t buf_type = -1);
		// implement remove input/output
		virtual void add_buf(const float_s* __restrict buf, uint8_t buf_type);
		// remove_buf
		virtual void init() {};

		virtual const float_s* get_out_buf() const; // read-only pointer
		virtual void generate_buf();
		bool sum_bufs(const uint8_t buf_type, float_s* dest); // use for control-rate modulations: only adding constant every control_rate samples => don't need vectorization
		bool sum_bufs(const uint8_t buf_type, float_s* dest, const float_s constant);

		virtual void note_on(const uint8_t note, const uint8_t velocity);
		virtual void note_off();

	protected:
		float_s out_buf[config::buffer_size];

	private:
		static int last_id;

		// use unique_ptr instead
		Module(const Module&) = delete;
		Module& operator=(const Module&) = delete;
	};

	///////////////////////////////////////////////////////

	class MultichannelModule : public Module {
	public:
		using Buffer = array<float_s, config::buffer_size>;

		MultichannelModule(vector<const float*>* in_bufs_ptr_ = nullptr, uint8_t num_channels = 0);
		int add_output(Module* __restrict output, uint8_t buf_type);
		const vector<Buffer>& get_out_bufs() const; // read-only pointer
		void set_num_channels(uint8_t value);

	protected:
		vector<Buffer> out_bufs;

	private:
		const float_s* get_out_buf() = delete;
	};
}