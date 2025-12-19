// biquad, fir, lader, state variable

#pragma once

#undef _MSC_VER // otherwise dspfilters has tr1 issues

#include "utils/includes.h"
#include "utils/config.h"
#include "synthesis/modules/fx/fx.h"

#include "dspfilters/Dsp.h"

#include <cassert>

namespace synthesis {
	template <class Type, int Channels> // must be rbj. inputs can be mono or stereo (eventually)
	class Filter : public Fx {
	public:
		inline Filter() {
			params[0] = config::sample_rate;
			params[1] = 800;
			params[2] = 1.25;
		}

		inline void generate_buf() {
			if (in_buf->data[0] == EMPTY_BUF_MARKER) {
				out_buf[0] = EMPTY_BUF_MARKER;
				return;
			}

			memcpy(out_buf, in_buf->data, config::buffer_size * sizeof(float_s));

			float* data[1]{ out_buf }; // if mono. data[1] means array of 1 array pointer
			filter.process(config::actual_buffer_size, data);

			Fx::generate_buf();
		}

		inline void set_cutoff(double value) {
			assert(value > 0);
			cutoff = value;
			params[1] = value;
			filter.setParams(params);
		}

		inline void set_resonance(double value) {
			assert(value > 0);
			cutoff = value;
			params[2] = value;
			filter.setParams(params);
		}

		inline void set_band_width(double value) {
			set_resonance(value);
		}

	private:
		Dsp::SmoothedFilterDesign<Type, Channels> filter{ 1024 };
		Dsp::Params params{};
		double cutoff{};
		int band_width{};
		double resonance{};
	};
}

