// 1.13c D2Game+0xE0F40 probe: regs EAX=pRoom,
// ECX=nY, EDX=pUnit, stack nX, pGame, a6. r[0]=pRoom, r[1]=nY, r[2]=pUnit, r[3]=nX, r[5]=pGame, r[6]=a6, r[7]=aux:
// aux+8 = the hook log (from `hooklog`), aux+0x10/0x14 = the level/act the two D2Common hooks return, aux+0x18/0x1c
// = the 0xDF9A0 clients test and the 0xA478 UNITS_GetRoom return (hook values the log cannot carry), aux+0x120 =
// the objects.txt nHasCollision bytes the 0xA520 hook hands back through the aux blob.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "../d2game/sunit_add_e0f40.h"

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_e0f40(uint32_t* r)
{
    const uint32_t aux = r[7];
    const uint32_t sc[11] = { 0xA556, HookEnv::rd32(aux + 0x10), 0xA56E, HookEnv::rd32(aux + 0x14), 0xA520, aux,
                              0xDF9A0, HookEnv::rd32(aux + 0x18), 0xA478, HookEnv::rd32(aux + 0x1C), 0 };
    HookEnv e((uint32_t*)(uintptr_t)HookEnv::rd32(aux + 8), sc);
    r[4] = e0f40::Add(e, r[2], r[3], r[1], r[5], r[0], r[6]);
}
