// 1.13c D2Game+0x9F4A0 probe: the game's own D2GAME_SKILLMANA_ConsumeCharge_9F4A0 (D2Game/SKILLS/Skills.cpp).
// eax = game r[0], ecx = item id r[1], edx = player r[2], stack skill r[3]; sc r[5], log r[6], sgptDataTables slot r[7]. Blobs: n2 0, n1 1, game 2, u 3, dt 4, sc 5, lg 6.
// Every unit word is only handed to a callee, so the retail addresses go through as they are. Retail inlines the item lookup: the game's 128-slot id chain
// (+0x1720, node id +0xC, next +0xE4), read here from the game blob. nStuff/nShiftedStuff come from the retail data-table blob (+0xC6C/+0xC70).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"
#include "fn_tag.h"
#include "D2DataTbls.h"
#include "../d2moo/include/D2Game/SKILLS/Skills.h"
#include "../d2moo/include/D2Game/UNIT/SUnit.h"
#include "../d2moo/include/D2Game/GAME/SCmd.h"
#include "../d2moo/include/D2ItemMods.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/D2StatList.h"
#include "../d2moo/compat/test_seam.h"

static HookEnv* E;
static const uint32_t kFile = 0xB20FB980u;   // the source-file string retail pushes to 0xA49C, logged as its module offset
static uint32_t dw(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static uint32_t ra(const void* p) { return (uint32_t)(uintptr_t)p; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9f4a0(uint32_t* r)
{
    HookEnv he((uint32_t*)(uintptr_t)r[6], (const uint32_t*)(uintptr_t)r[5]); E = &he;
    const uint32_t dt = dw(dw(r[7]));
    D2DataTablesStrc tbl; std::memset(&tbl, 0, sizeof tbl);
    tbl.nStuff = (int)dw(dt + 0xC6C); tbl.nShiftedStuff = (int)dw(dt + 0xC70);
    D2Seam<&SUNIT_GetServerUnit>::hook = [](D2GameStrc* g, int32_t, int32_t id) -> D2UnitStrc* {
        uint32_t n = dw(ra(g) + 0x1720 + ((uint32_t)id & 0x7F) * 4);
        while (n && dw(n + 0xC) != (uint32_t)id) n = dw(n + 0xE4);
        return (D2UnitStrc*)(uintptr_t)n; };
    D2Seam<&SKILLS_GetSkillIdFromSkill>::hook = [](D2SkillStrc* s, const char*, int) -> int { return (int)E->h(0xA49C, 0, 0, 0, 0, 0, ra(s), kFile, 0x382); };
    D2Seam<&SKILLS_GetSkillLevel>::hook = [](D2UnitStrc* u, D2SkillStrc* s, BOOL b) -> int { return (int)E->h(0xA400, 0, 0, 0, 0, 0, ra(u), ra(s), (uint32_t)b); };
    D2Seam<&ITEMMODS_GetItemCharges>::hook = [](D2UnitStrc* it, int id, int lv, int* c, D2StatListStrc** l) -> BOOL { return (BOOL)E->h(0xAE20, 0, 0, 0, 0, 0, ra(it), (uint32_t)id, (uint32_t)lv, E->stk(c), E->stk(l)); };
    D2Seam<&STATLIST_GetStatValue>::hook = [](D2StatListStrc* l, int s, uint16_t ly) -> int { return (int)E->h(0xA712, 0, 0, 0, 0, 0, ra(l), (uint32_t)s, (uint32_t)ly); };
    D2Seam<&STATLIST_SetBaseStat2>::hook = [](D2StatListStrc* l, int s, int v, uint16_t ly, D2UnitStrc*) { E->h(0xAE86, 0, 0, 0, 0, 0, ra(l), (uint32_t)s, (uint32_t)v, (uint32_t)ly); };
    D2Seam<&SUNIT_GetClientFromPlayer>::hook = [](D2UnitStrc* u, const char*, int32_t) -> D2ClientStrc* { return (D2ClientStrc*)(uintptr_t)((u && dw(ra(u)) == 0) ? dw((uint32_t)E->h(0xA4C0, 0, 0, 0, 0, 0, ra(u)) + 0x9C) : 0); };
    D2Seam<&D2GAME_PACKETS_SendPacket0x3E_6FC3EC20>::hook = [](D2ClientStrc* c, D2UnitStrc* it, uint8_t a, int32_t s, uint32_t v, uint16_t ly) { E->h(0x8B010, ra(it), 0, 0, (uint32_t)ly, 0, ra(c), (uint32_t)a, (uint32_t)s, v); };
    D2DataTablesStrc* old = sgptDataTables; sgptDataTables = &tbl;
    r[4] = (uint32_t)D2GAME_SKILLMANA_ConsumeCharge_9F4A0((D2GameStrc*)(uintptr_t)r[0], (int32_t)r[1], (D2UnitStrc*)(uintptr_t)r[2], (D2SkillStrc*)(uintptr_t)r[3]);
    sgptDataTables = old;
    D2Seam<&SUNIT_GetServerUnit>::hook = nullptr; D2Seam<&SKILLS_GetSkillIdFromSkill>::hook = nullptr; D2Seam<&SKILLS_GetSkillLevel>::hook = nullptr;
    D2Seam<&ITEMMODS_GetItemCharges>::hook = nullptr; D2Seam<&STATLIST_GetStatValue>::hook = nullptr; D2Seam<&STATLIST_SetBaseStat2>::hook = nullptr;
    D2Seam<&SUNIT_GetClientFromPlayer>::hook = nullptr; D2Seam<&D2GAME_PACKETS_SendPacket0x3E_6FC3EC20>::hook = nullptr;
    fnTagLog(r, 6, 7);
}
