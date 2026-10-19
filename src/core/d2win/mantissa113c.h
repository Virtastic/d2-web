#pragma once
#include <cstdint>
// Three explicit 32-bit limbs; pointers stay native-width. Bit offsets are 0..95.
extern "C" {
int32_t d2win_190fd(const uint32_t* mantissa);
void d2win_190e2(uint32_t* destination, const uint32_t* source);
void d2win_196d1(uint32_t* destination, const uint32_t* source);
void d2win_19023(uint32_t* mantissa, int32_t bit);
void d2win_19116(uint32_t* mantissa, int32_t shift);
int32_t d2win_18ff1(const uint32_t* mantissa, int32_t bit);
}
