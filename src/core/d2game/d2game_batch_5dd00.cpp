// src/core/d2game/d2game_batch_5dd00.cpp -- 1.13c D2Game.dll sub_5DD00 (RE-DONE, alpha lane), from build-wasm/ghidra-d2game/decomp/0x5DD00.c.
// Callers 0x5EC30, 0x5DEF0. param_1 = quest data (+4 = game); unaff_ESI = player unit (explicit). Walks the
// player's data +0x30 list head; if an entry with nonzero first dword is found, 0x48410 runs first. Callees are seams.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
}

struct D2GameSub5DD00Env {
    uint8_t* (*ptr)(uint32_t p);                              // 32-bit blob pointer to host pointer
    void     (*f48410)(uint32_t game, void* unit, void* arg); // 0x48410 stack (game, unit, 0)
    void     (*o10159)(uint32_t a);                           // Common #10159, stack = unit[0x2c]
    void     (*f8E6F0)(void* node, uint32_t game);            // 0x8E6F0: EAX = node, EDI = game
    void     (*o10887)(void* unit, int32_t a, int32_t b, int32_t c); // #10887 stack (unit, 6, 0, 0)
    void     (*o11103)(void* unit, int32_t a);                // #11103 stack (unit, 0)
    void     (*f75E10)(uint32_t game, int32_t* msg, int32_t one); // 0x75E10 stack (game, &msg, 1)
    void     (*o10158)(void* unit);                           // #10158 stack (unit)
};

// 1.13c D2Game+0x5DD00 (ESI = unit, stack = questData; ret 4), checked against the disassembly.
void sub_5DD00(D2GameSub5DD00Env& e, uint8_t* questData, uint8_t* unit) {
    uint8_t* list = nullptr;
    if (unit && rd32(unit, 0) == 1 && rd32(unit, 0x14)) list = e.ptr(rd32(e.ptr(rd32(unit, 0x14)), 0x30));
    uint8_t* node = e.ptr(rd32(list, 0));
    while (node) {
        if (rd32(node, 0) != 0) { e.f48410(rd32(questData, 4), unit, nullptr); break; }
        node = e.ptr(rd32(node, 8));
    }
    e.o10159(rd32(unit, 0x2c));
    uint32_t game = rd32(questData, 4);
    for (uint8_t* n = e.ptr(rd32(unit, 0xdc)); n;) {
        uint8_t* cur = n;
        n = e.ptr(rd32(n, 0x24));
        if (cur[0] == 2) e.f8E6F0(cur, game);
    }
    e.o10887(unit, 6, 0, 0);
    e.o11103(unit, 0);
    int32_t msg[8] = {0};                // 0x20-byte message: [0] = 0xc, [1] = unit, byte 0x15 = 0x64
    msg[0] = 0xc;
    msg[1] = (int32_t)(uintptr_t)unit;
    ((uint8_t*)msg)[0x15] = 0x64;
    e.f75E10(game, msg, 1);
    e.o10158(unit);
    wr32(unit, 0xc4, rd32(unit, 0xc4) | 1);
}
