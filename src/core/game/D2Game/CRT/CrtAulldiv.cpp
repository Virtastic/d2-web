#include <cstdint>

// 1.13c D2Game+0x3E70 (__aulldiv): unsigned 64-bit / 64-bit division. Operands are the dividend
// EDX:EAX (hi=param_2, lo=param_1) and the divisor EBX:ECX (hi=param_4, lo=param_3); the result is
// EDX:EAX. The decompile's two limbs are the classic restored-divide; both paths give the same 64-bit
// quotient (the divisor-high==0 path splits it into a 64/32 quotient plus the overflow word), so the
// single 64-bit divide below is the rule. A zero divisor raises #DE in retail and here alike.
extern "C" uint64_t d2game_aulldiv(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi)
{
    return ((uint64_t)hi << 32 | lo) / ((uint64_t)dhi << 32 | dlo);
}
