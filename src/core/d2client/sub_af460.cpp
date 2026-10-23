// src/core/d2client/sub_af460.cpp -- D2Client+0xAF460 transcribed from the cached decompile.
// Ordinals/offsets beyond the decompile are unverified. Callee 0x2A750 is an env seam.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

using D2ClientAf460Dispatch = void (*)(uint32_t kind, int32_t* unit, int32_t* block /*7 dwords*/);

// 1.13c D2Client+0xAF460 (82 B): __fastcall(int* param_1 = ECX, uint8_t* pkt = EDX). Unpack a
// server packet into a 7-dword block and call 0x2A750(pkt[5], param_1, block).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_af460(D2ClientAf460Dispatch d2a750, int32_t* param_1,
                                                     const uint8_t* pkt) {
    uint16_t w6, w8; int16_t s13;
    std::memcpy(&w6, pkt + 6, 2); std::memcpy(&w8, pkt + 8, 2); std::memcpy(&s13, pkt + 0xd, 2);
    int32_t b[7] = { w6, w8, pkt[10], pkt[0xc], s13, pkt[0xf], pkt[0xb] };
    d2a750(pkt[5], param_1, b);
}
