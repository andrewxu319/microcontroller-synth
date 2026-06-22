#pragma once

#include "RBJ.h"
#include "synthesis/modules/module.h"

namespace synthesis::RBJFilter {
    template <typename Derived>
	class ShelfFilter : public RBJ<ShelfFilter<Derived>> {
	public:
        ShelfFilter();
        void generate_buf() override;
        void set_slope(double value_dB_per_octave);
        void set_gain(double value_dB);
		void compute_alpha();
        void compute_coefficients();

        enum BufType {
			AUDIO,
            WET,
            CUTOFF,
            SLOPE,
            GAIN
		};

    protected:
        float_s gain;
        float_s effective_gain;
        float_s A; // effective value
	};
}

using namespace synthesis::RBJFilter;

template<typename Derived>
ShelfFilter<Derived>::ShelfFilter() 
    : gain{}, effective_gain{}, A{ 1.0f }
{
    Filter::set_qbs(1.0f);
}

template<typename Derived>
void ShelfFilter<Derived>::generate_buf() {
	if (this->audio_in_buf[0] == this->EMPTY_BUF_MARKER) {
		this->out_buf[0] = this->EMPTY_BUF_MARKER;
		return;
	}

	float_s cutoff_buf_sum[config::buffer_size];
	bool cutoff_mods{ this->sum_bufs(BufType::CUTOFF, cutoff_buf_sum, this->cutoff) };
	float_s slope_buf_sum[config::buffer_size]; // qbs
	bool slope_mods{ this->sum_bufs(BufType::SLOPE, slope_buf_sum, this->qbs) };
    float_s gain_buf_sum[config::buffer_size];
    bool gain_mods{ this->sum_bufs(BufType::GAIN, gain_buf_sum, this->gain) };

	if (!cutoff_mods && !slope_mods && !gain_mods) {
		for (size_t j{}; j < config::buffer_size; j++) {
			this->out_buf[j] = this->b0 * this->audio_in_buf[j] + this->w1;
			this->w1 = this->b1 * this->audio_in_buf[j] - this->a1 * this->out_buf[j] + this->w2;
			this->w2 = this->b2 * this->audio_in_buf[j] - this->a2 * this->out_buf[j];
		}
	}
	else {
		for (size_t i{}; i < config::buffer_size; i += config::control_rate) {
			if (cutoff_mods) {
				this->effective_omega = 2 * M_PI / static_cast<double>(config::sample_rate) * cutoff_buf_sum[i];
				this->cos_omega = std::cos(this->effective_omega);
				this->sin_omega = std::sin(this->effective_omega);
			}
			if (slope_mods) {
				this->effective_qbs = slope_buf_sum[i];
			}
            if (gain_mods) {
                effective_gain = gain_buf_sum[i];
            }
			static_cast<Derived*>(this)->compute_alpha();
			static_cast<Derived*>(this)->compute_coefficients();

			for (size_t j{ i }; j < i + config::control_rate; j++) {
				this->out_buf[j] = this->b0 * this->audio_in_buf[j] + this->w1;
				this->w1 = this->b1 * this->audio_in_buf[j] - this->a1 * this->out_buf[j] + this->w2;
				this->w2 = this->b2 * this->audio_in_buf[j] - this->a2 * this->out_buf[j];
			}
		}
	}

	this->mix_dry_wet();
	Module::generate_buf();
}

template<typename Derived>
void ShelfFilter<Derived>::set_slope(double value_dB_per_octave) {
    // max is 1.0
    this->set_qbs(value_dB_per_octave);
}

// negative value for shelf
template<typename Derived>
void ShelfFilter<Derived>::set_gain(double value_dB) {
    gain = value_dB;
    effective_gain = value_dB; // remove this???
    compute_alpha();
    static_cast<Derived*>(this)->compute_coefficients();
}

template<typename Derived>
void ShelfFilter<Derived>::compute_alpha() {
    A = pow(10.0, effective_gain / 40.0f);
    this->alpha = this->sin_omega / 2.0 * std::sqrt((A + 1.0f / A) * (1.0f / this->effective_qbs - 1) + 2.0f);
}

template<typename Derived>
void ShelfFilter<Derived>::compute_coefficients() {
    static_cast<Derived*>(this)->compute_coefficients();
}