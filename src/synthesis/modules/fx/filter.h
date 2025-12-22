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
			: Fx(mods, sizeof(mods) / sizeof(vector<Module*>))
		{
			params[0] = config::sample_rate;
			params[1] = 800;
			params[2] = 1.0;
		}

		inline void generate_buf() override {
			if (audio_in_buf->data[0] == EMPTY_BUF_MARKER) {
				out_buf[0] = EMPTY_BUF_MARKER;
				return;
			}

			memcpy(out_buf, audio_in_buf->data, config::buffer_size * sizeof(float_s));

			for (int i{ 0 }; i < config::actual_buffer_size; i += config::control_rate) {
				int update_params{ false };
				if (!mods[Mods::CUTOFF].empty()) {
					params[1] = cutoff;
					for (const Module* module : mods[Mods::CUTOFF]) {
						params[1] += in_bufs[module->id].data[i];
					}
					update_params = true;
				}
				if (!mods[Mods::RESONANCE].empty()) {
					params[2] = resonance;
					for (const Module* module : mods[Mods::RESONANCE]) {
						params[2] += in_bufs[module->id].data[i];
					}
					update_params = true;
				}
				else if (!mods[Mods::BAND_WIDTH].empty()) {
					params[2] = band_width;
					for (const Module* module : mods[Mods::BAND_WIDTH]) {
						band_width += in_bufs[module->id].data[i];
					}
					update_params = true;
				}
				if (update_params) {
					filter.setParams(params);
				}
				float* data[1]{ out_buf + i }; // if mono. data[1] means array of 1 array pointer
				filter.process(config::control_rate, data);
			}

			mix_dry_wet(Mods::WET);
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

		enum Mods {
			WET,
			CUTOFF,
			RESONANCE,
			BAND_WIDTH
		};

	private:
		Dsp::FilterDesign<Type, Channels> filter{};
		Dsp::Params params{};
		double cutoff{};
		int band_width{};
		double resonance{};
		vector<Module*> mods[4]{};
	};
}

