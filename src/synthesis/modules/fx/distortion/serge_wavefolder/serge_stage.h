#pragma once

#include "synthesis/modules/fx/distortion/wavefolder.h"

#ifdef TEENSY
#undef B1
#else
#include <immintrin.h>
#endif

namespace synthesis {
    class SergeStage : public Fx {
    public:
        SergeStage();
        void generate_buf();

        enum BufTypes {
            AUDIO
        };

    private:
        static constexpr float_s V_in_scal = 0.3308173308173308f;
        static constexpr float_s anti_aliasing_thresh = 1e-1f / V_in_scal; // TEMPORARY

        // spline fit approximations of f_ and F_. see /other/serge_spline_approximations.ipynb for details
        // approximation works for -1000 < x < 1000, where x is the post-gain/offset input buffer
        // coefficients are in ascending order (a0, a1, ...)
        static constexpr float_s knot = 6.0f;

        // coefficients for spline approximation of f(x), |x| <= knot
        static constexpr float_s a1 = 1.5022530851068352f;
        static constexpr float_s a3 = -6.22729428e-01f;
        static constexpr float_s a5 = 1.07155879e-01f;
        static constexpr float_s a7 = -1.13339678e-02f;
        static constexpr float_s a9 = 7.36620422e-04f;
        static constexpr float_s a11 = -2.94621547e-05f;
        static constexpr float_s a13 = 7.05228624e-07f;
        static constexpr float_s a15 = -9.25031972e-09f;
        static constexpr float_s a17 = 5.10699803e-11f;
        // coefficients for spline approximation of f(x), |x| > knot
        static constexpr float_s b0 = 3.284071965157672;
        static constexpr float_s b1 = -1.3781155310417945;
        
        // coefficients for spline approximation of F(x), |x| <= knot
        static constexpr float_s A2 = a1 / 2.0f;
        static constexpr float_s A4 = a3 / 4.0f;
        static constexpr float_s A6 = a5 / 6.0f;
        static constexpr float_s A8 = a7 / 8.0f;
        static constexpr float_s A10 = a9 / 10.0f;
        static constexpr float_s A12 = a11 / 12.0f;
        static constexpr float_s A14 = a13 / 14.0f;
        static constexpr float_s A16 = a15 / 17.0f;
        static constexpr float_s A18 = a17 / 18.0f;
        // coefficients for spline approximation of F(x), |x| > knot
        static constexpr float_s B1 = b0;
        static constexpr float_s B2 = b1 / 2.0f;

        float_s last_sample;
        std::vector<const float_s*> in_bufs[1];

#ifndef TEENSY
        // all have FORCE_INLINE. i just need to put them in .cpp for some reason or there are errors
        __m256 f1(__m256 x) const;
        __m256 f02(__m256 x) const;
        __m256 F1(__m256 x) const;
        __m256 F02(__m256 x) const;
        __m256 branch_1(__m256 x, __m256 x_prev, __m256 avg_with_prev, __m256 diffs, __m256 aa_mask, int aa_mask_cumulative) const;
        __m256 branch_02(__m256 x, __m256 x_prev, __m256 avg_with_prev, __m256 diffs, __m256 aa_mask, int aa_mask_cumulative) const;
        __m256 compute_block(__m256 x, __m256 x_prev) const;
#endif
    };
}