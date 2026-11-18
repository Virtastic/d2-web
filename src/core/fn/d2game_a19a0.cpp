// 1.13c D2Game+0xA19A0 probe: the game's own D2GAME_SKILLS_Handler_6FD12BA0 (D2Game/SKILLS/Skills.cpp).
// eax = unit r[0]; stack game r[1], id r[2], level r[3], a5 r[5], a6 r[6], a7 r[7]; sc r[8], log r[9], sgptDataTables slot r[10], word tables r[11].
// Blobs: tt 0, tg 1, rc 2, dt 3, g 4, u 5, sc 6, lg 7. The skills row is the retail row read field by field (+0x2C start, +0x2E do, +0x46 missile, +0x16A item effect,
// +0x190 delay, +0x10 anim, flags +4/+8); the do and start words come from the word tables (+0x170 / +0x0); every callee is the recorder logging retail's call.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"
#include "fn_tag.h"
#include "mirror_layouts.h"
#include "../d2moo/include/D2Game/SKILLS/Skills.h"
#include "../d2moo/include/D2Game/SKILLS/SkillAss.h"
#include "../d2moo/include/D2Game/UNIT/SUnit.h"
#include "../d2moo/include/D2Game/GAME/Clients.h"
#include "../d2moo/include/D2Game/PLAYER/PlrModes.h"
#include "../d2moo/include/D2Game/MISSILES/Missiles.h"
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/D2Dungeon.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/Path/Path.h"
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/compat/test_seam.h"

static HookEnv* E; static mirror::Mirror* M; static uint32_t g_dt, g_tt, g_self;
static const uint32_t kFile = 0xB20FB980u;   // the source-file string retail pushes to 0xA49C, logged as its module offset
static uint32_t dw(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static int32_t s16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
static uint32_t ra(const void* p) { return (uint32_t)(uintptr_t)p; }
static uint32_t gm(const void* p) { return (uint32_t)(uintptr_t)p; }
static int32_t __fastcall doStub(D2GameStrc* g, D2UnitStrc* u, int32_t id, int32_t lvl) { return (int32_t)E->h(0x78710, 0, gm(g), M->ra(u), 0, 0, (uint32_t)id, (uint32_t)lvl); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_a19a0(uint32_t* r)
{
    HookEnv he((uint32_t*)(uintptr_t)r[9], (const uint32_t*)(uintptr_t)r[8]); E = &he;
    mirror::Mirror m(mirror::types, mirror::ntypes); M = &m;
    g_dt = dw(dw(r[10])); g_tt = r[11];
    if (r[0]) m.allow(r[0], fnBlobLen(r, 5));
    auto pUnit = m.get<D2UnitStrc>(r[0]); g_self = r[0];
    D2Seam<&SKILLS_GetSkillsTxtRecord>::hook = [](int32_t id) -> D2SkillsTxt* {
        static D2SkillsTxt row;
        if (id < 0 || id >= (int32_t)dw(g_dt + 0xBA0)) return nullptr;
        const uint32_t rec = dw(g_dt + 0xB98) + (uint32_t)id * 0x23C;
        if (!rec) return nullptr;
        std::memset(&row, 0, sizeof row);
        row.dwFlags[0] = dw(rec + 4); row.dwFlags[1] = dw(rec + 8);
        row.wSrvStartFunc = (int16_t)s16(rec + 0x2C); row.wSrvDoFunc = (int16_t)s16(rec + 0x2E);
        row.wSrvMissile = (int16_t)s16(rec + 0x46); row.wItemEffect = (int16_t)s16(rec + 0x16A);
        row.dwDelay = dw(rec + 0x190); row.nAnim = *(const uint8_t*)(uintptr_t)(rec + 0x10);
        return &row; };
    D2Seam<&D2GAME_SKILLS_GetSrvDoFunc_6FD12BA0>::hook = [](int32_t n) -> SkillDoFunc { return dw(g_tt + 0x170 + (uint32_t)n * 4) ? &doStub : nullptr; };
    D2Seam<&D2GAME_SKILLS_GetSrvStartFunc_6FD12BA0>::hook = [](int32_t n) -> SkillStartFunc { return dw(g_tt + (uint32_t)n * 4) ? &doStub : nullptr; };
    D2Seam<&UNITS_GetUsedSkill>::hook = [](D2UnitStrc* u) -> D2SkillStrc* { return (D2SkillStrc*)(uintptr_t)E->h(0xAABA, 0, 0, 0, 0, 0, M->ra(u)); };
    D2Seam<&SKILLS_GetFlags>::hook = [](D2SkillStrc* s) -> int { return (int)E->h(0xA79C, 0, 0, 0, 0, 0, ra(s)); };
    D2Seam<&UNITS_GetRoom>::hook = [](D2UnitStrc* u) -> D2ActiveRoomStrc* { return (D2ActiveRoomStrc*)(uintptr_t)E->h(0xA478, 0, 0, 0, 0, 0, M->ra(u)); };
    D2Seam<&DUNGEON_IsRoomInTown>::hook = [](D2ActiveRoomStrc* rm) -> BOOL { return (BOOL)E->h(0xA808, 0, 0, 0, 0, 0, ra(rm)); };
    D2Seam<&UNITS_SetUsedSkill>::hook = [](D2UnitStrc* u, D2SkillStrc* s) { E->h(0xA496, 0, 0, 0, 0, 0, M->ra(u), ra(s)); };
    D2Seam<&sub_6FC817D0>::hook = [](D2GameStrc* g, D2UnitStrc* u, D2SkillStrc* s, int32_t mode, int32_t x, int32_t y, int32_t a7) { E->h(0x78550, 0, gm(g), M->ra(u), 0, 0, ra(s), (uint32_t)mode, (uint32_t)x, (uint32_t)y, (uint32_t)a7); };
    D2Seam<&SKILLS_GetSkillIdFromSkill>::hook = [](D2SkillStrc* s, const char*, int) -> int { return (int)E->h(0xA49C, 0, 0, 0, 0, 0, ra(s), kFile, 0xB47); };
    D2Seam<&SKILLS_GetSkillLevel>::hook = [](D2UnitStrc* u, D2SkillStrc* s, BOOL b) -> int { return (int)E->h(0xA400, 0, 0, 0, 0, 0, M->ra(u), ra(s), (uint32_t)b); };
    D2Seam<&SKILLS_GetHighestLevelSkillFromUnitAndId>::hook = [](D2UnitStrc* u, int id) -> D2SkillStrc* { return (D2SkillStrc*)(uintptr_t)E->h(0xA3F4, 0, M->ra(u), (uint32_t)id); };
    D2Seam<&sub_6FD10CE0>::hook = [](D2UnitStrc* u) -> int32_t { return (int32_t)E->h(0x9EC80, 0, 0, 0, M->ra(u)); };
    D2Seam<&DATATBLS_GetMissilesTxtRecord>::hook = [](int id) -> D2MissilesTxt* { return (D2MissilesTxt*)(uintptr_t)E->h(0x1250, (uint32_t)id); };
    // 9FA30 runs as the game's own body; its callees are the recorder, as in the 9FA30 probe
    D2Seam<&sub_6FCBC7E0>::hook = [](D2GameStrc* g, D2UnitStrc* u) -> void { E->h(0xE07A0, M->ra(u), gm(g)); };
    D2Seam<&PATH_GetTargetUnit>::hook = [](D2DynamicPathStrc* p) -> D2UnitStrc* { const uint32_t t = (uint32_t)E->h(0xA598, 0, 0, 0, 0, 0, M->ra(p)); return t == g_self ? M->get<D2UnitStrc>(t) : (D2UnitStrc*)(uintptr_t)t; };   // the unit itself is the mirror copy, so the target-is-self test sees one pointer
    D2Seam<&D2COMMON_10175_PathGetFirstPointX>::hook = [](D2DynamicPathStrc* p) -> int { return (int)E->h(0xAAC6, 0, 0, 0, 0, 0, M->ra(p)); };
    D2Seam<&D2COMMON_10176_PathGetFirstPointY>::hook = [](D2DynamicPathStrc* p) -> int { return (int)E->h(0xAAD8, 0, 0, 0, 0, 0, M->ra(p)); };
    D2Seam<&CLIENTS_GetUnitX>::hook = [](D2UnitStrc* u) -> int32_t { const uint32_t o = M->ra(u), ut = dw(o), p = dw(o + 0x2C); return (ut == 2 || ut == 4 || ut == 5) ? (int32_t)dw(p + 0xC) : p ? (int32_t)E->h(0xA454, 0, 0, 0, 0, 0, p) : 0; };
    D2Seam<&CLIENTS_GetUnitY>::hook = [](D2UnitStrc* u) -> int32_t { const uint32_t o = M->ra(u), ut = dw(o), p = dw(o + 0x2C); return (ut == 2 || ut == 4 || ut == 5) ? (int32_t)dw(p + 0x10) : p ? (int32_t)E->h(0xA3B8, 0, 0, 0, 0, 0, p) : 0; };
    // retail ignores both results here (the web build's client-sound call on a failed missile is not part of the retail body), so the stand-ins report success
    D2Seam<&sub_6FD11710>::hook = [](D2GameStrc*, int32_t, D2UnitStrc* u, int32_t, int32_t, int32_t dx, int32_t dy, int32_t tx, int32_t ty, int32_t chk) -> D2UnitStrc* {
        E->h(0xA1880, (uint32_t)tx, 0, 0, 0, M->ra(u), (uint32_t)dx, (uint32_t)dy, (uint32_t)ty, (uint32_t)chk); return (D2UnitStrc*)(uintptr_t)1; };
    D2Seam<&sub_6FD11420>::hook = [](D2GameStrc* g, int32_t mi, D2UnitStrc*, int32_t, int32_t, int32_t dx, int32_t dy, int32_t tx, int32_t ty, int32_t) -> D2UnitStrc* {
        E->h(0xA28F0, 0, gm(g), (uint32_t)mi, 0, 0, (uint32_t)dx, (uint32_t)dy, (uint32_t)tx, (uint32_t)ty); return (D2UnitStrc*)(uintptr_t)1; };
    D2Seam<&D2GAME_SKILLMANA_Consume_6FD10A50>::hook = [](D2GameStrc* g, D2UnitStrc* u, int32_t id, int32_t lv) -> int32_t { return (int32_t)E->h(0x9F850, (uint32_t)id, M->ra(u), 0, 0, 0, gm(g), (uint32_t)lv); };
    D2Seam<&sub_6FD118C0>::hook = [](D2GameStrc* g, D2UnitStrc* u) -> int32_t { return (int32_t)E->h(0xA1790, 0, 0, 0, M->ra(u), 0, gm(g)); };
    D2Seam<&SKILLS_EvaluateSkillFormula>::hook = [](D2UnitStrc* u, unsigned int c, int id, int lv) -> int { return (int)E->h(0xAC9A, 0, 0, 0, 0, 0, M->ra(u), c, (uint32_t)id, (uint32_t)lv); };
    D2Seam<&D2GAME_SKILLS_SetDelay_6FD11C00>::hook = [](D2GameStrc* g, D2UnitStrc* u, int32_t d) { (void)u; E->h(0x9FF70, (uint32_t)d, 0, 0, 0, 0, gm(g)); };
    r[4] = (uint32_t)D2GAME_SKILLS_Handler_6FD12BA0((D2GameStrc*)(uintptr_t)r[1], pUnit, (int32_t)r[2], (int32_t)r[3], (int32_t)r[5], (int32_t)r[6], (int32_t)r[7]);
    D2Seam<&SKILLS_GetSkillsTxtRecord>::hook = nullptr; D2Seam<&D2GAME_SKILLS_GetSrvDoFunc_6FD12BA0>::hook = nullptr; D2Seam<&D2GAME_SKILLS_GetSrvStartFunc_6FD12BA0>::hook = nullptr;
    D2Seam<&UNITS_GetUsedSkill>::hook = nullptr; D2Seam<&SKILLS_GetFlags>::hook = nullptr; D2Seam<&UNITS_GetRoom>::hook = nullptr; D2Seam<&DUNGEON_IsRoomInTown>::hook = nullptr;
    D2Seam<&UNITS_SetUsedSkill>::hook = nullptr; D2Seam<&sub_6FC817D0>::hook = nullptr; D2Seam<&SKILLS_GetSkillIdFromSkill>::hook = nullptr; D2Seam<&SKILLS_GetSkillLevel>::hook = nullptr;
    D2Seam<&SKILLS_GetHighestLevelSkillFromUnitAndId>::hook = nullptr; D2Seam<&sub_6FD10CE0>::hook = nullptr; D2Seam<&DATATBLS_GetMissilesTxtRecord>::hook = nullptr;
    D2Seam<&sub_6FCBC7E0>::hook = nullptr; D2Seam<&PATH_GetTargetUnit>::hook = nullptr; D2Seam<&D2COMMON_10175_PathGetFirstPointX>::hook = nullptr; D2Seam<&D2COMMON_10176_PathGetFirstPointY>::hook = nullptr;
    D2Seam<&CLIENTS_GetUnitX>::hook = nullptr; D2Seam<&CLIENTS_GetUnitY>::hook = nullptr; D2Seam<&sub_6FD11710>::hook = nullptr; D2Seam<&sub_6FD11420>::hook = nullptr;
    D2Seam<&D2GAME_SKILLMANA_Consume_6FD10A50>::hook = nullptr; D2Seam<&sub_6FD118C0>::hook = nullptr; D2Seam<&SKILLS_EvaluateSkillFormula>::hook = nullptr; D2Seam<&D2GAME_SKILLS_SetDelay_6FD11C00>::hook = nullptr;
    m.commit();
    fnTagLog(r, 7, 8);
}
