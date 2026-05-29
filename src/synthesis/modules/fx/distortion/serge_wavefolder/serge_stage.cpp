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
#endif

#ifdef TEENSY
;

#elif defined(__AVX512F__)

FORCE_INLINE __m512 SergeStage::f1(__m512 src, __mmask16 mask, __m512 x) const {
    const __m512 a1_{ _mm512_set1_ps(a1) };
    const __m512 a3_{ _mm512_set1_ps(a3) };
    const __m512 a5_{ _mm512_set1_ps(a5) };
    const __m512 a7_{ _mm512_set1_ps(a7) };
    const __m512 a9_{ _mm512_set1_ps(a9) };
    const __m512 a11_{ _mm512_set1_ps(a11) };
    const __m512 a13_{ _mm512_set1_ps(a13) };
    const __m512 a15_{ _mm512_set1_ps(a15) };
    const __m512 a17_{ _mm512_set1_ps(a17) };

    // horner's method
    __m512 x_sq{ _mm512_mask_mul_ps(src, mask, x, x) }; // x_sq now acts as the "src" because it contains the original src entries and mask fma requires the src to be an operand in the fma
    __m512 r15{ _mm512_mask_fmadd_ps(x_sq, mask, a17_, a15_) };
    __m512 r13{ _mm512_mask_fmadd_ps(x_sq, mask, r15, a13_) };
    __m512 r11{ _mm512_mask_fmadd_ps(x_sq, mask, r13, a11_) };
    __m512 r9{ _mm512_mask_fmadd_ps(x_sq, mask, r11, a9_) };
    __m512 r7{ _mm512_mask_fmadd_ps(x_sq, mask, r9, a7_) };
    __m512 r5{ _mm512_mask_fmadd_ps(x_sq, mask, r7, a5_) };
    __m512 r3{ _mm512_mask_fmadd_ps(x_sq, mask, r5, a3_) };
    __m512 r1{ _mm512_mask_fmadd_ps(x_sq, mask, r3, a1_) };
    return _mm512_mask_mul_ps(src, mask, x, r1);
}

FORCE_INLINE __m512 SergeStage::f02(__m512 src, __mmask16 mask, __m512 x) const {
    const __m512 b0_{ _mm512_set1_ps(b0) };
    const __m512 b0_neg_{ _mm512_set1_ps(-b0) };
    const __m512 b1_{ _mm512_set1_ps(b1) };

    const __m512 zero_reg{ _mm512_setzero_ps() };
    __mmask16 f02_mask{ _mm512_mask_cmp_ps_mask(mask, x, zero_reg, _CMP_GT_OQ) };

    // can't use fma because the mask is not also an operand
    __m512 x_times_b1{ _mm512_mask_mul_ps(src, mask, x, b1_) };
    __m512 f0{ _mm512_mask_add_ps(src, mask, x_times_b1, b0_neg_) }; // don't need mask for first segment. second segment will override
    return _mm512_mask_add_ps(f0, f02_mask, x_times_b1, b0_);

    // CHECK CORRECTNESS AT THE END
}

FORCE_INLINE __m512 SergeStage::F1(__m512 src, __mmask16 mask, __m512 x) const {
    const __m512 A2_{ _mm512_set1_ps(A2) };
    const __m512 A4_{ _mm512_set1_ps(A4) };
    const __m512 A6_{ _mm512_set1_ps(A6) };
    const __m512 A8_{ _mm512_set1_ps(A8) };
    const __m512 A10_{ _mm512_set1_ps(A10) };
    const __m512 A12_{ _mm512_set1_ps(A12) };
    const __m512 A14_{ _mm512_set1_ps(A14) };
    const __m512 A16_{ _mm512_set1_ps(A16) };
    const __m512 A18_{ _mm512_set1_ps(A18) };

    // horner's method
    __m512 x_sq{ _mm512_mul_ps(x, x) }; // x_sq now acts as the "src" because it contains the original src entries and mask fma requires the src to be an operand in the fma
    __m512 r16{ _mm512_mask_fmadd_ps(x_sq, mask, A18_, A16_) };
    __m512 r14{ _mm512_mask_fmadd_ps(x_sq, mask, r16, A14_) };
    __m512 r12{ _mm512_mask_fmadd_ps(x_sq, mask, r14, A12_) };
    __m512 r10{ _mm512_mask_fmadd_ps(x_sq, mask, r12, A10_) };
    __m512 r8{ _mm512_mask_fmadd_ps(x_sq, mask, r10, A8_) };
    __m512 r6{ _mm512_mask_fmadd_ps(x_sq, mask, r8, A6_) };
    __m512 r4{ _mm512_mask_fmadd_ps(x_sq, mask, r6, A4_) };
    __m512 r2{ _mm512_mask_fmadd_ps(x_sq, mask, r4, A2_) };
    return _mm512_mul_ps(x_sq, r2);
}

FORCE_INLINE __m512 SergeStage::F02(__m512 src, __mmask16 mask, __m512 x) const {
    const __m512 B1_{ _mm512_set1_ps(B1) };
    const __m512 B1_neg_{ _mm512_set1_ps(-B1) };
    const __m512 B2_{ _mm512_set1_ps(B2) };

    const __m512 zero_reg{ _mm512_setzero_ps() };
    __mmask16 F02_mask{ _mm512_mask_cmp_ps_mask(mask, x, zero_reg, _CMP_GT_OQ) };

    // can't use fma because the src is not also an operand

    // F0
    __m512 B2_times_x{ _mm512_mask_mul_ps(src, mask, B2_, x) };
    __m512 B2_times_x_minus_B1{ _mm512_mask_add_ps(src, mask, B2_times_x, B1_neg_) };
    __m512 F0_result{ _mm512_mask_mul_ps(src, mask, B2_times_x_minus_B1, x) };

    // F2
    __m512 B2_times_x_plus_B1{ _mm512_mask_add_ps(src, mask, B2_times_x, B1_) };
    __m512 F2_result{ _mm512_mask_mul_ps(src, mask, B2_times_x_plus_B1, x) };

    return _mm512_mask_blend_ps(F02_mask, F0_result, F2_result);
}

FORCE_INLINE __m512 SergeStage::compute_block(__m512 x, __m512 x_prev) const {
    const __m512 SIGN_BIT_MASK{ _mm512_set1_ps(-0.0f) };

    // absolute values
    __m512 x_abs{ _mm512_andnot_ps(SIGN_BIT_MASK, x) };
    __m512 x_prev_abs{ _mm512_andnot_ps(SIGN_BIT_MASK, x_prev) };

    // precompute values required by the 02 branches
    __m512 sum_with_prev{ _mm512_add_ps(x, x_prev) };
    const __m512 half_reg{ _mm512_set1_ps(0.5f) };
    __m512 avg_with_prev{ _mm512_mul_ps(sum_with_prev, half_reg) };

    // absolute differences
    __m512 diffs{ _mm512_sub_ps(x, x_prev) };
    __m512 diffs_abs{ _mm512_andnot_ps(SIGN_BIT_MASK, diffs) };

    // decide what to evaluate: f1, f02, F1, F02
    const __m512 aa_thresh_reg{ _mm512_set1_ps(anti_aliasing_thresh) };
    __mmask16 aa_mask{ _mm512_cmp_ps_mask(diffs_abs, aa_thresh_reg, _CMP_GT_OQ) }; // 0xFF if x > anti_aliasing_thresh, 0x00 otherwise
    __mmask16 aa_n_mask{ _knot_mask16(aa_mask) };
    const __m512 knot_reg{ _mm512_set1_ps(knot) };
    __mmask16 pw_mask{ _mm512_cmp_ps_mask(x_abs, knot_reg, _CMP_GT_OQ) }; // 0xFF if x > knot, 0x00 otherwise
    __mmask16 pw_n_mask{ _knot_mask16(pw_mask) };
    __mmask16 f1_mask{ _kand_mask16(aa_n_mask, pw_n_mask) };
    __mmask16 f02_mask{ _kand_mask16(aa_n_mask, pw_mask) };
    __mmask16 F1_mask{ _kand_mask16(aa_mask, pw_n_mask) };
    __mmask16 F02_mask{ _kand_mask16(aa_mask, pw_mask) };

    __m512 result{};

    // f1
    result = f1(result, f1_mask, avg_with_prev);

    // f02
    result = f02(result, f02_mask, avg_with_prev);

    // F1. USING MASKED OPERATIONS WILL PROB MAKE THIS FASTER
    __m512 F1_result{ F1(result, F1_mask, x) }; // use abs because F is even
    __m512 F1_prev_result{ F1(result, F1_mask, x_prev) };
    __m512 F1_diffs{ _mm512_mask_sub_ps(result, F1_mask, F1_result, F1_prev_result) };
    result = _mm512_mask_div_ps(result, F1_mask, F1_diffs, diffs);

    // F02
    __m512 F02_result{ F02(result, F02_mask, x) }; // use abs because F is even
    __m512 F02_prev_result{ F02(result, F02_mask, x_prev) };
    __m512 F02_diffs{ _mm512_mask_sub_ps(result, F02_mask, F02_result, F02_prev_result) };
    result = _mm512_mask_div_ps(result, F02_mask, F02_diffs, diffs);

    return result;
}

void SergeStage::generate_buf() {
    // first block of 16
    __m512 x{ _mm512_loadu_ps(audio_in_buf) };
    __m512i permutation_idx{ _mm512_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14) };
    __m512 suffix{ _mm512_permutexvar_ps(permutation_idx, x) };
    __mmask16 load_mask{ _mm512_int2mask(0b0000000000000001) };
    __m512 x_prev{ _mm512_mask_load_ps(suffix, load_mask, &last_sample) };
    _mm512_storeu_ps(out_buf, compute_block(x, x_prev));

    for (size_t i{ 16 }; i < config::buffer_size; i += 16) {
        // load input
        __m512 x{ _mm512_loadu_ps(audio_in_buf + i) };
        __m512 x_prev{ _mm512_loadu_ps(audio_in_buf + i - 1) };

        _mm512_storeu_ps(out_buf + i, compute_block(x, x_prev));
    }

    last_sample = audio_in_buf[config::buffer_size - 1];
}

#elif defined(__AVX2__)
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
    __m256i permutation_idx{ _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6) };
    __m256 suffix{ _mm256_permutevar8x32_ps(x, permutation_idx) };
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