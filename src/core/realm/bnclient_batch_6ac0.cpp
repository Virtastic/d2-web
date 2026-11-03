// src/core/realm/bnclient_batch_6ac0.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x6AC0.c (image base 0x6FF20000).
//
// Row: sub_6AC0 0x6AC0, MSVC 2003 (VC7.1) Release CRT `strcat`: the decompile scans dst for its terminator
// first (byte scan to alignment, then the 0x7EFEFEFF zero-byte test), then copies src there, dword-wise with
// a byte tail. A byte-wise append produces the same bytes and return value.
#include <cstdint>
#include <emscripten.h>

// 1.13c BNClient+0x06AC0 (232 B, callers 0x47FD __NMSG_WRITE, 0x72CA report_failure): strcat(dst, src).
// Append src including its terminator after dst's terminator and return dst.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t* bn_strcat(uint8_t* dst, const uint8_t* src) {
    uint8_t* const ret = dst;
    while (*dst) ++dst;
    do {
        *dst++ = *src;
    } while (*src++ != 0);
    return ret;
}
