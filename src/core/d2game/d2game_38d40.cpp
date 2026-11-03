// D2Game+0x38D40 (export 10018, GAME_GetCurrentGameField): stdcall (index). Same lock/lookup prefix as 0x38BC0 (0xA328, id from 0x6FD30C08, leave
// the global CS, game = 0x2AAE0(id) cached to 0x6FD30C04); a null game is fatal 0x69; value = dword at game+0xBC+4*index; a null [game+0x18]
// is fatal 0x100F, else it is left; returns the value.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game38D40Env {
    void     (*enterWrap)(uint32_t line);
    uint32_t (*leaveCs)(uint32_t cs);
    uint32_t (*getGame)(uint32_t id);
    void     (*fatal)(uint32_t line);
    uint32_t globalId;
    uint32_t globalCs;
    uint32_t* globalGame;
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x38D40
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_38D40(uint32_t index, const D2Game38D40Env* e)
{
    e->enterWrap(0x125);
    uint32_t id = e->globalId;
    if (id == 0 || id == 0xffffffffu) id = 0;
    e->leaveCs(e->globalCs);
    const uint32_t g = e->getGame(id);
    *e->globalGame = g;
    if (!g) e->fatal(0x69);
    const uint32_t v = rd32(g + 0xbc + 4u * index);
    const uint32_t cs = rd32(g + 0x18);
    if (!cs) e->fatal(0x100f);
    e->leaveCs(cs);
    return v;
}
