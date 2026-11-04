// src/core/fog/fog_1b0c0.cpp -- 1.13c Fog.dll sub_1B0C0 (pool free), from build-wasm/ghidra-fog/decomp/0x1B0C0.c.
// Register form: EAX = pool (NULL = the default pool at 6FF9AF94), ECX = pp (the address of a slot holding the block pointer), two stack words
// (file, line) that are popped and unused (ret 8). Retail 32-bit layout (fog_d128a.cpp): pool +0x7A0 block count, +0x7A8 key-sorted table of
// {block, key}; block +0 base, +4 bitmap, +8 in use, +0xC prev, +0x10 next, +0x14 class; class +0x18 slot size, +0x1C slot count.
#include <cstdint>

extern "C" uint32_t fog_sub_1a9f0_lookup(const uint32_t* tab, int32_t count, uint32_t key);
extern "C" void fog_sub_1ab60(uint8_t* n, uint8_t* l);

struct Fog1b0c0Api {
    uint8_t* (*default_pool)();                       // 6FF9AF94
    void (*pool_error)(uint32_t key, uint8_t* pool);  // Fog+0x1AA80 (EAX = key, stack = pool)
};

namespace {
inline uint32_t& W(uint8_t* b, uint32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
inline uint8_t* P(uint32_t v) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(v)); }
}

// 1.13c Fog+0x1B0C0
extern "C" void fog_sub_1b0c0(const Fog1b0c0Api* api, uint8_t* pool, const uint32_t* pp) {
    if (!pool) pool = api->default_pool();
    const uint32_t key = *pp;
    if (key == 0) return;
    uint8_t* blk = P(fog_sub_1a9f0_lookup(reinterpret_cast<const uint32_t*>(P(W(pool, 0x7a8))), static_cast<int32_t>(W(pool, 0x7a0)), key));
    if (!blk || key < W(blk, 0)) { api->pool_error(key, pool); return; }
    uint8_t* cls = P(W(blk, 0x14));
    const int32_t idx = static_cast<int32_t>(key - W(blk, 0)) / static_cast<int32_t>(W(cls, 0x18));
    if (idx >= static_cast<int32_t>(W(cls, 0x1c))) { api->pool_error(key, pool); return; }
    uint32_t* word = reinterpret_cast<uint32_t*>(P(W(blk, 4)) + (idx >> 5) * 4);
    const uint32_t mask = 1u << (idx & 0x1f);
    if (*word & mask) {
        *word &= ~mask;
        W(blk, 8) = W(blk, 8) - 1;
        fog_sub_1ab60(blk, cls);
    }
}
