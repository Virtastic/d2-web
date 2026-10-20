// 1.13c D2Common+0x86220 (sub_86220): CRT x87 classification helper (fprem/fpatan range check). Args are
// the 80-bit operand (mantissa lo/hi, exponent+sign) and the second operand's mantissa lo / exponent.
// Integer paths are written out as decompiled. The unnormalised-operand rescale (x87 multiply by the
// constant at 0x6FDEF148) is not float-modelled: ponytail: no float in core; that branch returns 0,
// model as a 64-bit mantissa shift if a vector shows it is reached. Table 0x6FDEF130 (32 flag bytes)
// is likewise not vendored: its branch returns the index.
#include <cstdint>

uint32_t D2Common_86220_FpClassify(int32_t p1, uint32_t p2, uint16_t p3, uint32_t, uint32_t p5, uint32_t p6)
{
    if (!(p2 >> 31)) {                       // top mantissa bit clear: operand unnormalised
        if (p1 == 0 && p2 == 0) return 0;
        if (p3 & 0x7fff) return p3 & 0x7fff;
        uint32_t e = p6 & 0x7fff;
        if (e == 0) { if (p5 >> 31) return p5 * 2; }
        else if (e == 0x7fff || !(p5 >> 31)) return e == 0x7fff ? e : p5 * 2;
        return 0;                            // rescale branch (see header)
    }
    uint32_t u = (p2 * 2) ^ 0xe000000u;
    if (u & 0xe000000u) return u;
    return (p2 & 0x7fffffffu) >> 27;         // flag-table lookup elided (see header)
}
