// D2Common 1.13c: CRT __ld12tod / __ld12tof thunks over __ld12cvt.
#include <cstdint>
#include <emscripten.h>

extern "C" {
uint32_t D2Common_87752_ld12cvt(const uint16_t* p1, uint32_t* p2, const int32_t* p3);

// Format tables (6 ints: max_exp, min_exp, mantissa bits, exp bits, total bits, bias).
static const int32_t kDoubleFmt[6] = {1024, -1023, 53, 11, 64, 1023};
static const int32_t kFloatFmt[6] = {128, -127, 24, 8, 32, 127};

// 1.13c D2Common+0x878AA: _LDBL12 -> double.
EMSCRIPTEN_KEEPALIVE intptr_t D2Common_878AA(const uint16_t* ld12, uint8_t out[8]) {
 return (intptr_t)D2Common_87752_ld12cvt(ld12, (uint32_t*)out, kDoubleFmt);
}

// 1.13c D2Common+0x878C0: _LDBL12 -> float.
EMSCRIPTEN_KEEPALIVE intptr_t D2Common_878C0(const uint16_t* ld12, uint8_t out[8]) {
 return (intptr_t)D2Common_87752_ld12cvt(ld12, (uint32_t*)out, kFloatFmt);
}
}
