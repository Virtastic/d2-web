#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0xA050 -- MSVC __aulldvrm. The four retail words are the
// dividend (low, high) and divisor (low, high). EDX:EAX receives the quotient;
// ECX:EBX receives the remainder. Integer division retains retail's zero-
// divisor trap, while fixed-width words keep the x86 register layout explicit.
extern "C" EMSCRIPTEN_KEEPALIVE
uint64_t d2_fn_d2client_a050(uint32_t dividendLow, uint32_t dividendHigh,
                             uint32_t divisorLow, uint32_t divisorHigh,
                             uint64_t* remainder) {
    const uint64_t dividend = (uint64_t(dividendHigh) << 32) | dividendLow;
    const uint64_t divisor = (uint64_t(divisorHigh) << 32) | divisorLow;
    *remainder = dividend % divisor;
    return dividend / divisor;
}
