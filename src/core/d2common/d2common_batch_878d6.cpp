// D2Common 1.13c: CRT string -> double (strtod core).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// Unwired callees (weak). 87E33 = string -> _LDBL12 (12 bytes) + int status; 878AA = __ld12tod.
__attribute__((weak)) intptr_t D2Common_87E33(uint16_t* ld12, int32_t* status, const uint8_t* str, int32_t a, int32_t b, int32_t c, int32_t d);
__attribute__((weak)) intptr_t D2Common_878AA(const uint16_t* ld12, uint8_t out[8]);

// 1.13c D2Common+0x878D6: parse the string into a 12-byte long double, convert to double at out.
EMSCRIPTEN_KEEPALIVE void D2Common_878D6(uint8_t out[8], const uint8_t* number) {
 uint16_t ld12[6];
 int32_t status;
 D2Common_87E33(ld12, &status, number, 0, 0, 0, 0);
 D2Common_878AA(ld12, out);
}
}
