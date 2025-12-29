// biquad, fir, lader, state variable

#pragma once

#undef _MSC_VER // otherwise dspfilters has tr1 issues

#include "utils/includes.h"
#include "utils/config.h"
#include "utils/accelerator.h"
#include "synthesis/modules/fx/fx.h"

#include "dspfilters/Dsp.h"

#include <cassert>

namespace synthesis {
	template <class Type, int Channels> // must be rbj. inputs can be mono or stereo (eventually)
	class Filter : public Fx {
	public:
		inline Filter()
			: Fx(in_bufs)
		{
			params[0] = config::sample_rate;
			params[1] = 800;
			params[2] = 1.0;
		}

		inline void generate_buf() override {
			if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
				out_buf[0] = EMPTY_BUF_MARKER;
				return;
			}

			memcpy(out_buf, audio_in_buf, config::buffer_size * sizeof(float_s));
			
			float_s cutoff_buf_sum[config::buffer_size];
			const bool cutoff_mods{ sum_bufs(BufTypes::CUTOFF, cutoff_buf_sum) };
			float_s resonance_buf_sum[config::buffer_size];
			const bool resonance_mods{ sum_bufs(BufTypes::RESONANCE, resonance_buf_sum) };
			float_s band_width_buf_sum[config::buffer_size];
			const bool band_width_mods{ sum_bufs(BufTypes::BAND_WIDTH, band_width_buf_sum) };

			for (int i{ 0 }; i < config::actual_buffer_size; i += config::control_rate) {
				int update_params{ false };
				if (cutoff_mods) {
					params[1] = cutoff + cutoff_buf_sum[i];
					update_params = true;
				}
				if (resonance_mods) {
					params[2] = resonance + resonance_buf_sum[i];
					update_params = true;
				}
				if (band_width_mods) {
					params[2] = band_width + band_width_buf_sum[i];
					update_params = true;
				}
				if (update_params) {
					filter.setParams(params);
				}
				float* data[1]{ out_buf + i }; // if mono. data[1] means array of 1 array pointer
				filter.process(config::control_rate, data);
			}

			mix_dry_wet();
		}

		inline void set_cutoff(double value) {
			assert(value > 0);
			cutoff = value;
			params[1] = value;
			filter.setParams(params);
		}

		inline void set_resonance(double value) {
			assert(value > 0);
			resonance = value;
			params[2] = value;
			filter.setParams(params);
		}

		inline void set_band_width(double value) {
			set_resonance(value);
		}

		enum BufTypes {
			AUDIO,
			WET,
			CUTOFF,
			RESONANCE,
			BAND_WIDTH
		};

	protected:
		vector<const float_s*> in_bufs[5]{};

	private:
		Dsp::FilterDesign<Type, Channels> filter{};
		Dsp::Params params{};
		double cutoff{};
		int band_width{};
		double resonance{};
	};
}

