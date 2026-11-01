// D2Game: a fresh player unit is attached to its game and initialised, in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

struct D2Game38620Env {
    void (*statInit)(uint32_t zero, uint32_t unit);                                  // 0xA448 thunk (stdcall 2)
    uint32_t (*seed)(uint32_t memPool);                                              // 0xAD8A thunk (stdcall 1)
    void (*b2b2)(uint32_t unit);                                                     // 0xB2B2 thunk (stdcall 1)
    void (*setCallback)(uint32_t unit, uint32_t zero, uint32_t fn, uint32_t game);   // 0xADAE thunk (stdcall 4)
    void (*b29a)(uint32_t unit);                                                     // 0xB29A thunk (stdcall 1)
    void (*createIntros)(uint32_t unit, uint32_t game);                              // 0x376F0: EAX = unit, ECX = game
    uint32_t (*playerData)(uint32_t unit);                                           // 0xA4C0 thunk (stdcall 1)
    void (*freeTimer)(uint32_t game, uint32_t unit);                                 // 0x8E880: EAX = game; stack unit (ret 4)
    void (*b24d0)(uint32_t game, uint32_t unit);                                     // 0xB24D0: EAX = game; stack unit (ret 4)
    void (*s97120)(uint32_t unit);                                                   // 0x97120 (stdcall 1)
    void (*setMode)(uint32_t unit, uint32_t one);                                    // 0xE0460: EAX = unit, BL = 2; stack 1 (ret 4)
    uint32_t callbackFn;                                                             // module address of D2Game+0xD9470
};

// 1.13c D2Game+0x38620
// ESI = unit, EAX = game; stack id; ret 4. A null unit, or a unit whose type (+0) is not 0 after the intro setup,
// is a Fog assert (exit). The id goes to +0xC, flags 0xE are set in +0xC4, the stat list is set up, +0xA8 takes a
// seed from the game's memory pool (+0x1C), the callback D2Game+0xD9470 is registered, 0x376F0 creates the intros,
// the player data's +0x9C is cleared, a pending timer (+0xDC) is freed by 0x8E880 and +0xDC cleared, then 0xB24D0,
// 0x97120 and 0xE0460 (BL = 2, stack 1) finish it.
void UNITS_AttachToRoomAndInit_38620(uint32_t unit, uint32_t game, uint32_t id, const D2Game38620Env& env)
{
    if (unit == 0) _exit(0x47);
    wr32(unit + 0xC, id);
    wr32(unit + 0xC4, rd32(unit + 0xC4) | 0xE);
    env.statInit(0, unit);
    wr32(unit + 0xA8, env.seed(rd32(game + 0x1C)));
    env.b2b2(unit);
    env.setCallback(unit, 0, env.callbackFn, game);
    env.b29a(unit);
    env.createIntros(unit, game);
    if (rd32(unit) != 0) _exit(0x4BC);
    wr32(env.playerData(unit) + 0x9C, 0);
    if (rd32(unit + 0xDC) != 0) env.freeTimer(game, unit);
    wr32(unit + 0xDC, 0);
    env.b24d0(game, unit);
    env.s97120(unit);
    env.setMode(unit, 1);
}
