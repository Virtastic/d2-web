// 1.13c D2Client+0x82B20: item packet -- an item in the player's world moves to a new position (mode byte at packet 2 +0x10).
// Retail words (unit pointers, the inventory handle) are 32-bit addresses; callees are named by their thunk RVA.
#pragma once
#include <cstdint>
struct D2c82b20G {
    uint32_t* player;   // +0x11BBFC  player unit (retail word)
    uint32_t* hash;     // +0x10AE08  item unit hash, 128 buckets, chain at unit+0xE4, id at +0xC, type at +0
};
struct D2c82b20Env {
    void     (*f82820)(uint32_t eax, uint32_t esi, uint32_t edi, uint32_t s0, uint32_t s1, uint32_t s2);
    void     (*ca24)(uint32_t unit, uint32_t w);       // w: packet 2's slot word with its low byte replaced by the mode byte
    void     (*ca06)(uint32_t unit, uint32_t x);
    void     (*ca48)(uint32_t unit, uint32_t y);
    uint32_t (*c1cc)(uint32_t unit, uint32_t flag);
    void     (*f81ff0)(uint32_t esi, uint32_t edi, uint32_t s0);
    uint32_t (*ca6c)(uint32_t inv, uint32_t unit, uint32_t page);
    uint32_t (*f1630)(uint32_t ecx);
    uint32_t (*ca84)(uint32_t inv, uint32_t unit, uint32_t v);
    void     (*fbe2b0)();
    void     (*f81aa0)(uint32_t esi);
    void     (*fatal)(uint32_t code);                   // 10265/10024 + _exit(-1)
};
extern "C" void d2_client_82b20(const D2c82b20G* g, const D2c82b20Env* e, uint32_t pkt, uint32_t p2);
