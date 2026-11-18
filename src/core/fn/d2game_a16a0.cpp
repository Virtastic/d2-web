// 1.13c D2Game+0xA16A0 probe: the game's own sub_6FD11340 (D2Game/SKILLS/Skills.cpp), spend one use of a stack item.
// esi = item r[0], stack game r[1], player r[2], sc r[3], log r[5]. The unit words are only handed to callees, so the retail addresses are passed through as they are;
// every callee is the recorder logging retail's call (the client read is inlined in retail: type 0, then A4C0 and the player data's client at +0x9C).
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "fn_tag.h"
#include "../include/D2Game/SKILLS/Skills.h"
#include "../include/D2Game/UNIT/SUnit.h"
#include "../include/D2Game/GAME/SCmd.h"
#include "../include/D2Game/ITEMS/Items.h"
#include "../include/D2Game/PLAYER/PlrModes.h"
#include "../include/D2StatList.h"
#include "../compat/test_seam.h"

static HookEnv* E;
static uint32_t dw(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static uint32_t ra(const void* p) { return (uint32_t)(uintptr_t)p; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a16a0(uint32_t* r)
{
    HookEnv he((uint32_t*)(uintptr_t)r[5], (const uint32_t*)(uintptr_t)r[3]); E = &he;
    D2Seam<&STATLIST_UnitGetStatValue>::hook = [](const D2UnitStrc* u, int s, uint16_t l) -> int32_t { return (int32_t)E->h(0xA42A, 0, 0, 0, 0, 0, ra(u), (uint32_t)s, (uint32_t)l); };
    D2Seam<&STATLIST_SetUnitStat>::hook = [](D2UnitStrc* u, int s, int v, uint16_t l) { E->h(0xA472, 0, 0, 0, 0, 0, ra(u), (uint32_t)s, (uint32_t)v, (uint32_t)l); };
    D2Seam<&sub_6FC51310>::hook = [](D2GameStrc* g, D2UnitStrc* u) { E->h(0xEA10, 0, 0, 0, ra(u), 0, ra(g)); };
    D2Seam<&SUNIT_GetClientFromPlayer>::hook = [](D2UnitStrc* u, const char*, int32_t) -> D2ClientStrc* { return (D2ClientStrc*)(uintptr_t)((u && dw(ra(u)) == 0) ? dw((uint32_t)E->h(0xA4C0, 0, 0, 0, 0, 0, ra(u)) + 0x9C) : 0); };
    D2Seam<&D2GAME_PACKETS_SendPacket0x3E_6FC3EC20>::hook = [](D2ClientStrc* c, D2UnitStrc* u, uint8_t a, int32_t s, uint32_t v, uint16_t) { E->h(0x8B010, ra(u), 0, 0, 0, 0, ra(c), (uint32_t)a, (uint32_t)s, v); };
    D2Seam<&STATLIST_GetMaxDurabilityFromUnit>::hook = [](D2UnitStrc* u) -> int { return (int)E->h(0xAA00, 0, 0, 0, 0, 0, ra(u)); };
    D2Seam<&sub_6FC80A30>::hook = [](D2GameStrc* g, D2UnitStrc* u) { E->h(0x79100, ra(u), 0, 0, 0, 0, ra(g)); };
    r[4] = (uint32_t)sub_6FD11340((D2GameStrc*)(uintptr_t)r[1], (D2UnitStrc*)(uintptr_t)r[2], (D2UnitStrc*)(uintptr_t)r[0]);
    D2Seam<&STATLIST_UnitGetStatValue>::hook = nullptr; D2Seam<&STATLIST_SetUnitStat>::hook = nullptr; D2Seam<&sub_6FC51310>::hook = nullptr;
    D2Seam<&SUNIT_GetClientFromPlayer>::hook = nullptr; D2Seam<&D2GAME_PACKETS_SendPacket0x3E_6FC3EC20>::hook = nullptr;
    D2Seam<&STATLIST_GetMaxDurabilityFromUnit>::hook = nullptr; D2Seam<&sub_6FC80A30>::hook = nullptr;
    fnTagLog(r, 4, 5);   // blobs u, a1, pd, sc, lg
}
