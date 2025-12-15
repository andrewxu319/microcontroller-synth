//#pragma once
//
//#include "utils/includes.h"
//#include "utils/config.h"
//#include "synthesis/modules/module.h"
//
//#include "dspfilters/Dsp.h"
//
//namespace synthesis {
//	class Filter : public Module {
//	public:
//		Filter(const int channels_);
//		void generate_buf();
//
//		void set_order();
//		void set_cutoff();
//		void set_band_width();
//		void set_resonance();
//
//		enum class FilterType {
//			LowPass,
//			HighPass,
//			BandPass
//		};
//
//	private:
//		unique_ptr<void*> filter;
//		int channels; // inputs can be mono or stereo
//		FilterType type;
//		int order;
//		double cutoff;
//		int band_width;
//		double resonance;
//	};
//}
//
