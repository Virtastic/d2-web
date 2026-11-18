// 1.13c D2Game+0x9F850 probe: the game's own D2GAME_SKILLMANA_Consume_6FD10A50 (D2Game/SKILLS/Skills.cpp).
// eax = skill id r[0], ecx = player r[1]; stack game r[2], level r[3]; sc r[5], log r[6], sgptDataTables slot r[7]. Blobs: rc 0, dt 1, dp 2, g 3, u 4, sc 5, lg 6.
// The skills row is read from the retail data table (slot -> dp -> dt, rows at +0xB98, count at +0xBA0, stride 0x23C: mana +0x18A, per level +0x18C,
// shift +0x188, minimum +0x186) into a native row; the unit words are only handed to callees; every callee is the recorder logging retail's call.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"
#include "fn_tag.h"
#include "D2DataTbls.h"
#include "../d2moo/include/D2Game/SKILLS/Skills.h"
#include "../d2moo/include/D2Game/SKILLS/SkillAss.h"
#include "../d2moo/include/D2Game/SKILLS/SkillDruid.h"
#include "../d2moo/include/D2Game/UNIT/SUnit.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/D2States.h"
#include "../d2moo/include/D2StatList.h"
#include "../d2moo/compat/test_seam.h"

static HookEnv* E;
static uint32_t g_dt;
static const uint32_t kFile = 0xB20FB980u;   // the source-file string retail pushes to 0xA49C, logged as its module offset
static uint32_t dw(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static uint16_t w16(uint32_t a) { return *(const uint16_t*)(uintptr_t)a; }
static uint32_t ra(const void* p) { return (uint32_t)(uintptr_t)p; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9f850(uint32_t* r)
{
    HookEnv he((uint32_t*)(uintptr_t)r[6], (const uint32_t*)(uintptr_t)r[5]); E = &he;
    g_dt = dw(dw(r[7]));
    D2Seam<&UNITS_GetUsedSkill>::hook = [](D2UnitStrc* u) -> D2SkillStrc* { return (D2SkillStrc*)(uintptr_t)E->h(0xAABA, 0, 0, 0, 0, 0, ra(u)); };
    D2Seam<&SKILLS_GetSkillIdFromSkill>::hook = [](D2SkillStrc* s, const char*, int) -> int { return (int)E->h(0xA49C, 0, 0, 0, 0, 0, ra(s), kFile, 0x3BB); };
    D2Seam<&SKILLS_GetOwnerGUIDFromSkill>::hook = [](D2SkillStrc* s) -> D2UnitGUID { return (D2UnitGUID)E->h(0xA4BA, 0, 0, 0, 0, 0, ra(s)); };
    D2Seam<&D2GAME_SKILLMANA_ConsumeCharge_9F4A0>::hook = [](D2GameStrc* g, int32_t guid, D2UnitStrc* u, D2SkillStrc* s) -> int32_t { return (int32_t)E->h(0x9F4A0, ra(g), (uint32_t)guid, ra(u), 0, 0, ra(s)); };
    D2Seam<&SKILLS_GetSkillsTxtRecord>::hook = [](int32_t id) -> D2SkillsTxt* {
        static D2SkillsTxt row;
        if (id < 0 || id >= (int32_t)dw(g_dt + 0xBA0)) return nullptr;
        const uint32_t rec = dw(g_dt + 0xB98) + (uint32_t)id * 0x23C;
        if (!rec) return nullptr;
        std::memset(&row, 0, sizeof row);
        row.wMinMana = w16(rec + 0x186); row.nManaShift = *(const uint8_t*)(uintptr_t)(rec + 0x188);
        row.wMana = (int16_t)w16(rec + 0x18A); row.wLevelMana = (int16_t)w16(rec + 0x18C);
        return &row; };
    D2Seam<&STATES_CheckState>::hook = [](D2UnitStrc* u, int st) -> BOOL { return (BOOL)E->h(0xA68E, 0, 0, 0, 0, 0, ra(u), (uint32_t)st); };
    D2Seam<&D2GAME_SKILLS_BloodMana_6FD025E0>::hook = [](D2UnitStrc*, int32_t cost) -> int32_t { return (int32_t)E->h(0xBE20, (uint32_t)cost); };
    D2Seam<&STATLIST_UnitGetStatValue>::hook = [](const D2UnitStrc* u, int s, uint16_t l) -> int32_t { return (int32_t)E->h(0xA42A, 0, 0, 0, 0, 0, ra(u), (uint32_t)s, (uint32_t)l); };
    D2Seam<&STATLIST_AddUnitStat>::hook = [](D2UnitStrc* u, int s, int v, uint16_t l) { E->h(0xA4D2, 0, 0, 0, 0, 0, ra(u), (uint32_t)s, (uint32_t)v, (uint32_t)l); };
    r[4] = (uint32_t)D2GAME_SKILLMANA_Consume_6FD10A50((D2GameStrc*)(uintptr_t)r[2], (D2UnitStrc*)(uintptr_t)r[1], (int32_t)r[0], (int32_t)r[3]);
    D2Seam<&UNITS_GetUsedSkill>::hook = nullptr; D2Seam<&SKILLS_GetSkillIdFromSkill>::hook = nullptr; D2Seam<&SKILLS_GetOwnerGUIDFromSkill>::hook = nullptr;
    D2Seam<&D2GAME_SKILLMANA_ConsumeCharge_9F4A0>::hook = nullptr; D2Seam<&SKILLS_GetSkillsTxtRecord>::hook = nullptr; D2Seam<&STATES_CheckState>::hook = nullptr;
    D2Seam<&D2GAME_SKILLS_BloodMana_6FD025E0>::hook = nullptr; D2Seam<&STATLIST_UnitGetStatValue>::hook = nullptr; D2Seam<&STATLIST_AddUnitStat>::hook = nullptr;
    fnTagLog(r, 6, 7);
}
