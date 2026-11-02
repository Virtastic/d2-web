// 1.13c D2CMP+0xA1B0 (stdcall, three doubles): the HSL "hue to channel" helper.
// Retail's x87 runs at the default 64-bit-mantissa precision: every intermediate is an 80-bit extended value and is rounded to a
// double only when the result leaves in ST0 (proved against the captured vectors: 5 tuples differ by one ulp under double arithmetic).
// With an 80-bit long double the arithmetic below is exact as written; on a quad long double (wasm) each operation is rounded to 64
// mantissa bits by r64.
#include <cfloat>
#include "d2cmp_palstate.h"

static inline long double r64(long double x) {
#if LDBL_MANT_DIG > 64
    const long double c = x * (long double)(1ull << (LDBL_MANT_DIG - 64)) + x;   // Veltkamp split, constant 2^(M-64)+1
    return c - (c - x);
#else
    return x;
#endif
}

// 1.13c D2CMP+0x0A1B0
// hue > 360 loses 360, hue < 0 gains 360 (once each). Below 60 the value rises linearly from m1 to m2, below 180
// it holds m2, below 240 it falls linearly back, else m1. The slope multiplies by the double 1/60 (0x6FE300C8).
extern "C" double d2cmp_a1b0(double m1, double m2, double hue) {
    const long double kInv60 = (long double)(1.0 / 60.0);
    long double h = hue;
    if (h > 360.0L) h = r64(h - 360.0L);
    if (h < 0.0L) h = r64(h + 360.0L);
    if (h < 60.0L) return (double)r64(r64(r64(h * r64((long double)m2 - m1)) * kInv60) + m1);
    if (h < 180.0L) return m2;
    if (h < 240.0L) return (double)r64(r64(r64(r64((long double)m2 - m1) * r64(240.0L - h)) * kInv60) + m1);
    return m1;
}
