// 1.13c D2Game+0xA1790 probe: the game's own sub_6FD118C0 (D2Game/SKILLS/Skills.cpp), the two hand slots' stack item.
// ebx = unit r[0], stack game r[1], sc r[2], log r[3]. Blobs: u 0, sc 1, lg 2. The unit is read through the Mirror (type, inventory); every callee
// (372C0, A430 for the bow and crossbow, A7AE, A940, A9AC, A802, A16A0) is the recorder logging retail's call.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "fn_tag.h"
#include "mirror_layouts.h"
#include "../include/D2Game/SKILLS/Skills.h"
#include "../include/D2Game/PLAYER/Player.h"
#include "../include/D2Inventory.h"
#include "../include/D2Items.h"
#include "../include/D2StatList.h"
#include "../compat/test_seam.h"

static HookEnv* E; static mirror::Mirror* M;
static uint32_t ra(const void* p) { return M->ra(p); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a1790(uint32_t* r)
{
    HookEnv he((uint32_t*)(uintptr_t)r[3], (const uint32_t*)(uintptr_t)r[2]); E = &he;
    mirror::Mirror m(mirror::types, mirror::ntypes); M = &m;
    m.allow(r[0], fnBlobLen(r, 0));
    auto pUnit = m.get<D2UnitStrc>(r[0]);
    D2Seam<&sub_6FC7C7B0>::hook = [](D2UnitStrc* u) -> D2UnitStrc* { return (D2UnitStrc*)(uintptr_t)E->h(0x372C0, 0, 0, 0, 0, 0, ra(u)); };
    D2Seam<&ITEMS_CheckItemTypeId>::hook = [](const D2UnitStrc* u, int t) -> BOOL { return (BOOL)E->h(0xA430, 0, 0, 0, 0, 0, ra(u), (uint32_t)t); };
    D2Seam<&INVENTORY_GetItemFromBodyLoc>::hook = [](D2InventoryStrc* i, int l) -> D2UnitStrc* { return (D2UnitStrc*)(uintptr_t)E->h(0xA7AE, 0, 0, 0, 0, 0, ra(i), (uint32_t)l); };
    D2Seam<&ITEMS_CheckIfStackable>::hook = [](D2UnitStrc* u) -> uint32_t { return E->h(0xA940, 0, 0, 0, 0, 0, ra(u)); };
    D2Seam<&ITEMS_CheckIfThrowable>::hook = [](D2UnitStrc* u) -> int { return (int)E->h(0xA9AC, 0, 0, 0, 0, 0, ra(u)); };
    D2Seam<&STATLIST_UnitGetItemStatOrSkillStatValue>::hook = [](D2UnitStrc* u, int s, uint16_t l) -> int32_t { return (int32_t)E->h(0xA802, 0, 0, 0, 0, 0, ra(u), (uint32_t)s, (uint32_t)l); };
    D2Seam<&sub_6FD11340>::hook = [](D2GameStrc* g, D2UnitStrc* p, D2UnitStrc*) -> int32_t { return (int32_t)E->h(0xA16A0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)g, ra(p)); };
    r[4] = (uint32_t)sub_6FD118C0((D2GameStrc*)(uintptr_t)r[1], pUnit);
    D2Seam<&sub_6FC7C7B0>::hook = nullptr; D2Seam<&ITEMS_CheckItemTypeId>::hook = nullptr; D2Seam<&INVENTORY_GetItemFromBodyLoc>::hook = nullptr;
    D2Seam<&ITEMS_CheckIfStackable>::hook = nullptr; D2Seam<&ITEMS_CheckIfThrowable>::hook = nullptr;
    D2Seam<&STATLIST_UnitGetItemStatOrSkillStatValue>::hook = nullptr; D2Seam<&sub_6FD11340>::hook = nullptr;
    m.commit();
    fnTagLog(r, 2, 3);
}
