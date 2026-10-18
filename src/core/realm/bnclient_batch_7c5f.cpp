// src/core/realm/bnclient_batch_7c5f.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x7C5F.c (image base 0x6FF20000).
//
// Row: _atol 0x7C5F, MSVC 2003 (VC7.1) Release CRT. The locale whitespace test (0x3852 ___isctype_mt,
// mask 8) is the only seam; the digit loop is the decompile.
#include <cstdint>
#include <emscripten.h>

struct BnAtolEnv {
    int32_t (*is_space)(uint32_t c);   // 0x3852 ___isctype_mt(c, 8) through the thread's locale
};

static int32_t bn_ascii_space_7c5f(uint32_t c) {
    return (c == 0x20 || (c >= 0x09 && c <= 0x0d)) ? 8 : 0;
}

static const BnAtolEnv g_bn_atol_default = {bn_ascii_space_7c5f};

// 1.13c BNClient+0x07C5F (136 B, caller 0x6F60 _atoi): _atol(str). Skip leading whitespace, take one
// optional '-' or '+', then accumulate decimal digits until the first non-digit; a leading '-' negates.
// No overflow check (the multiply wraps like the retail int).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_atol(const char* str, const BnAtolEnv* env) {
    if (env == nullptr) env = &g_bn_atol_default;
    while (env->is_space((uint8_t)*str) != 0) ++str;

    const uint8_t* p = (const uint8_t*)str + 1;
    const uint8_t first = (uint8_t)*str;
    uint32_t c = first;
    if (first == '-' || first == '+') {
        c = *p;
        ++p;
    }

    int32_t accum = 0;
    for (;;) {
        const int32_t digit = (c >= '0' && c <= '9') ? (int32_t)(c - '0') : -1;
        if (digit == -1) break;
        accum = (int32_t)((uint32_t)digit + (uint32_t)accum * 10u);
        c = *p++;
    }
    return first == '-' ? -accum : accum;
}
