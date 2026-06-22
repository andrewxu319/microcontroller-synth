#pragma once

#include "utils/global.h"
#include "utils/config.h"

#include <limits>
#include <unordered_map>
#include <array>

namespace synthesis {
	class MultichannelModule;

	class Module {
	public:
		std::vector<Module*> inputs;
		std::vector<Module*> outputs;
		static constexpr float_s EMPTY_BUF_MARKER{ std::numeric_limits<float_s>::min() };
		std::vector<const float_s*>* in_bufs_ptr; // make sure to initialize with in_bufs
		static const float_s empty_buf[config::buffer_size];
		size_t num_dependencies;
		size_t num_dependencies_visited;

		Module(std::vector<const float*>* in_bufs_ptr_ = nullptr);
		int add_input(Module* __restrict input, uint8_t buf_type = -1);
		int add_input(MultichannelModule* __restrict input, uint8_t buf_type = -1);
		int add_output(Module* __restrict output, uint8_t buf_type = -1);
		// implement remove input/output
		virtual void add_buf(const float_s* buf, uint8_t buf_type);
		// remove_buf
		virtual void init() {};

		virtual const float_s* get_out_buf() const; // read-only pointer
		virtual void generate_buf();
		bool sum_bufs(const uint8_t buf_type, float_s* dest); // use for control-rate modulations: only adding constant every control_rate samples => don't need vectorization
		bool sum_bufs(const uint8_t buf_type, float_s* dest, const float_s constant);

		virtual void note_on(const uint8_t note, const uint8_t velocity) {};
		virtual void note_off() {};
		virtual void change_note(const uint8_t note) {};

	protected:
		float_s out_buf[config::buffer_size];

	private:
		// to create modules in application.cpp and to store sub-modules in other modules, use unique_ptr instead
		Module(const Module&) = delete;
		Module& operator=(const Module&) = delete;
	};

	///////////////////////////////////////////////////////

	class MultichannelModule : public Module {
	public:
		using Buffer = std::array<float_s, config::buffer_size>;

		MultichannelModule(std::vector<const float*>* in_bufs_ptr_ = nullptr, uint8_t num_channels = 0);
		int add_output(Module* __restrict output, uint8_t buf_type);
		const std::vector<Buffer>& get_out_bufs() const; // read-only pointer
		void set_num_channels(uint8_t value);

	protected:
		std::vector<Buffer> out_bufs;

	private:
		const float_s* get_out_buf() = delete;
	};
}