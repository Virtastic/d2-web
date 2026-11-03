// Seam for D2Game+0x70010 (ESI = dst, EDI = src, stack = p1, ret 4). All callees are stdcall D2Common/Fog thunks.
#pragma once
#include <cstdint>
struct D2Game70010Env {
    uint32_t (*kind)(uint32_t dst);                                   // 0xAA66
    uint32_t (*getA)(uint32_t src);                                   // 0xABB0
    void (*setA)(uint32_t dst, uint32_t v);                           // 0xABCE
    uint32_t (*getB)(uint32_t src);                                   // 0xB0C6
    void (*setB)(uint32_t dst, uint32_t v);                           // 0xB0BA
    void (*setC)(uint32_t dst, uint32_t mask, uint32_t p1);           // 0xA65E
    uint32_t (*check)(uint32_t dst, uint32_t mask, uint32_t line);    // 0xA8A4(dst, 0x1000000, 0x9AA, "PlrTrade.cpp")
};
void d2game_port_70010(uint32_t dst, uint32_t src, uint32_t p1, const D2Game70010Env* env);
