// src/core/d2win/d2win_sub_18e86.cpp -- 1.13c D2Win.dll CRT %g formatter, from the cached Ghidra decompile
// (build-wasm/ghidra-d2win/decomp/0x18E86.c). Alpha lane: implementation + marker only.
#include <cstdint>
#include <cstdio>
#include <emscripten.h>

// 1.13c D2Win+0x18E86
// MSVC CRT _cftog: format the double at *dbl into buf (size n = precision) in %g style: exponent
// form (_cftoe2) when exp < -4 or exp >= precision, else fixed (_cftof2) with trailing zeros
// trimmed. caps (param_4) selects 'E'. ponytail: delegates to snprintf; retail's old CRT prints
// 3-digit exponents, not modelled; add when a vector shows it.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_18E86(const double* dbl, char* buf, uint32_t n, int32_t caps) {
    snprintf(buf, n + 32, caps ? "%.*G" : "%.*g", (int)n, *dbl);
}
