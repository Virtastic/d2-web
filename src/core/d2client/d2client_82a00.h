// 1.13c D2Client+0x82A00: item packet -- an item the player holds goes back into the inventory (slot word and grid bytes in packet 2).
// Retail words (unit pointers, the inventory handle) are 32-bit addresses; callees are named by their thunk RVA.
#pragma once
#include <cstdint>
struct D2c82a00G {
    uint32_t* player;   // +0x11BBFC  player unit (retail word)
    uint32_t* hash;     // +0x10AE08  item unit hash, 128 buckets, chain at unit+0xE4, id at +0xC, type at +0
};
struct D2c82a00Env {
    uint32_t (*c6d0)(uint32_t inv, uint32_t unit);                                // returns the unit or 0
    void     (*ca0c)(uint32_t inv);
    uint32_t (*c1a8)(uint32_t unit, uint32_t flag, uint32_t line, uint32_t file);
    void     (*c544)(uint32_t pl, uint32_t unit);
    void     (*caa2)(uint32_t inv, uint32_t slot);
    void     (*f82980)(uint32_t eax, uint32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    void     (*c7d8)(uint32_t inv, uint32_t unit);
    uint32_t (*c202)(uint32_t inv);
    void     (*f162d0)(uint32_t eax);
    void     (*f81aa0)(uint32_t esi);
    void     (*fatal)(uint32_t code);                                             // 10265/10024 + _exit(-1)
};
extern "C" void d2_client_82a00(const D2c82a00G* g, const D2c82a00Env* e, uint32_t eax, uint32_t p1);
