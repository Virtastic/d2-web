// src/core/d2game/d2game_batch_509e0.cpp -- 1.13c D2Game.dll sub_509E0 (RE-DONE, alpha lane), from build-wasm/ghidra-d2game/decomp/0x509E0.c.
// Caller 0x51340. param_1 = game (+0xA8 = tick), param_2 = unit (type@0, pPlayerData@0x14, pOwnerList@0xDC, flags@0xC4),
// param_3 = enable flag, in_EAX = optional {u32 a; _; _; u32 b} pair. Callees are seams on the Env.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
}

struct D2GameSub509E0Env {
    uint8_t* (*ptr)(uint32_t p);                                                  // 32-bit blob pointer -> host pointer
    void     (*f8E6F0)(void* node);                                               // 0x8E6F0
    void     (*f8EA80)(void* game, void* unit, int a, uint32_t b, int c, int d, int e); // 0x8EA80
    void     (*fE0460)();                                                         // 0xE0460
    void     (*o10945)(void* unit, int a, int b);
    int      (*o10786)();                                                         // seam: ambient register args
    void     (*f232B0)(void* unit, int a);                                        // 0x232B0
};

// 1.13c D2Game+0x509E0
void sub_509E0(D2GameSub509E0Env& e, uint8_t* game, uint8_t* unit, int enable, const uint32_t* pair) {
    if (!enable) return;
    uint32_t b = 6, a = 0xffffffff;
    if (pair) { b = pair[0]; a = pair[3]; }
    if (unit) {
        auto statePtr = [&]() -> uint8_t* {
            uint32_t pd = rd32(unit, 0x14);
            return pd ? e.ptr(rd32(e.ptr(pd), 0x28)) : nullptr;
        };
        if (rd32(unit, 0) == 1) {
            uint8_t* s = statePtr();
            wr32(s, 0x2c, a); wr32(s, 0x30, b); wr32(s, 0x28, (uint32_t)(uintptr_t)game);
        }
        if (rd32(unit, 0) == 1) {
            uint8_t* s = statePtr();
            if (!pair) { wr32(s, 0xc, 0xffffffff); wr32(s, 0x10, 1); }
            else       { wr32(s, 0xc, pair[3]);    wr32(s, 0x10, pair[0]); }
        }
        for (uint8_t* n = e.ptr(rd32(unit, 0xdc)); n;) {
            uint8_t* cur = n;
            n = e.ptr(rd32(n, 0x24));
            if (cur[0] == 2) e.f8E6F0(cur);
        }
    }
    e.f8EA80(game, unit, 2, rd32(game, 0xa8) + 0xf, 0, 0, 0);
    e.fE0460();
    if (unit) {
        if (rd32(unit, 0) == 1 && rd32(unit, 0x14)) {
            uint8_t* pd = e.ptr(rd32(unit, 0x14));
            wr32(pd, 0x38, 0); wr32(pd, 0x34, 0);
        }
        wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x80000000u);
    }
    e.o10945(unit, 0x60, 1);
    int d = e.o10786();
    if (d > 0) {
        e.f232B0(unit, 0);
        e.f8EA80(game, unit, 7, rd32(game, 0xa8) + (uint32_t)d, 0, 0, 0);
    }
}
