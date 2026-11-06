#pragma once
#include <cstdint>

// Callee seams of the AI scan callbacks and monster-AI steps 0x7B860..0x7C1D0, carrying the registers retail passes.
struct D2g7b860Env {
    int32_t (*a688)(const uint8_t* u);                            // stdcall(unit)
    int32_t (*a3a0)(const uint8_t* u);                            // stdcall(unit): max of stat 6
    int32_t (*a42a)(const uint8_t* u, int32_t stat, int32_t lyr); // stdcall(unit, stat, layer)
    int32_t (*a514)(const uint8_t* u);                            // stdcall(unit)
    int32_t (*a6f4)(const uint8_t* unit, const uint8_t* cand);    // stdcall(unit, cand)
    int32_t (*a718)(const uint8_t* u);                            // stdcall(unit)
    int32_t (*ad60)(const uint8_t* u);                            // stdcall(unit)
    int32_t (*ad4e)(const uint8_t* u);                            // stdcall(unit)
    int32_t (*a508)(const uint8_t* u);                            // stdcall(unit)
    int32_t (*a3b8)(const uint8_t* path);                         // stdcall(path)
    int32_t (*a454)(const uint8_t* path);                         // stdcall(path)
    uint32_t (*a478)(const uint8_t* u, int32_t x, int32_t y, uint32_t flag);  // stdcall(unit, x, y, flag)
    void (*afee)(uint32_t v);                                     // stdcall(v)
    void (*a6d6)(const uint8_t* u, int32_t mode);                 // stdcall(unit, mode)
    void (*ab08)(const uint8_t* path, int32_t z);                 // stdcall(path, 0)
    void (*a496)(const uint8_t* u, int32_t z);                    // stdcall(unit, 0)
    int32_t (*c1900)(const uint8_t* unit, const uint8_t* cand);   // EAX = unit, EBX = cand
    int32_t (*c18a0)(const uint8_t* u);                           // EAX = unit
    const uint8_t* (*c1220)(uint32_t id);                         // EAX = class id: MonStats row
    const uint8_t* (*c1320)(uint32_t id);                         // EAX = class id: MonStats2 row
    int32_t (*c13b0)(uint32_t id, uint32_t bit);                  // EAX = class id, EDX = bit
    int32_t (*c22a80)(const uint8_t* u, uint32_t bit);            // EAX = unit, stack = bit
    void (*c8e6f0)(const uint8_t* node, const uint8_t* game);      // EAX = node, EDI = game
    uint32_t (*c8ea80)(const uint8_t* game, const uint8_t* unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);  // ECX, EDX, 5 stack
    uint32_t (*f22c0)(const uint8_t* game, const uint8_t* unit, int32_t v);   // EAX = game, ESI = unit, stack = frames
    uint32_t (*f2380)(uint32_t eax, const uint8_t* unit, int32_t skill, uint32_t target, uint32_t z0, uint32_t z1);  // EAX = skill|AL mode, ECX = unit, 4 stack
    void (*f2220)(const uint8_t* game, const uint8_t* unit, const uint8_t* target);  // EAX = 4, EDI = unit, stack = game, target (ret 8)
    uint32_t (*c75e10)(const uint8_t* game, const uint32_t* blk, uint32_t n); // stdcall(game, &blk[8], n)
    int32_t tableCount;                                           // *(*sgptDataTables + 0xA80)
    const uint8_t* bitMasks;                                      // gdwBitMasks
};

extern "C" uint32_t d2_game_7b860(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7b860Env* e);
extern "C" uint32_t d2_game_7b8e0(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7b860Env* e);
extern "C" uint32_t d2_game_7ba40(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7b860Env* e);
extern "C" uint32_t d2_game_7bb00(const uint8_t* unit, const uint8_t* cand, uint8_t* acc, const D2g7b860Env* e);
extern "C" uint32_t d2_game_7be10(const uint8_t* game, uint8_t* unit, const D2g7b860Env* e);
extern "C" uint32_t d2_game_7bf50(const uint8_t* game, uint8_t* unit, const uint8_t* ctx, const D2g7b860Env* e);
extern "C" uint32_t d2_game_7c060(const uint8_t* game, uint8_t* unit, const uint8_t* ctx, const D2g7b860Env* e);
extern "C" uint32_t d2_game_7c580(const uint8_t* game, uint8_t* unit, const uint8_t* ctx, const D2g7b860Env* e);
extern "C" uint32_t d2_game_7c1d0(const uint8_t* game, uint8_t* unit, const uint8_t* ctx, const D2g7b860Env* e);

namespace d2g7b860 {
inline uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; __builtin_memcpy(&v, p + o, 4); return v; }
inline int32_t rd16(const uint8_t* p, uint32_t o) { int16_t v; __builtin_memcpy(&v, p + o, 2); return v; }
inline void wr(uint8_t* p, uint32_t o, uint32_t v) { __builtin_memcpy(p + o, &v, 4); }
inline const uint8_t* ptr(const uint8_t* p, uint32_t o) { return (const uint8_t*)(uintptr_t)rd(p, o); }
}
