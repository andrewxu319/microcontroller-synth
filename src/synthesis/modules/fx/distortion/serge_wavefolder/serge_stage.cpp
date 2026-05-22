// this is probably the most fun thing i've ever written

#include "serge_stage.h"

#include "utils/math.h"
#include "utils/timer.h"

#include <cmath>
#include <cstdio>

using namespace synthesis;

SergeStage::SergeStage()
    : Fx(in_bufs), last_sample{}
{
}

#ifndef TEENSY
    // FORCE_INLINE https://meghprkh.github.io/blog/posts/c++-force-inline/
    #if defined(__clang__)
        #define FORCE_INLINE [[gnu::always_inline]] [[gnu::gnu_inline]] extern inline
    #elif defined(__GNUC__)
        #define FORCE_INLINE [[gnu::always_inline]] inline
    #elif defined(_MSC_VER)
        #pragma warning(error: 4714)
        #define FORCE_INLINE [[msvc::forceinline]] inline
    #else
        //#error Unsupported compiler
    #endif

FORCE_INLINE __m256 SergeStage::f1(__m256 x) const {
    const __m256 a1_{ _mm256_set1_ps(a1) };
    const __m256 a3_{ _mm256_set1_ps(a3) };
    const __m256 a5_{ _mm256_set1_ps(a5) };
    const __m256 a7_{ _mm256_set1_ps(a7) };
    const __m256 a9_{ _mm256_set1_ps(a9) };
    const __m256 a11_{ _mm256_set1_ps(a11) };
    const __m256 a13_{ _mm256_set1_ps(a13) };
    const __m256 a15_{ _mm256_set1_ps(a15) };
    const __m256 a17_{ _mm256_set1_ps(a17) };

    // horner's method
    __m256 x_sq{ _mm256_mul_ps(x, x) };
    __m256 r15{ _mm256_fmadd_ps(x_sq, a17_, a15_) };
    __m256 r13{ _mm256_fmadd_ps(x_sq, r15, a13_) };
    __m256 r11{ _mm256_fmadd_ps(x_sq, r13, a11_) };
    __m256 r9{ _mm256_fmadd_ps(x_sq, r11, a9_) };
    __m256 r7{ _mm256_fmadd_ps(x_sq, r9, a7_) };
    __m256 r5{ _mm256_fmadd_ps(x_sq, r7, a5_) };
    __m256 r3{ _mm256_fmadd_ps(x_sq, r5, a3_) };
    __m256 r1{ _mm256_fmadd_ps(x_sq, r3, a1_) };
    return _mm256_mul_ps(x, r1);
}

FORCE_INLINE __m256 SergeStage::f02(__m256 x) const {
    const __m256 b0_{ _mm256_set1_ps(b0) };
    const __m256 b0_neg_{ _mm256_set1_ps(-b0)};
    const __m256 b1_{ _mm256_set1_ps(b1) };

    __m256 f0_result{ _mm256_fmadd_ps(x, b1_, b0_neg_) };
    __m256 f2_result{ _mm256_fmadd_ps(x, b1_, b0_) };

    // blend f0 & f2
    const __m256 zero_reg{ _mm256_setzero_ps() };
    __m256 f02_mask{ _mm256_cmp_ps(x, zero_reg, _CMP_GT_OQ) };
    return _mm256_blendv_ps(f0_result, f2_result, f02_mask);
}

FORCE_INLINE __m256 SergeStage::F1(__m256 x) const {
    const __m256 A2_{ _mm256_set1_ps(A2) };
    const __m256 A4_{ _mm256_set1_ps(A4) };
    const __m256 A6_{ _mm256_set1_ps(A6) };
    const __m256 A8_{ _mm256_set1_ps(A8) };
    const __m256 A10_{ _mm256_set1_ps(A10) };
    const __m256 A12_{ _mm256_set1_ps(A12) };
    const __m256 A14_{ _mm256_set1_ps(A14) };
    const __m256 A16_{ _mm256_set1_ps(A16) };
    const __m256 A18_{ _mm256_set1_ps(A18) };

    // horner's method
    __m256 x_sq{ _mm256_mul_ps(x, x) };
    __m256 r16{ _mm256_fmadd_ps(x_sq, A18_, A16_) };
    __m256 r14{ _mm256_fmadd_ps(x_sq, r16, A14_) };
    __m256 r12{ _mm256_fmadd_ps(x_sq, r14, A12_) };
    __m256 r10{ _mm256_fmadd_ps(x_sq, r12, A10_) };
    __m256 r8{ _mm256_fmadd_ps(x_sq, r10, A8_) };
    __m256 r6{ _mm256_fmadd_ps(x_sq, r8, A6_) };
    __m256 r4{ _mm256_fmadd_ps(x_sq, r6, A4_) };
    __m256 r2{ _mm256_fmadd_ps(x_sq, r4, A2_) };
    return _mm256_mul_ps(x_sq, r2);
}

FORCE_INLINE __m256 SergeStage::F02(__m256 x) const {
    const __m256 B1_{ _mm256_set1_ps(B1) };
    const __m256 B1_neg_{ _mm256_set1_ps(-B1) };
    const __m256 B2_{ _mm256_set1_ps(B2) };

    __m256 r1_0{ _mm256_fmadd_ps(x, B2_, B1_neg_) };
    __m256 F0_result{ _mm256_mul_ps(x, r1_0) };
    
    __m256 r1_2{ _mm256_fmadd_ps(x, B2_, B1_) };
    __m256 F2_result{ _mm256_mul_ps(x, r1_2) };

    // blend F0 & F2
    const __m256 zero_reg{ _mm256_setzero_ps() };
    __m256 F02_mask{ _mm256_cmp_ps(x, zero_reg, _CMP_GT_OQ) };
    return _mm256_blendv_ps(F0_result, F2_result, F02_mask);
}

FORCE_INLINE __m256 SergeStage::branch_1(__m256 x, __m256 x_prev, __m256 avg_with_prev, __m256 diffs, __m256 aa_mask, int aa_mask_cumulative) const {
    // evaluate f1 only
    if (aa_mask_cumulative == 0x00) {
        return f1(avg_with_prev);
    }
    // evaluate F1 only
    else if (aa_mask_cumulative == 0xFF) {
        __m256 F1_result{ F1(x) }; // use abs because F is even
        __m256 F1_prev_result{ F1(x_prev) };
        __m256 F1_diffs{ _mm256_sub_ps(F1_result, F1_prev_result) };
        return _mm256_div_ps(F1_diffs, diffs);
    }
    // evaluate f1 & F1, then blend
    else {
        // f1
        __m256 f1_branch_result{ f1(avg_with_prev) };

        // F1
        // for later: avoid div by 0 here
        __m256 F1_result{ F1(x) };
        __m256 F1_prev_result{ F1(x_prev) };
        __m256 F1_diffs{ _mm256_sub_ps(F1_result, F1_prev_result) };
        __m256 F1_branch_result{ _mm256_div_ps(F1_diffs, diffs) };

        // blend f1 & F1
        return _mm256_blendv_ps(f1_branch_result, F1_branch_result, aa_mask);
    }
}

// "branch 02" means the union of the branch containing f0/F0 (left segment) and the branch containing f2/F2 (right segment)
FORCE_INLINE __m256 SergeStage::branch_02(__m256 x, __m256 x_prev, __m256 avg_with_prev, __m256 diffs, __m256 aa_mask, int aa_mask_cumulative) const {
    // evaluate f02 only
    if (aa_mask_cumulative == 0x00) {
        return f02(avg_with_prev);
    }
    // evaluate F02 only
    else if (aa_mask_cumulative == 0xFF) {
        __m256 F02_result{ F02(x) }; // use abs because F is even
        __m256 F02_prev_result{ F02(x_prev) };
        __m256 F02_diffs{ _mm256_sub_ps(F02_result, F02_prev_result) };
        return _mm256_div_ps(F02_diffs, diffs);
    }
    // evaluate f02 & F02, then blend
    else {
        // f02
        __m256 f02_branch_result{ f02(avg_with_prev) };

        // F02
        // for later: avoid div by 0 here
        __m256 F02_result{ F02(x) };
        __m256 F02_prev_result{ F02(x_prev) };
        __m256 F02_diffs{ _mm256_sub_ps(F02_result, F02_prev_result) };
        __m256 F02_branch_result{ _mm256_div_ps(F02_diffs, diffs) };

        // blend f02 & F02
        return _mm256_blendv_ps(f02_branch_result, F02_branch_result, aa_mask);
    }
}

FORCE_INLINE __m256 SergeStage::compute_block(__m256 x, __m256 x_prev) const {
    const __m256 SIGN_BIT_MASK{ _mm256_set1_ps(-0.0f) };

    // absolute values
    __m256 x_abs{ _mm256_andnot_ps(SIGN_BIT_MASK, x) };
    __m256 x_prev_abs{ _mm256_andnot_ps(SIGN_BIT_MASK, x_prev) };

    // precompute values required by the 02 branches
    __m256 sum_with_prev{ _mm256_add_ps(x, x_prev) };
    const __m256 half_reg{ _mm256_set1_ps(0.5f) };
    __m256 avg_with_prev{ _mm256_mul_ps(sum_with_prev, half_reg) };

    // absolute differences
    __m256 diffs{ _mm256_sub_ps(x, x_prev) };
    __m256 diffs_abs{ _mm256_andnot_ps(SIGN_BIT_MASK, diffs) };

    // decide what to evaluate: f1, f02, F1, F02
    const __m256 aa_thresh_reg{ _mm256_set1_ps(anti_aliasing_thresh) };
    __m256 aa_mask{ _mm256_cmp_ps(diffs_abs, aa_thresh_reg, _CMP_GT_OQ) }; // 0xFF if x > anti_aliasing_thresh, 0x00 otherwise
    int aa_mask_cumulative{ _mm256_movemask_ps(aa_mask) };
    const __m256 knot_reg{ _mm256_set1_ps(knot) };
    __m256 pw_mask{ _mm256_cmp_ps(x_abs, knot_reg, _CMP_GT_OQ) }; // 0xFF if x > knot, 0x00 otherwise
    int pw_mask_cumulative{ _mm256_movemask_ps(pw_mask) };
    
    // evaluate f1 and/or F1
    if (pw_mask_cumulative == 0x00) {
        return branch_1(x, x_prev, avg_with_prev, diffs, aa_mask, aa_mask_cumulative);
    }
    // evaluate f02 and/or F02
    if (pw_mask_cumulative == 0xFF) {
        return branch_02(x, x_prev, avg_with_prev, diffs, aa_mask, aa_mask_cumulative);
    }
    // evaluate both
    __m256 branch_1_result{ branch_1(x, x_prev, avg_with_prev, diffs, aa_mask, aa_mask_cumulative) };
    __m256 branch_02_result{ branch_02(x, x_prev, avg_with_prev, diffs, aa_mask, aa_mask_cumulative) };
    return _mm256_blendv_ps(branch_1_result, branch_02_result, pw_mask);
}

void SergeStage::generate_buf() {
    // first block of 8
    __m256 x{ _mm256_loadu_ps(audio_in_buf) };
    __m256i permutation_mask{ _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6) };
    __m256 suffix{ _mm256_permutevar8x32_ps(x, permutation_mask) };
    __m256 prefix{ _mm256_castps128_ps256(_mm_set_ss(last_sample)) }; // cast from _m128 to _m256
    __m256 x_prev{ _mm256_blend_ps(suffix, prefix, 0b00000001) };
    _mm256_storeu_ps(out_buf, compute_block(x, x_prev));

    for (size_t i{ 8 }; i < config::buffer_size; i += 8) {
        // load input
        __m256 x{ _mm256_loadu_ps(audio_in_buf + i) };
        __m256 x_prev{ _mm256_loadu_ps(audio_in_buf + i - 1) };

        _mm256_storeu_ps(out_buf + i, compute_block(x, x_prev));
    }

    last_sample = audio_in_buf[config::buffer_size - 1];
}
#endif