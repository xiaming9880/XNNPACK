// Auto-generated file. Do not edit!
//   Template: src/f32-sigmoid/sse-p5-div.c.in
//   Generator: tools/xngen
//
// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <smmintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/intrinsics-polyfill.h>
#include <xnnpack/vunary.h>


void xnn_f32_sigmoid_ukernel__sse41_p5_div_x4(
    size_t n,
    const float* x,
    float* y,
    const void* params)
{
  assert(n % sizeof(float) == 0);

  const __m128 vmagic_bias = _mm_set1_ps(0x1.8000FEp23f);
  // The smallest x for which sigmoidf(x) is normalized.
  // This number is also the smallest x for which expf(x) is normalized.
  const __m128 vdenorm_cutoff = _mm_set1_ps(-0x1.5D589Ep+6f);
  const __m128 vlog2e = _mm_set1_ps(0x1.715476p+0f);
  // Last 7 bits are zeroes
  const __m128 vminus_ln2_hi = _mm_set1_ps(-0x1.62E400p-1f);
  const __m128 vminus_ln2_lo = _mm_set1_ps(-0x1.7F7D1Cp-20f);
  const __m128 vone = _mm_set1_ps(1.0f);
  const __m128 vsign_mask = _mm_set1_ps(-0.0f);

  const __m128 vc1 = _mm_set1_ps(0x1.FFFFF6p-1f);
  const __m128 vc2 = _mm_set1_ps(0x1.FFFDC6p-2f);
  const __m128 vc3 = _mm_set1_ps(0x1.555A80p-3f);
  const __m128 vc4 = _mm_set1_ps(0x1.573A1Ap-5f);
  const __m128 vc5 = _mm_set1_ps(0x1.0F9F9Cp-7f);

  for (; n >= 4 * sizeof(float); n -= 4 * sizeof(float)) {
    const __m128 vx = _mm_loadu_ps(x);

    // General structure of the algorithm:
    //           / exp(x) / (1 + exp(x)) if x <= 0
    //   f[x] := 
    //           \ 1 - f[-x] if x >= 0
    //
    // First we compute f[z] := exp(z) / (1 + exp(z)) where z = -abs(x),
    // then replace result with 1 - f[z] if x >= 0.
    const __m128 vz = _mm_or_ps(vx, vsign_mask);

    // Compute reduced argument n := round(z / log(2)).
    // We do it by adding a large number (magic bias) to the product z * (1/log(2)), which cause rounding of the result
    // to an integer, then subtracing the large number back. The trick with adding large number is valid only within
    // certain bounds (|x| <= 2**22), but thats ok, because inputs x outside of [-87.336544, 17.328678] (i.e. z outsize
    // [0, 87.336544]) underflow or saturate sigmoidf(x) anyway. We fixup the result for such inputs at the very end of
    // the algorithm.
    __m128 vn = _mm_add_ps(_mm_mul_ps(vz, vlog2e), vmagic_bias);

    // Create a floating-point number s (scale) such that s == 2**n for inputs which don't cause underflow, i.e.
    // -87.33642 <= z <= 0.0, and -126 <= n <= 0 accordingly.
    const __m128 vs = _mm_castsi128_ps(_mm_slli_epi32(_mm_castps_si128(vn), 23));

    // Subtract the large number back to get final n := round(z / log(2)).
    vn = _mm_sub_ps(vn, vmagic_bias);

    // Compute reduced argument t := z - n * log(2).
    // Use Cody-Waite range reduction method (note two constants to represent log(2)) to improve accuracy.
    __m128 vt = _mm_add_ps(_mm_mul_ps(vn, vminus_ln2_hi), vz);
    vt = _mm_add_ps(_mm_mul_ps(vn, vminus_ln2_lo), vt);

    // Compute degree-5 polynomial approxiatmion for exp(t) on [-log(2)/2, log(2)/2].
    __m128 vp = _mm_add_ps(_mm_mul_ps(vc5, vt), vc4);
    vp = _mm_add_ps(_mm_mul_ps(vp, vt), vc3);
    vp = _mm_add_ps(_mm_mul_ps(vp, vt), vc2);
    vp = _mm_add_ps(_mm_mul_ps(vp, vt), vc1);

    // Reconstruct the exp(z) value:
    //   e = s * (1 + t * (c1 + t * (c2 + t * (c3 + t * (c4 + t * c5)))))
    //     = s + (t * s) * (c1 + t * (c2 + t * (c3 + t * (c4 + t * c5))))
    //     = s + (t * s) * p
    vt = _mm_mul_ps(vt, vs);
    __m128 ve = _mm_add_ps(_mm_mul_ps(vt, vp), vs);

    // Denominator of the sigmoid fraction: 1.0 + exp(z)
    __m128 vd = _mm_add_ps(ve, vone);

    // Reconstruct sigmoid(-z) = exp(z) / (1.0 + exp(z))
    __m128 vf = _mm_div_ps(ve, vd);

    // For inputs below denormal cutoff, replace output with +0.0f.
    // Note that for NaN inputs, comparison result is false, and outputs are left unchanged.
    vf = _mm_andnot_ps(_mm_cmplt_ps(vz, vdenorm_cutoff), vf);

    // Reconstruct sigmoid(x) = x < 0 ? sigmoid(z) : 1.0 - sigmoid(z)
    vf = _mm_blendv_ps(_mm_sub_ps(vone, vf), vf, vx);

    _mm_storeu_ps(y, vf);

    x += 4;
    y += 4;
  }
  if XNN_UNLIKELY(n != 0) {
    const __m128 vx = _mm_loadu_ps_notsan(x);

    // General structure of the algorithm:
    //           / exp(x) / (1 + exp(x)) if x <= 0
    //   f[x] := 
    //           \ 1 - f[-x] if x >= 0
    //
    // First we compute f[z] := exp(z) / (1 + exp(z)) where z = -abs(x),
    // then replace result with 1 - f[z] if x >= 0.
    const __m128 vz = _mm_or_ps(vx, vsign_mask);

    // Compute reduced argument n := round(z / log(2)).
    // We do it by adding a large number (magic bias) to the product z * (1/log(2)), which cause rounding of the result
    // to an integer, then subtracing the large number back. The trick with adding large number is valid only within
    // certain bounds (|x| <= 2**22), but thats ok, because inputs x outside of [-87.336544, 17.328678] (i.e. z outsize
    // [0, 87.336544]) underflow or saturate sigmoidf(x) anyway. We fixup the result for such inputs at the very end of
    // the algorithm.
    __m128 vn = _mm_add_ps(_mm_mul_ps(vz, vlog2e), vmagic_bias);

    // Create a floating-point number s (scale) such that s == 2**n for inputs which don't cause underflow, i.e.
    // -87.33642 <= z <= 0.0, and -126 <= n <= 0 accordingly.
    const __m128 vs = _mm_castsi128_ps(_mm_slli_epi32(_mm_castps_si128(vn), 23));

    // Subtract the large number back to get final n := round(z / log(2)).
    vn = _mm_sub_ps(vn, vmagic_bias);

    // Compute reduced argument t := z - n * log(2).
    // Use Cody-Waite range reduction method (note two constants to represent log(2)) to improve accuracy.
    __m128 vt = _mm_add_ps(_mm_mul_ps(vn, vminus_ln2_hi), vz);
    vt = _mm_add_ps(_mm_mul_ps(vn, vminus_ln2_lo), vt);

    // Compute degree-5 polynomial approxiatmion for exp(t) on [-log(2)/2, log(2)/2].
    __m128 vp = _mm_add_ps(_mm_mul_ps(vc5, vt), vc4);
    vp = _mm_add_ps(_mm_mul_ps(vp, vt), vc3);
    vp = _mm_add_ps(_mm_mul_ps(vp, vt), vc2);
    vp = _mm_add_ps(_mm_mul_ps(vp, vt), vc1);

    // Reconstruct the exp(z) value:
    //   e = s * (1 + t * (c1 + t * (c2 + t * (c3 + t * (c4 + t * c5)))))
    //     = s + (t * s) * (c1 + t * (c2 + t * (c3 + t * (c4 + t * c5))))
    //     = s + (t * s) * p
    vt = _mm_mul_ps(vt, vs);
    __m128 ve = _mm_add_ps(_mm_mul_ps(vt, vp), vs);

    // Denominator of the sigmoid fraction: 1.0 + exp(z)
    __m128 vd = _mm_add_ps(ve, vone);

    // Reconstruct sigmoid(-z) = exp(z) / (1.0 + exp(z))
    __m128 vf = _mm_div_ps(ve, vd);

    // For inputs below denormal cutoff, replace output with +0.0f.
    // Note that for NaN inputs, comparison result is false, and outputs are left unchanged.
    vf = _mm_andnot_ps(_mm_cmplt_ps(vz, vdenorm_cutoff), vf);

    // Reconstruct sigmoid(x) = x < 0 ? sigmoid(z) : 1.0 - sigmoid(z)
    vf = _mm_blendv_ps(_mm_sub_ps(vone, vf), vf, vx);

    if (n & (2 * sizeof(float))) {
      _mm_storel_pi((__m64*) y, vf);
      vf = _mm_movehl_ps(vf, vf);
      y += 2;
    }
    if (n & (1 * sizeof(float))) {
      _mm_store_ss(y, vf);
    }
  }
}
