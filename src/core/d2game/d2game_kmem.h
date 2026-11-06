// Native-width view of a 32-bit D2Game image for the d2game_<rva>.cpp ports of the struct-argument batch: every pointer
// is a 32-bit retail address and every read or write goes through these callbacks (the engine is wasm64; the 32-bit
// blobs the capture records are addressed below 4 GiB, so the probes dereference them directly).
#pragma once
#include <cstdint>

struct D2gMem {
    uint8_t  (*r8)(uint32_t a);
    uint16_t (*r16)(uint32_t a);
    uint32_t (*r32)(uint32_t a);
    void     (*w8)(uint32_t a, uint8_t v);
    void     (*w16)(uint32_t a, uint16_t v);
    void     (*w32)(uint32_t a, uint32_t v);
};

// quest-record lookup: game+0x10F4 holds a list head; its first word is the first node; walk +0xF4 for node type 0x17 and
// return node+0x18 (0 = none)
static inline uint32_t d2gQuestRec(const D2gMem& m, uint32_t game) {
    uint32_t l = m.r32(game + 0x10F4);
    if (!l) return 0;
    uint32_t p = m.r32(l);
    while (p) {
        if (m.r32(p) == 0x17) return m.r32(p + 0x18);
        p = m.r32(p + 0xF4);
    }
    return 0;
}
