// src/core/realm/d2mcpclient_batch_40b0.cpp -- one UNMAPPED 1.13c D2MCPClient.dll alpha-lane row
// (implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-d2mcpclient/decomp/0x40B0.c (image base 0x6FA20000).
//
// Row: sub_40B0 0x40B0, MSVC 2003 (VC7.1) Release CRT `strcpy`. The retail body is the usual aligned
// dword copy with the 0x7EFEFEFF zero-byte test; a byte copy produces the same bytes and return value.
// Byte-identical to the 232-byte BNClient twin src/core/realm/bnclient_batch_6ac0.cpp (0x6AC0).
#include <cstdint>
#include <emscripten.h>

// 1.13c D2MCPClient+0x040B0 (232 B, callers 0x025EC __NMSG_WRITE, 0x05603 report_gsfailure):
// strcpy(dst, src). Copy src including its terminator into dst and return dst.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t* d2_mcp_strcpy_40b0(uint8_t* dst, const uint8_t* src) {
    uint8_t* const ret = dst;
    do {
        *dst++ = *src;
    } while (*src++ != 0);
    return ret;
}
