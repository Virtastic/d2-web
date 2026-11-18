// 1.13c D2Game+0xA28F0 probe: the game's own sub_6FD11420 (D2Game/SKILLS/Skills.cpp), build and send a skill missile request.
// ecx = game r[0], edx = missile r[1]; stack unit r[2], skill r[3], level r[5], x r[6], y r[7], target x r[8], target y r[9], check r[10]; sc r[11], log r[12].
// Blobs: td 0, u 1, sc 2, lg 3. The unit is read through the Mirror; its coordinates, the target lookup (9FA30, which writes the request's target words
// through the log's out-writes), the to-hit stat (A42A), the hand-item check (A1790) and the send (6F930, the request is a local: only its stack tag is logged)
// are the recorder logging retail's call. CLIENTS_GetUnitX/Y are inlined in retail: path +0xC/+0x10 for unit types 2, 4, 5, else A454/A3B8 or 0 with no path.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "fn_tag.h"
#include "mirror_layouts.h"
#include "../include/D2Game/SKILLS/Skills.h"
#include "../include/D2Game/UNIT/SUnit.h"
#include "../include/D2Game/GAME/Clients.h"
#include "../include/D2Game/MISSILES/Missiles.h"
#include "../include/D2StatList.h"
#include "../compat/test_seam.h"

static HookEnv* E; static mirror::Mirror* M;
static uint32_t dw(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a28f0(uint32_t* r)
{
    HookEnv he((uint32_t*)(uintptr_t)r[12], (const uint32_t*)(uintptr_t)r[11]); E = &he;
    mirror::Mirror m(mirror::types, mirror::ntypes); M = &m;
    m.allow(r[2], fnBlobLen(r, 1));
    auto pUnit = m.get<D2UnitStrc>(r[2]);
    D2Seam<&CLIENTS_GetUnitX>::hook = [](D2UnitStrc* u) -> int32_t { const uint32_t o = M->ra(u), ut = dw(o), p = dw(o + 0x2C); return (ut == 2 || ut == 4 || ut == 5) ? (int32_t)dw(p + 0xC) : p ? (int32_t)E->h(0xA454, 0, 0, 0, 0, 0, p) : 0; };
    D2Seam<&CLIENTS_GetUnitY>::hook = [](D2UnitStrc* u) -> int32_t { const uint32_t o = M->ra(u), ut = dw(o), p = dw(o + 0x2C); return (ut == 2 || ut == 4 || ut == 5) ? (int32_t)dw(p + 0x10) : p ? (int32_t)E->h(0xA3B8, 0, 0, 0, 0, 0, p) : 0; };
    D2Seam<&D2GAME_GetXAndYFromTargetUnit_6FD14020>::hook = [](D2GameStrc* g, D2UnitStrc* u, int32_t* px, int32_t* py) -> int32_t { return (int32_t)E->h(0x9FA30, M->ra(u), (uint32_t)(uintptr_t)g, 0, E->stk(px), 0, E->stk(py)); };
    D2Seam<&STATLIST_UnitGetStatValue>::hook = [](const D2UnitStrc* u, int s, uint16_t l) -> int32_t { return (int32_t)E->h(0xA42A, 0, 0, 0, 0, 0, M->ra(u), (uint32_t)s, (uint32_t)l); };
    D2Seam<&sub_6FD118C0>::hook = [](D2GameStrc* g, D2UnitStrc* u) -> int32_t { return (int32_t)E->h(0xA1790, 0, 0, 0, M->ra(u), 0, (uint32_t)(uintptr_t)g); };
    D2Seam<&MISSILES_CreateMissileFromParams>::hook = [](D2GameStrc* g, D2MissileStrc* p) -> D2UnitStrc* { return (D2UnitStrc*)(uintptr_t)E->h(0x6F930, 0, (uint32_t)(uintptr_t)g, E->stk(p)); };
    r[4] = (uint32_t)(uintptr_t)sub_6FD11420((D2GameStrc*)(uintptr_t)r[0], (int32_t)r[1], pUnit, (int32_t)r[3], (int32_t)r[5], (int32_t)r[6], (int32_t)r[7], (int32_t)r[8], (int32_t)r[9], (int32_t)r[10]);
    D2Seam<&CLIENTS_GetUnitX>::hook = nullptr; D2Seam<&CLIENTS_GetUnitY>::hook = nullptr; D2Seam<&D2GAME_GetXAndYFromTargetUnit_6FD14020>::hook = nullptr;
    D2Seam<&STATLIST_UnitGetStatValue>::hook = nullptr; D2Seam<&sub_6FD118C0>::hook = nullptr; D2Seam<&MISSILES_CreateMissileFromParams>::hook = nullptr;
    m.commit();
    fnTagLog(r, 3, 4);
}
