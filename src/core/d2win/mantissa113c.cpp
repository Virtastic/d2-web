// Prior art used: D2MOO's D2Win module has no MSVC CRT mantissa helpers.
// Derived from cached 1.13c D2Win bodies. These helpers operate on three 32-bit
// limbs, with opposite limb ordering for add_12 (least significant first) and
// the conversion mantissa helpers (most significant first). No host floats.
#include "mantissa113c.h"

namespace {
uint32_t addCarry(uint32_t left, uint32_t right, uint32_t* output) {
    const uint32_t sum = left + right;
    *output = sum;
    return sum < left || sum < right;
}
}

// 1.13c D2Win+0x190FD
extern "C" int32_t d2win_190fd(const uint32_t* mantissa) {
    for (int i = 0; i != 3; ++i) if (mantissa[i] != 0) return 0;
    return 1;
}

// 1.13c D2Win+0x190E2
extern "C" void d2win_190e2(uint32_t* destination, const uint32_t* source) {
    // Preserve forward-copy aliasing, rather than snapshotting or using memcpy.
    for (int i = 0; i != 3; ++i) destination[i] = source[i];
}

// 1.13c D2Win+0x196D1
extern "C" void d2win_196d1(uint32_t* destination, const uint32_t* source) {
    if (addCarry(destination[0], source[0], destination)) {
        if (addCarry(destination[1], 1, destination + 1)) ++destination[2];
    }
    if (addCarry(destination[1], source[1], destination + 1)) ++destination[2];
    addCarry(destination[2], source[2], destination + 2);
}

// 1.13c D2Win+0x19023
extern "C" void d2win_19023(uint32_t* mantissa, int32_t bit) {
    const int32_t limb = bit / 32;
    uint32_t carry = addCarry(mantissa[limb], uint32_t{1} << ((31u - uint32_t(bit % 32)) & 31), mantissa + limb);
    for (int32_t i = limb - 1; i >= 0 && carry; --i) carry = addCarry(mantissa[i], 1, mantissa + i);
}

// 1.13c D2Win+0x19116
extern "C" void d2win_19116(uint32_t* mantissa, int32_t shift) {
    const uint32_t bits = uint32_t(shift % 32) & 31;
    uint32_t carry = 0;
    for (int i = 0; i != 3; ++i) {
        const uint32_t word = mantissa[i];
        mantissa[i] = (word >> bits) | carry;
        carry = (word & ~(UINT32_MAX << bits)) << ((32 - bits) & 31);
    }
    const int32_t words = shift / 32;
    for (int32_t i = 2; i >= 0; --i) mantissa[i] = i < words ? 0 : mantissa[i - words];
}

// 1.13c D2Win+0x18FF1
extern "C" int32_t d2win_18ff1(const uint32_t* mantissa, int32_t bit) {
    int32_t limb = bit / 32;
    uint32_t tail = mantissa[limb] & ~(UINT32_MAX << ((31u - uint32_t(bit % 32)) & 31));
    for (;;) {
        if (tail != 0) return 0;
        if (++limb > 2) return 1;
        tail = mantissa[limb];
    }
}

// 1.13c D2Win+0x19070
extern "C" int32_t d2win_19070(uint32_t* mantissa, int32_t bit) {
    int32_t carry = 0;
    const int32_t limb = bit / 32;
    const uint32_t sh = (31u - uint32_t(bit % 32)) & 31;
    if ((mantissa[limb] & (uint32_t{1} << sh)) && d2win_18ff1(mantissa, bit) == 0) {
        // __IncMan(bit-1) inlined: its EAX at exit (the last ___addl carry) is the return value.
        const int32_t b = bit - 1, l = b / 32;
        carry = int32_t(addCarry(mantissa[l], uint32_t{1} << ((31u - uint32_t(b % 32)) & 31), mantissa + l));
        for (int32_t i = l - 1; i >= 0 && carry; --i) carry = int32_t(addCarry(mantissa[i], 1, mantissa + i));
    }
    mantissa[limb] &= UINT32_MAX << sh;
    for (int32_t i = limb + 1; i < 3; ++i) mantissa[i] = 0;
    return carry;
}
