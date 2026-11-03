// D2Game+0x38BC0 (export 10042, GAME_GetActData): stdcall (nAct byte, p2, p3). Order (disassembled): Fog enter wrapper 0xA328 (ecx = &cs 0x6FD30BE8,
// edx = 0x125); id = global 0x6FD30C08 (0 or -1 reads as 0); LeaveCriticalSection(&cs); game = 0x2AAE0(id) stored to global 0x6FD30C04;
// nAct >= 5 is fatal 0x8D + exit; else Ordinal 11010 (thunk 0xA526, 3 stack words) on game+0xBC+4*nAct with p2, p3; a null game is fatal 0x100E,
// a null [game+0x18] fatal 0x100F + exit, else LeaveCriticalSection of it; returns the 11010 result.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game38BC0Env {
    void     (*enterWrap)(uint32_t line);                // 0xA328 with ecx = the global CS
    uint32_t (*leaveCs)(uint32_t cs);                    // LeaveCriticalSection (IAT 0xF8D00)
    uint32_t (*getGame)(uint32_t id);                    // 0x2AAE0
    uint32_t (*actQuery)(uint32_t act, uint32_t p2, uint32_t p3); // Ordinal 11010 via 0xA526
    void     (*fatal)(uint32_t line);                    // never returns
    uint32_t globalId;                                   // *(0x6FD30C08)
    uint32_t globalCs;                                   // 0x6FD30BE8
    uint32_t* globalGame;                                // 0x6FD30C04 := game
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x38BC0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_38BC0(uint8_t nAct, uint32_t p2, uint32_t p3, const D2Game38BC0Env* e)
{
    e->enterWrap(0x125);
    uint32_t id = e->globalId;
    if (id == 0 || id == 0xffffffffu) id = 0;
    e->leaveCs(e->globalCs);
    const uint32_t g = e->getGame(id);
    *e->globalGame = g;
    if (nAct >= 5) e->fatal(0x8d);
    // retail reads the act pointer before its own null check: a null game faults there
    const uint32_t r = e->actQuery(rd32(g + 0xbc + 4u * nAct), p2, p3);
    if (!g) e->fatal(0x100e);
    const uint32_t cs = rd32(g + 0x18);
    if (!cs) e->fatal(0x100f);
    e->leaveCs(cs);
    return r;
}
