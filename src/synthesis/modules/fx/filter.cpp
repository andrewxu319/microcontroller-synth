//#include "filter.h"
//
//using namespace synthesis;
//
//Filter::Filter(const int channels_)
//	: Module(),
//	filter{},
//	channels{ channels_ },
//	type{ FilterType::LowPass },
//	order{},
//	cutoff{},
//	band_width{},
//	resonance{}
//{
//	//filter = static_cast<unique_ptr<void*>>(make_unique<Module>{ Module() });
//	filter->setParams(Dsp::Params{
//		config::sample_rate,
//		cutoff,
//		resonance
//	});
//}
//
//void Filter::set_order() {
//	;
//}
//
////void Filter::set_cutoff();
////void Filter::set_band_width();
////void Filter::set_resonance();