// src/core/d2game/d2game_batch_7f370.cpp -- two 1.13c D2Game.dll AI-skill chance handlers transcribed
// from the cached Ghidra decompiles (0x7F370, 0x7F760). Implementation + marker only (batch lane).
// Both: __fastcall(unit=ECX, game=EDX, aiTickArgs=stack). Units/records are opaque, addressed by
// retail 32-bit offsets. Callees outside the batch are seams in retail argument order. Percent rolls
// use the seed at +0x20/+0x24 (state = lo*0x6AC690C5 + hi, 64-bit).
#include <cstdint>
#include <cstring>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline int16_t rds16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void* P(uint32_t v) { return (void*)(uintptr_t)v; }

struct D2GameBatch7F370Env {
    void  (*ai_action_F2540)(void* unit, void* aiCtl, void* target, int a, int b, int c, int d);   // GUESS: FUN_6fd12540
    void  (*ai_wait_F22C0)(int frames);                                                              // GUESS: FUN_6fd122c0
    void  (*ai_F1D90)(void* target, void* unit, void* aiCtl, int a, int b);                         // GUESS: FUN_6fd11d90
    void  (*ai_F2380)(void* aiCtl);                                                                  // GUESS: FUN_6fd12380
    void  (*ordinal_11103)(void* aiCtl, int zero);                                                   // GUESS: Ordinal_11103
    void* (*fn_1320)();                                                                              // GUESS: FUN_6fc21320
    void  (*cmd_75E10)(void* unit, void* cmd);                                                       // GUESS: FUN_6fc95e10
    const uint8_t* bitMasks;                                                                         // gdwBitMasks image
};
D2GameBatch7F370Env g_d2gameBatch7F370Env;

// Seed step: returns the roll (low 32 bits) and stores the new 64-bit state.
static uint32_t roll(void* aiCtl) {
    uint64_t v = (uint64_t)rd32(aiCtl, 0x20) * 0x6AC690C5u + rd32(aiCtl, 0x24);
    std::memcpy((uint8_t*)aiCtl + 0x20, &v, 8);
    return (uint32_t)v;
}

// 1.13c D2Game+0x7F370
extern "C" void D2Game_7F370(void* unit, void* aiCtl, void* args) {
    D2GameBatch7F370Env& E = g_d2gameBatch7F370Env;
    void* target = P(rd32(args, 8));
    uint32_t rec = rd32(args, 0x1C);
    uint32_t diff = ((const uint8_t*)unit)[0x6D];
    if (rd32(args, 0x18) == 0) {
        if ((int)(roll(aiCtl) % 100) < rds16(P(rec), 0x56 + diff * 2)) { E.ai_F1D90(target, unit, aiCtl, 2, 1); return; }
        if ((int)(roll(aiCtl) % 100) >= rds16(P(rec), 0x56 + diff * 2)) { E.ai_wait_F22C0(rds16(P(rec), 0x62 + diff * 2)); return; }
    } else {
        if ((int)(roll(aiCtl) % 100) < rds16(P(rec), 0x5C + diff * 2)) {
            E.ordinal_11103(aiCtl, 0);
            uint32_t cmd[7] = {0, 0, 0, 0, 0, 0, 0};   // GUESS: stack command block at local_28..local_c
            cmd[0] = 4; cmd[1] = (uint32_t)(uintptr_t)aiCtl;
            uint8_t flag = 100;
            if (aiCtl) {
                void* r = E.fn_1320();
                if (r && (((const uint8_t*)r)[0x104] & rd32(E.bitMasks, 0x10))) flag = 0x65;
            }
            ((uint8_t*)cmd)[0x15] = flag;   // uStack_14 high byte of low word
            E.cmd_75E10(unit, cmd);
            return;
        }
        if (rds16(P(rec), 0x68 + diff * 2) <= (int)(roll(aiCtl) % 100)) { E.ai_wait_F22C0(rds16(P(rec), 0x62 + diff * 2)); return; }
        int v = (roll(aiCtl) & 0xFF) < 0x80 ? 5 : 6;
        uint32_t ex = 0;
        if (aiCtl && rd32(aiCtl, 0) == 1 && rd32(aiCtl, 0x14)) ex = rd32(P(rd32(aiCtl, 0x14)), 0x2C);
        wr32(P(ex), 0x18, v);
        ((uint8_t*)P(ex))[0x20] = 2;
    }
    E.ai_action_F2540(unit, aiCtl, target, 2, 0, 0, 1);
}

// 1.13c D2Game+0x7F760
extern "C" void D2Game_7F760(void* unit, void* aiCtl, void* args) {
    D2GameBatch7F370Env& E = g_d2gameBatch7F370Env;
    void* target = P(rd32(args, 8));
    void* st = P(rd32(args, 0));
    uint32_t rec = rd32(args, 0x1C);
    uint32_t diff = ((const uint8_t*)unit)[0x6D];
    if (rd32(args, 0x18) == 0) {
        wr32(st, 0x14, 0);
        if ((int)(roll(aiCtl) % 100) < rds16(P(rec), 0x62 + diff * 2)) { E.ai_action_F2540(unit, aiCtl, target, 2, 0, 0, 1); return; }
        E.ai_wait_F22C0(rds16(P(rec), 0x68 + diff * 2));
        return;
    }
    if (rd32(st, 0x14) != 0) {
        if (rds16(P(rec), 0x56 + diff * 2) <= (int)(roll(aiCtl) % 100)) { E.ai_wait_F22C0(rds16(P(rec), 0x5C + diff * 2)); return; }
    }
    wr32(st, 0x14, 1);
    if (rds16(P(rec), 0x170) >= 0 && (int)(roll(aiCtl) % 100) < rds16(P(rec), 0x6E + diff * 2)) { E.ai_F2380(aiCtl); return; }
    if (rds16(P(rec), 0x172) >= 0 && (int)(roll(aiCtl) % 100) < rds16(P(rec), 0x74 + diff * 2)) { E.ai_F2380(aiCtl); return; }
    E.ordinal_11103(aiCtl, 0);
    if (aiCtl) E.fn_1320();
    uint32_t cmd[7] = {0};   // GUESS: stack command block (&stack0xffffffd8) left uninitialised in retail
    E.cmd_75E10(unit, cmd);
}
