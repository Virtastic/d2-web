// 1.13c D2CMP+0x17C10 (sub_17C10, 117 bytes), read from the retail disassembly. Input: the x87 ST0 value
// (a double, the retail control word pinned to 53-bit precision); output: EDX:EAX as a 64-bit integer.
//
// The routine is a double-to-int64 conversion with a sign-dependent correction. It stores round-to-nearest
// int64 r (FISTP, the indefinite value 0x8000000000000000 out of range), and float32 bits of x. If r is
// nonzero (low word, or high word with the sign bit masked off) it takes d = x - r, and the float32 bits db
// of d: for x with the sign bit clear, r -= (db > 0x80000000); for x with the sign bit set, r += ((db ^ 0x80000000)
// > 0x80000000). Both are 64-bit add/sub-with-carry chains. Net effect: truncation toward zero, with the
// indefinite value for out-of-range inputs. The port keeps the instruction-level steps.
#include <cstdint>
#include <cmath>
#include <cstring>

// 1.13c D2CMP+0x17C10
extern "C"
uint64_t d2cmp_17c10(double x) {
    int64_t r;
    if (x >= -9223372036854775808.0 && x < 9223372036854775808.0) r = (int64_t)std::nearbyint(x);
    else r = INT64_MIN;                                          // FISTP indefinite
    uint32_t lo = (uint32_t)(uint64_t)r, hi = (uint32_t)((uint64_t)r >> 32);
    if (lo != 0 || (hi & 0x7fffffffu) != 0) {
        float fx = (float)x;
        uint32_t xbits;
        memcpy(&xbits, &fx, 4);
        float fd = (float)(x - (double)r);
        uint32_t db;
        memcpy(&db, &fd, 4);
        if ((xbits & 0x80000000u) == 0) {
            if (db > 0x80000000u) r -= 1;                        // add ecx,0x7fffffff; sbb eax/edx,0
        } else {
            uint32_t a = db ^ 0x80000000u;                       // xor ecx,0x80000000; add ecx,0x7fffffff; adc eax/edx,0
            if (a > 0x80000000u) r += 1;
        }
    }
    return (uint64_t)r;
}
