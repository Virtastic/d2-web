// 1.13c D2Game+0x9FA30 probe: the game's own D2GAME_GetXAndYFromTargetUnit_6FD14020 (D2Game/SKILLS/Skills.cpp).
// eax = unit r[0], ebx = pX r[1], ecx = game r[2], stack pY r[3], sc r[5], log r[6]. Blobs: td 0, u 1, t 2, px 3, py 4, sc 5, lg 6.
// The unit and its target are read through the Mirror as the retail image holds them; the callees (E07A0, A598, AAC6, AAD8, and the inlined X/Y reads, which
// retail resolves to A454/A3B8, or to the path +0xC/+0x10 for unit types 2, 4, 5) are the recorder logging retail's call.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "fn_tag.h"
#include "mirror_layouts.h"
#include "../include/D2Game/SKILLS/Skills.h"
#include "../include/D2Game/UNIT/SUnit.h"
#include "../include/D2Game/GAME/Clients.h"
#include "../include/Path/Path.h"
#include "../compat/test_seam.h"

static HookEnv* E; static mirror::Mirror* M; static uint32_t g_self;
static uint32_t dw(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9fa30(uint32_t* r)
{
    HookEnv he((uint32_t*)(uintptr_t)r[6], (const uint32_t*)(uintptr_t)r[5]); E = &he;
    mirror::Mirror m(mirror::types, mirror::ntypes); M = &m;
    m.allow(r[16], fnBlobLen(r, 1));   // unit
    m.allow(r[18], fnBlobLen(r, 2));   // target
    auto pUnit = m.get<D2UnitStrc>(r[0]); g_self = r[0];
    D2Seam<&sub_6FCBC7E0>::hook = [](D2GameStrc* g, D2UnitStrc* u) -> void { E->h(0xE07A0, M->ra(u), (uint32_t)(uintptr_t)g); };
    D2Seam<&PATH_GetTargetUnit>::hook = [](D2DynamicPathStrc* p) -> D2UnitStrc* { const uint32_t t = (uint32_t)E->h(0xA598, 0, 0, 0, 0, 0, M->ra(p)); return t == g_self ? M->get<D2UnitStrc>(t) : (D2UnitStrc*)(uintptr_t)t; };   // the unit itself is the mirror copy, so the target-is-self test sees one pointer
    D2Seam<&D2COMMON_10175_PathGetFirstPointX>::hook = [](D2DynamicPathStrc* p) -> int { return (int)E->h(0xAAC6, 0, 0, 0, 0, 0, M->ra(p)); };
    D2Seam<&D2COMMON_10176_PathGetFirstPointY>::hook = [](D2DynamicPathStrc* p) -> int { return (int)E->h(0xAAD8, 0, 0, 0, 0, 0, M->ra(p)); };
    D2Seam<&CLIENTS_GetUnitX>::hook = [](D2UnitStrc* u) -> int32_t { const uint32_t o = M->ra(u), ut = dw(o), p = dw(o + 0x2C); return (ut == 2 || ut == 4 || ut == 5) ? (int32_t)dw(p + 0xC) : p ? (int32_t)E->h(0xA454, 0, 0, 0, 0, 0, p) : 0; };
    D2Seam<&CLIENTS_GetUnitY>::hook = [](D2UnitStrc* u) -> int32_t { const uint32_t o = M->ra(u), ut = dw(o), p = dw(o + 0x2C); return (ut == 2 || ut == 4 || ut == 5) ? (int32_t)dw(p + 0x10) : p ? (int32_t)E->h(0xA3B8, 0, 0, 0, 0, 0, p) : 0; };
    r[4] = (uint32_t)D2GAME_GetXAndYFromTargetUnit_6FD14020((D2GameStrc*)(uintptr_t)r[2], pUnit, (int32_t*)(uintptr_t)r[1], (int32_t*)(uintptr_t)r[3]);
    D2Seam<&sub_6FCBC7E0>::hook = nullptr; D2Seam<&PATH_GetTargetUnit>::hook = nullptr; D2Seam<&D2COMMON_10175_PathGetFirstPointX>::hook = nullptr; D2Seam<&D2COMMON_10176_PathGetFirstPointY>::hook = nullptr;
    D2Seam<&CLIENTS_GetUnitX>::hook = nullptr; D2Seam<&CLIENTS_GetUnitY>::hook = nullptr;
    m.commit();
    fnTagLog(r, 6, 7);
}
