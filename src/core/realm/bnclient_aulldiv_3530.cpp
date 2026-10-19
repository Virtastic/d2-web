#include <cstdint>
#include <emscripten.h>

extern "C" uint64_t d2_fn_bnclient_61c0(uint32_t dividend_low, uint32_t dividend_high,
                                         uint32_t divisor_low, uint32_t divisor_high);

// 1.13c BNClient+0x03530 (__aulldiv): unsigned 64-bit quotient, split into low/high input limbs.
// BNClient's sibling __aulldvrm at +0x061C0 implements the same quotient calculation.
extern "C" EMSCRIPTEN_KEEPALIVE uint64_t bnclient_aulldiv_3530(uint32_t dividend_low,
                                                              uint32_t dividend_high,
                                                              uint32_t divisor_low,
                                                              uint32_t divisor_high) {
    return d2_fn_bnclient_61c0(dividend_low, dividend_high, divisor_low, divisor_high);
}
