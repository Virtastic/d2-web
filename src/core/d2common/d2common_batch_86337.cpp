// 1.13c D2Common+0x86337 (sub_86337): CRT x87 binary-op special-case dispatcher. Retail enters with
// the operation code in EAX and the FPU stack in ST0..ST7; the low 6 bits of EAX pick a jump-table
// case. Bits 3-5 (g) name the second stack register ST[g]; bits 0-2 (k) pick the operand order:
// k=0,6,7 -> (ST[g], ST0); k=1..5 -> (ST0, ST[g]) (k=1,3 first execute an invalid-opcode trap that
// is not modelled). Each g>=1 case tail-calls sub_86220 with both 80-bit operands; g=0 (cases 0-7)
// only returns a NaN/ST1 result via x87 divide/move, which is not float-modelled (returns 0).
// Our ABI: st points to 8 x 80-bit registers as {mantissa lo, mantissa hi, sign/exponent u16 in u32}.
#include <cstdint>

uint32_t D2Common_86220_FpClassify(int32_t p1, uint32_t p2, uint16_t p3, uint32_t, uint32_t p5, uint32_t p6);

uint32_t D2Common_86337_FpDispatch(uint32_t eax, const uint32_t* st)
{
    uint32_t n = eax & 0x3f, g = n >> 3, k = n & 7;
    if (g == 0) return 0;
    const uint32_t* a = (k == 0 || k >= 6) ? st + 3 * g : st;
    const uint32_t* b = (k == 0 || k >= 6) ? st : st + 3 * g;
    return D2Common_86220_FpClassify((int32_t)a[0], a[1], (uint16_t)a[2], 0, b[1], b[2] & 0xffff) ;
}
