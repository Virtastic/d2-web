// 1.13c D2Game+0xDF7B0 probe: the game's own sub_6FCBCE70 (D2Game/UNIT/SUnit.cpp).
// ESI = pUnit r[0], stack pGame r[1], sc r[2], log r[3]; blobs: game 0, unit 1, anim 2, sc 3, log 4. The game and unit blobs are read through the
// retail-layout mirror; UNITS_GetFrameBonus (0xA622) and the queued event (0x8EA80, EVENT_SetEvent inlined in retail) are the recorders.
// The event type is not a recorded word (the descriptor's hook logs the registers and stack words 1..4), so the stand-in logs 0 for it.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "fn_tag.h"
#include "mirror_layouts.h"
#include "../include/D2Game/UNIT/SUnit.h"
#include "../include/D2Game/GAME/Event.h"
#include "../include/D2DataTbls.h"
#include "../include/Units/Units.h"
#include "../compat/test_seam.h"

static HookEnv* E; static mirror::Mirror* M;

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_df7b0(uint32_t* r)
{
    HookEnv he((uint32_t*)(uintptr_t)r[3], (const uint32_t*)(uintptr_t)r[2]); E = &he;
    mirror::Mirror m(mirror::types, mirror::ntypes); M = &m;
    m.allow(r[1], fnBlobLen(r, 0));
    m.allow(r[0], fnBlobLen(r, 1));
    if (const uint32_t anim = *(const uint32_t*)(uintptr_t)(r[0] + 0x50)) m.allow(anim, fnBlobLen(r, 2));
    auto pGame = m.get<D2GameStrc>(r[1]);
    auto pUnit = m.get<D2UnitStrc>(r[0]);
    D2Seam<&UNITS_GetFrameBonus>::hook = [](D2UnitStrc* u) -> int32_t { return (int32_t)E->h(0xA622, 0, 0, 0, 0, 0, M->ra(u)); };
    D2Seam<&EVENT_SetEvent>::hook = [](D2GameStrc* g, D2UnitStrc* u, D2C_EventTypes, int32_t expire, int32_t id, int32_t param) {
        E->h(0x8EA80, 0, M->ra(g), M->ra(u), 0, 0, (uint32_t)expire, 0, (uint32_t)id, (uint32_t)param); };
    sub_6FCBCE70(pGame, pUnit);
    D2Seam<&UNITS_GetFrameBonus>::hook = nullptr; D2Seam<&EVENT_SetEvent>::hook = nullptr;
    m.commit();
    r[4] = 0;   // void: the descriptor's retmask 0x0 makes the stale eax not a record
    fnTagLog(r, 4, 5);
}
