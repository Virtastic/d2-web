#include <cstdint>

uint64_t d2common_aulldvrm(uint64_t dividend, uint64_t divisor, uint64_t* remainder);

// 1.13c D2CMP+0x6280 -- CRT __aulldvrm (stdcall, 4 words, ret 0x10): unsigned 64-bit divide.
// Instruction-identical to D2Common+0x7470 once absolute addresses are normalised, so this delegates
// to that port. Quotient returned (EDX:EAX on retail), remainder through the pointer. Zero divisor faults.
extern "C" uint64_t d2cmp_6280(uint64_t dividend, uint64_t divisor, uint64_t* remainder) {
    return d2common_aulldvrm(dividend, divisor, remainder);
}
