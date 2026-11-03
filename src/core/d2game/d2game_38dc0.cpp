// D2Game+0x38DC0 (export 10039, SUNIT_GetServerUnitForClient): stdcall (unit type, unit id). Same lock/lookup prefix as 0x38BC0; a null game
// returns 0 (the lock is not left); else SUNIT_GetServerUnit 0xE03A0 (eax = type, ecx = game, edx = id); a null game afterwards is fatal 0x100E,
// a null [game+0x18] fatal 0x100F, else it is left; returns the unit.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game38DC0Env {
    void     (*enterWrap)(uint32_t line);
    uint32_t (*leaveCs)(uint32_t cs);
    uint32_t (*getGame)(uint32_t id);
    uint32_t (*getUnit)(uint32_t type, uint32_t game, uint32_t id);   // 0xE03A0
    void     (*fatal)(uint32_t line);
    uint32_t globalId;
    uint32_t globalCs;
    uint32_t* globalGame;
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x38DC0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_38DC0(uint32_t type, uint32_t unitId, const D2Game38DC0Env* e)
{
    e->enterWrap(0x125);
    uint32_t id = e->globalId;
    if (id == 0 || id == 0xffffffffu) id = 0;
    e->leaveCs(e->globalCs);
    const uint32_t g = e->getGame(id);
    *e->globalGame = g;
    if (!g) return 0;
    const uint32_t u = e->getUnit(type, g, unitId);
    const uint32_t cs = rd32(g + 0x18);
    if (!cs) e->fatal(0x100f);
    e->leaveCs(cs);
    return u;
}
