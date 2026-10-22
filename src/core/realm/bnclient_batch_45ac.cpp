// src/core/realm/bnclient_batch_45ac.cpp -- 1.13c BNClient.dll ___crtGetEnvironmentStringsA, from
// build-wasm/ghidra-bnclient/decomp/0x45AC.c (alpha lane). The OS environment block is an argument
// (no Win32 here); the DAT_6ff3e5f8 W/A probe state is the caller's choice of `wide`.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// 1.13c BNClient+0x045AC (290 B, caller 0x01FDD): ___crtGetEnvironmentStringsA. Returns a malloc'd
// double-NUL-terminated ANSI copy of the block, or null. wide=0: the A path measures to the terminating
// empty string and memcpys (size incl. final NUL). wide=1: the W path measures the UTF-16 block
// (count incl. final NUL), converts with WideCharToMultiByte(CP_ACP); here code units < 0x100 map to
// one byte (ponytail: the OS codepage conversion is not modelled, ASCII/Latin-1 only).
extern "C"
EMSCRIPTEN_KEEPALIVE char* bn_crt_get_environment_strings_a(const void* block, int wide) {
    if (!block) return nullptr;
    if (!wide) {
        const char* p = (const char*)block;
        while (*p) { while (*++p) {} ++p; }
        size_t n = (size_t)(p - (const char*)block) + 1;
        char* d = (char*)std::malloc(n);
        if (d) std::memcpy(d, block, n);
        return d;
    }
    const uint16_t* w = (const uint16_t*)block;
    const uint16_t* p = w;
    while (*p) { while (*++p) {} ++p; }
    size_t n = (size_t)(p - w) + 1;
    char* d = (char*)std::malloc(n);
    if (!d) return nullptr;
    for (size_t i = 0; i < n; ++i) d[i] = (char)w[i];
    return d;
}
