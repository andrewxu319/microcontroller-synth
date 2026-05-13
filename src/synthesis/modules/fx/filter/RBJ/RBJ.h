// see https://www.youtube.com/watch?v=rDERCmBAv3I

#pragma once

#include "synthesis/modules/fx/filter/filter.h"

#include <cmath>

namespace synthesis::RBJFilter {
	template <typename Derived>
	class RBJ : public Filter {
	public:
		RBJ();
		void generate_buf() override;
		void set_cutoff(double value);
		
	protected:
		void set_qbs(double value);
		float_s a1, a2, b0, b1, b2, w1, w2;
		float_s cos_omega, sin_omega, alpha; // effective values (modulated)
	};
}

using namespace synthesis::RBJFilter;

template <typename Derived>
RBJ<Derived>::RBJ()
	: Filter(), a1{}, a2{}, b0{}, b1{}, b2{}, w1{}, w2{}, cos_omega{ 1 }, sin_omega{ 0 }, alpha{}
{
	;
}

template <typename Derived>
void RBJ<Derived>::generate_buf() {
	if (audio_in_buf[0] == EMPTY_BUF_MARKER) {
		out_buf[0] = EMPTY_BUF_MARKER;
		return;
	}

	float_s cutoff_buf_sum[config::buffer_size];
	const bool cutoff_mods{ sum_bufs(BufType::CUTOFF, cutoff_buf_sum, cutoff) };
	float_s qbs_buf_sum[config::buffer_size];
	const bool qbs_mods{ sum_bufs(BufType::QBS, qbs_buf_sum, qbs) }; // FIX LATER

	if (!cutoff_mods && !qbs_mods && !qbs_mods) {
		for (size_t j{}; j < config::buffer_size; j++) {
			out_buf[j] = b0 * audio_in_buf[j] + w1;
			w1 = b1 * audio_in_buf[j] - a1 * out_buf[j] + w2;
			w2 = b2 * audio_in_buf[j] - a2 * out_buf[j];
		}
	}
	else {
		for (size_t i{}; i < config::buffer_size; i += config::control_rate) {
			for (size_t j{ i }; j < i + config::control_rate; j++) {
				out_buf[j] = b0 * audio_in_buf[j] + w1;
				w1 = b1 * audio_in_buf[j] - a1 * out_buf[j] + w2;
				w2 = b2 * audio_in_buf[j] - a2 * out_buf[j];
			}

			effective_omega = 2 * M_PI / static_cast<double>(config::sample_rate) * cutoff_buf_sum[i];
			cos_omega = std::cos(effective_omega);
			sin_omega = std::sin(effective_omega);
			effective_qbs = qbs_buf_sum[i];
			static_cast<Derived*>(this)->compute_alpha();
			static_cast<Derived*>(this)->compute_coefficients();
		}
	}

	mix_dry_wet();
}

template <typename Derived>
void RBJ<Derived>::set_cutoff(double value) {
	Filter::set_cutoff(value);
	cos_omega = std::cos(effective_omega);
	sin_omega = std::sin(effective_omega);
	static_cast<Derived*>(this)->compute_alpha();
	static_cast<Derived*>(this)->compute_coefficients();
}

template <typename Derived>
void RBJ<Derived>::set_qbs(double value) {
	Filter::set_qbs(value);
	static_cast<Derived*>(this)->compute_alpha();
	static_cast<Derived*>(this)->compute_coefficients();
}