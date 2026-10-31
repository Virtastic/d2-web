// 1.13c D2CMP+0x6280 (__aulldvrm, VS2003 CRT): instruction-identical to D2Common+0x7470; delegates.
// Quotient returned, remainder through the pointer; a zero divisor traps as in retail.
#include <cstdint>
uint64_t d2common_aulldvrm(uint64_t dividend, uint64_t divisor, uint64_t* remainder);
// 1.13c D2CMP+0x6280
extern "C" uint64_t d2cmp_6280(uint64_t dividend, uint64_t divisor, uint64_t* remainder) {
    return d2common_aulldvrm(dividend, divisor, remainder);
}
