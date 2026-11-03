// D2Game+0x38C70 (export 10005): count the unit entries of the current game under the global game-table lock.
// No arguments. Order (disassembled): Fog enter wrapper 0xA328 (ecx = &cs 0x6FD30BE8, edx = 0x125); id = global 0x6FD30C08 (0 or -1 reads as 0);
// LeaveCriticalSection(&cs); game = 0x2AAE0(id) (stdcall, 1 arg), stored to global 0x6FD30C04, a null game is fatal 0x7B + exit; count the entries of the
// 128 bucket chains at game+0x1320 (next link at +0xE4); [game+0x18] (the game's own lock) null is fatal 0x100F + exit, else LeaveCriticalSection of it;
// returns the count.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game38C70Env {
    void     (*enterWrap)(uint32_t line);                // 0xA328 with ecx = the global CS
    uint32_t (*leaveCs)(uint32_t cs);                    // LeaveCriticalSection (IAT 0xF8D00)
    uint32_t (*getGame)(uint32_t id);                    // 0x2AAE0
    void     (*fatal)(uint32_t line);                    // never returns
    uint32_t globalId;                                   // *(0x6FD30C08)
    uint32_t globalCs;                                   // 0x6FD30BE8
    uint32_t* globalGame;                                // 0x6FD30C04 := game
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x38C70
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_38C70(const D2Game38C70Env* e)
{
    e->enterWrap(0x125);
    uint32_t id = e->globalId;
    if (id == 0 || id == 0xffffffffu) id = 0;
    e->leaveCs(e->globalCs);
    const uint32_t g = e->getGame(id);
    *e->globalGame = g;
    if (!g) e->fatal(0x7b);
    uint32_t n = 0;
    for (uint32_t i = 0; i < 0x80; i++)
        for (uint32_t u = rd32(g + 0x1320 + i * 4); u; u = rd32(u + 0xe4)) n++;
    const uint32_t cs = rd32(g + 0x18);
    if (!cs) e->fatal(0x100f);
    e->leaveCs(cs);
    return n;
}
