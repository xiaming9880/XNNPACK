// Copyright 2020 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <psimd.h>

#include <xnnpack/math-stubs.h>


void xnn_math_f32_roundz__psimd_addsub(
    size_t n,
    const float* input,
    float* output)
{
  assert(n % (4 * sizeof(float)) == 0);

  // Mask for the sign bit of a floating-point number.
  const psimd_s32 vsign_mask = psimd_splat_s32(INT32_C(0x80000000));
  // Addition of this number to a floating-point number x cause rounding of the result to an integer. Then this magic
  // number is subtracted back from the result to get original x rounded to integer. This trick works only for
  // 0 <= x < 2**24, but all numbers in 2**23 <= x < 2**24 range are integers, so we can further restrict it to
  // 0 <= x < 2**23. Then the upper bound of the validity interval is conveniently the same as the magic number.
  const psimd_f32 vmagic_number = psimd_splat_f32(0x1.000000p+23f);
  // Unit constant to decrement absolute values rounded "wrong way" (i.e. away from zero) in the round-to-nearest-even
  // operation.
  const psimd_f32 vone = psimd_splat_f32(1.0f);

  for (; n != 0; n -= 4 * sizeof(float)) {
    const psimd_f32 vx = psimd_load_f32(input);
    input += 4;

    // The rounding trick works only for x >= 0, so we compute absolute value of x, round it, and restore the sign in
    // the end. This method works for round-toward-zero because it is an odd function.
    const psimd_f32 vabsx = psimd_andnotmask_f32(vsign_mask, vx);

    // Compute bitmask for the bits we want to copy from x. Other bits will be copied from the rounded abs(x).
    // If abs(x) < 2**23 or x is NaN, we want the sign bit from x and the rest from the rounded abs(x).
    // Otherwise (abs(x) >= 2**23), we want all bits from x.
    const psimd_s32 vrndmask = vsign_mask | (vabsx >= vmagic_number);
    // Addition-subtraction trick with the magic number to cause rounding to integer for abs(x).
    // Note: the result is valid only for 0 <= abs(x) < 2**23.
    // Note: addition-subtraction implicitly converts SNaN inputs to QNaNs.
    const psimd_f32 vrndabsx = psimd_sub_f32(psimd_add_f32(vabsx, vmagic_number), vmagic_number);

    // Compute adjustment to be subtracted from the rounded-to-nearest-even abs(x) value.
    // Adjustment is one if the rounded value is greater than the abs(x) value and zero otherwise (including NaN input).
    const psimd_f32 vadjustment = psimd_andmask_f32(vrndabsx > vabsx, vone);
    // Adjust abs(x) rounded to nearest-even via the addition-subtraction trick to get abs(x) rounded down.
    // Note: subtraction implicitly converts SNaN inputs to QNaNs.
    const psimd_f32 vflrabsx = psimd_sub_f32(vrndabsx, vadjustment);

    // Combine abs(x) rounded down via addition-subtraction trick with adjustment and the input x value.
    // For abs(x) < 2**23, the result is abs(x) rounded via addition-subtraction trick with the sign of x.
    // For NaN inputs, the result is x converted to QNaN as a side-effect of addition-subtraction and adjustment.
    // For abs(x) >= 2**23, the result is x itself.
    const psimd_f32 vy = psimd_blend_f32(vrndmask, vx, vflrabsx);

    psimd_store_f32(output, vy);
    output += 4;
  }
}
