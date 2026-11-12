// The play path of 1.13c D2Client+0x24490: the unit-mode sound dispatch with its callees, run on the facts the
// shell holds for a unit that just entered a mode. The rule is the proved ports (0x24490 dispatch, 0x23630 struck
// unit, 0x227E0 impact/body pair, 0x242B0 attack/weapon, 0x22A70 MonSounds profile, 0x22CA0 skill slot); this file
// only backs their host seams with the data tables and the session's facts and keeps the rows 0x88A70 accepts
// (a row > 0), each tagged with the callee that submitted it. The shell plays them.
//
// Seams and what backs them:
//   0x22430 type, 0x22490 class            the call's arguments
//   0x22A70 profile                        d2_client_22a70 over a unit with the call's class / type flags / superunique
//   0x1510(class, 0xD) MonStats2 CRITTER   MonStats(class).wMonStatsEx -> MonStats2 byte +4+(bit>>3), mask 1<<(bit&7)
//   0xC1FC(unit, 1) / (unit, 0x92)         the unit's states 1 and 146, flags bits 2 and 3
//   0x87BD0 busy                           flags bit 1 (a monster-range sound still playing: the sound engine's state)
//   0x222C0 class profile                  D2Client+0xF7694: per class {hit row, death row}
//   0x242B0 CVT (#10862)                   the hero's weapon hit-class index; table D2Client+0xF5538 (first dword of 14)
//   0x242B0 LCG roll (monster)             the roll the session already drew at the order's arrival (monster_face.cpp,
//                                          d2_client_attack_roll_take): replayed through the port's own gate and
//                                          draw, seed {lo 0, hi r} so the draw is r; -1 = no draw happened (gate shut)
//   0x220A0 unit +0x7C                     the session's per-unit sound tick (unit_sound_trigger_237e0.cpp)
// The session's own client-frame passes write the stamps (+0x7C, D2Client+0x11C4C4); the play path only reads them.
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <emscripten.h>
#include "client_host.h"
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/Units/Monster.h"
#include "../d2moo/include/D2DataTbls.h"

extern "C" void d2_client_24490(uint32_t uUnit, uint32_t nMode, uint32_t bUseGiven, D2cHost h);
extern "C" void d2_client_23630_rule(uint32_t uUnit, uint32_t dwHit, int32_t nMode, uint32_t uPlayerUnit, uint32_t uFrameTick,
                                     uint32_t* pSfxTicks, uint32_t* pOutFrameTick, D2cHost h);
extern "C" void d2_client_227e0(uint32_t uUnit, uint32_t uOut1, uint32_t dwLastHitClass, int32_t nMode, uint32_t uOut2, D2cHost h);
extern "C" uint32_t d2_client_22a70(const D2UnitStrc* pUnit, int32_t monStatsCount, uint32_t (*cb)(uint32_t op, uint32_t arg));
struct D2C242B0Env { uint32_t tick, lastTick, seedLo, seedHi; const uint32_t* cvtTable; int16_t animSpeed; uint32_t stamp; };
extern "C" void d2_client_242b0(D2C242B0Env* e, uint32_t unit, uint32_t flag, uint32_t mode, uintptr_t (*cb)(uint32_t, uintptr_t, uintptr_t, uintptr_t));
struct D2c22CA0Env {
    int32_t (*f22430)(); uint8_t* (*f22A70)(); int32_t (*f220A0)(uint32_t eax); uint32_t (*f38A70)(uint32_t eax);
    uint32_t (*ord10265)(uint32_t id); void (*ord10024)(uint32_t str); void (*exit_)(int32_t code);
    uint32_t g6fbc97f8; uint32_t* g6fbcc4c4;
};
extern "C" void d2c_22ca0(const D2c22CA0Env* e, uint32_t eax, uint32_t edi, uint8_t* mem);
extern "C" uint32_t d2_client_monster_sfx_ticks(uint32_t id);
extern "C" uint32_t d2_session_client_frame_now();

static_assert(offsetof(D2MonStatsTxt, wMonStatsEx) == 0x18, "0x1510 reads MonStats +0x18");

namespace {
// D2Client+0xF7694 -> per class {hit, death} Sounds rows (0x23630 reads word 0 for GH/KB, word 1 for DT).
const uint32_t kClassProfile[7][2] = { { 0xB3E, 0xB43 }, { 0xB5E, 0xB63 }, { 0xB4E, 0xB53 }, { 0xB46, 0xB4B },
                                       { 0xB56, 0xB5B }, { 0xB6E, 0xB73 }, { 0xB66, 0xB6B } };
// D2Client+0xF5538: 14 entries of 2 dwords, 0x242B0 reads the first (the weapon swing row by hit-class index).
const uint32_t kWeaponSwing[28] = { 0, 0, 251, 0, 262, 0, 268, 0, 274, 0, 280, 0, 286, 0, 292, 0, 268, 0, 298, 0 };

enum Kind : uint32_t { K_IMPACT = 1, K_HIT, K_DEATH, K_ATTACK, K_WEAPON, K_SKILL, K_SWING, K_BLOCK, K_KICK };
enum Flag : uint32_t { F_HERO = 1, F_BUSY = 2, F_STATE1 = 4, F_STATE146 = 8 };

struct Call { uint32_t type, id; int32_t cls; uint32_t typeFlags; int32_t superUnique; uint32_t hit, cvt, flags; int32_t roll; uint32_t mode; };
Call g;
D2UnitStrc g_unit; D2MonsterDataStrc g_md;
uint32_t g_rows[16], g_n = 0, g_kind = 0, g_sub = 0, g_subCalls = 0, g_hitMode = 0, g_skillMode = 0;
bool g_attackNext = false;
uint8_t g_scratch[0x100];

uint32_t unitAddr() { return (uint32_t)(uintptr_t)&g_unit; }
void keep(uint32_t row, uint32_t kind) { if (row > 0 && row < 0x10000 && g_n < 16) g_rows[g_n++] = row | (kind << 16); }

uint32_t profile22a70Cb(uint32_t op, uint32_t arg) {
    switch (op) {
    case 0: return (uint32_t)(uintptr_t)DATATBLS_GetSuperUniquesTxtRecord((int32_t)arg);
    case 1: return (uint32_t)(uintptr_t)DATATBLS_GetMonSoundsTxtRecordFromSoundId((int32_t)arg);
    case 2: return (uint32_t)(uintptr_t)DATATBLS_GetMonSoundsTxtRecordFromMonsterId((int32_t)arg);
    case 3: return (uint32_t)g.cls;
    case 4: return (uint32_t)(uintptr_t)DATATBLS_GetMonStatsTxtRecord((int32_t)arg);
    case 5: return (g.typeFlags & 0x10) ? 1u : 0u;   // 0x4C0D0: the minion bit
    }
    return 0;
}
uint32_t profile() {
    if (g.type != 1) return 0;
    return d2_client_22a70(&g_unit, sgptDataTables ? sgptDataTables->nMonStatsTxtRecordCount : 0, profile22a70Cb);
}
// 1.13c D2Client+0x1510: a MonStats2 flag bit of a monster class.
uint32_t monStats2Flag(int32_t cls, uint32_t bit) {
    const D2MonStatsTxt* ms = DATATBLS_GetMonStatsTxtRecord(cls);
    if (!ms || !sgptDataTables) return 0;
    const int32_t ex = (int16_t)ms->wMonStatsEx;
    if (ex < 0 || ex >= sgptDataTables->nMonStats2TxtRecordCount || !sgptDataTables->pMonStats2Txt) return 0;
    const uint8_t* r = reinterpret_cast<const uint8_t*>(&sgptDataTables->pMonStats2Txt[ex]);
    return (1u << (bit & 7)) & r[4 + (bit >> 3)];
}

uintptr_t cb242b0(uint32_t op, uintptr_t a, uintptr_t, uintptr_t) {
    switch (op) {
    case 0: return g.type;                 // TYPE
    case 1: return g.cvt;                  // CVT: #10862, the weapon's hit-class index
    case 5: return 0;                      // SPEED: only the submit's start offset
    case 6: {                              // SNDPLAY
        const uint32_t kind = g.type == 0 ? K_SWING : (g_attackNext ? K_ATTACK : K_WEAPON);
        g_attackNext = false; keep((uint32_t)a, kind); return a; }
    case 7: return profile();              // PROFILE
    case 8: return (g.flags & F_BUSY) ? 1 : 0;   // BUSY
    case 9: g_attackNext = true; return 0; // LIST: precedes the attack submit
    }
    return 0;                              // ERRSTR/ERRLOG/EXIT/STAMP/SETVAL: no row
}

uint32_t host(D2cCall* c) {
    switch (c->rva) {
    case 0x22430: return g.type;
    case 0x22490: return (uint32_t)g.cls;
    case 0x223D0: return g.mode;
    case 0x22A70: return profile();
    case 0x1510:  return monStats2Flag((int32_t)c->eax, c->edx);
    case 0xC1FC:  return c->s[1] == 1 ? ((g.flags & F_STATE1) ? 1u : 0u) : c->s[1] == 0x92 ? ((g.flags & F_STATE146) ? 1u : 0u) : 0u;
    case 0x87BD0: return (g.flags & F_BUSY) ? 1u : 0u;
    case 0x222C0: return c->eax < 7 ? (uint32_t)(uintptr_t)kClassProfile[c->eax] : 0u;
    case 0x88A70: {
        uint32_t kind = g_kind;
        if (g_sub == 0x23630) kind = g_subCalls++ < 2 ? K_IMPACT : g_hitMode ? K_HIT : K_DEATH;   // the first two are 0x227E0's pair
        keep(c->ebx, kind);
        return c->ebx;
    }
    case 0x23630: {
        const uint32_t m = c->s[0];
        g_sub = 0x23630; g_subCalls = 0; g_hitMode = (g.type == 1) ? (m == 3 || m == 0xD) : (m == 4 || m == 0x13);
        uint32_t sfx = 0, out = 0;
        d2_client_23630_rule(unitAddr(), g.hit, (int32_t)m, (g.flags & F_HERO) ? unitAddr() : 0, 0, &sfx, &out, host);
        g_sub = 0;
        return 0;
    }
    case 0x242B0: {
        D2C242B0Env e{};
        e.tick = 0x10000; e.cvtTable = kWeaponSwing; e.animSpeed = 256;
        if (g.type == 1 && g.roll >= 0) { e.lastTick = e.tick - 15; e.seedLo = 0; e.seedHi = (uint32_t)g.roll; }
        else e.lastTick = e.tick;          // no draw happened at the order: the 15-tick gate was shut
        const uint32_t saveFlags = g.flags;
        if (g.type == 1 && g.roll >= 0) g.flags &= ~F_BUSY;   // the draw happened, so the busy test passed then
        g_attackNext = false;
        d2_client_242b0(&e, unitAddr(), c->eax, c->s[0], cb242b0);
        g.flags = saveFlags;
        return 0;
    }
    case 0x22CA0: {
        D2c22CA0Env e{};
        e.f22430 = []() { return (int32_t)g.type; };
        e.f22A70 = []() { return (uint8_t*)(uintptr_t)profile(); };
        e.f220A0 = [](uint32_t) { return (int32_t)d2_client_monster_sfx_ticks(g.id); };
        // retail enters 0x88A70 with EBX = the slot 0x22CA0 just read (+0x44 + 4 * (mode - 8)); the port's seam carries
        // only EAX, so the row is read back from the same slot
        e.f38A70 = [](uint32_t) -> uint32_t {
            const uint32_t p = profile();
            keep(*(const uint32_t*)(uintptr_t)(p + 0x44 + 4 * (g_skillMode - 8)), K_SKILL);
            return 0x10; };
        e.ord10265 = [](uint32_t) { return 0u; }; e.ord10024 = [](uint32_t) {}; e.exit_ = [](int32_t) {};
        e.g6fbc97f8 = d2_session_client_frame_now();
        uint32_t last = 0; e.g6fbcc4c4 = &last;
        g_skillMode = c->edx;
        d2c_22ca0(&e, unitAddr(), c->edx, g_scratch);
        return 0;
    }
    case 0x22D60: {
        // 1.13c D2Client+0x22D60 -> 0x221C0: the block row from the hit class's low nibble
        const uint32_t n = g.hit & 0xF;
        keep(n >= 1 && n <= 9 ? 0x18E : (n == 10 || n == 11) ? 0x191 : 0, K_BLOCK);
        return 0;
    }
    }
    return 0;   // 0x87ED0 set value, 0x22DA0 / 0x22B20 sound removal: no row
}

void begin(uint32_t type, uint32_t id, int32_t cls, uint32_t typeFlags, int32_t superUnique, uint32_t hit, uint32_t cvt, uint32_t flags, int32_t roll, uint32_t mode) {
    g = { type, id, cls, typeFlags, superUnique, hit, cvt, flags, roll, mode };
    std::memset(&g_unit, 0, sizeof g_unit); std::memset(&g_md, 0, sizeof g_md);
    g_unit.dwUnitType = type; g_unit.dwClassId = cls; g_unit.dwLastHitClass = hit;
    if (type == 1) { g_md.nTypeFlag = (uint8_t)typeFlags; g_md.wBossHcIdx = (uint16_t)superUnique; g_unit.pMonsterData = &g_md; }
    g_n = 0; g_kind = 0; g_sub = 0; g_subCalls = 0; g_attackNext = false;
}
}

// One 0x24490 call for a unit entering `mode` (bUseGiven = 1). flags: 1 the client's own hero, 2 a monster sound
// still playing (0x87BD0), 4 state 1, 8 state 146. cvt: the hero's weapon hit-class index (#10862). roll: the value
// the session drew for this monster's attack entry, -1 when none. Returns the number of rows; read each with
// d2_client_unit_mode_sound_at(i) = row | kind << 16 (1 impact, 2 hit, 3 death, 4 attack, 5 weapon, 6 skill,
// 7 swing, 8 block, 9 kick), in 0x88A70's submit order.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_unit_mode_sounds(uint32_t type, uint32_t id, int32_t cls, uint32_t typeFlags,
                                                                    int32_t superUnique, uint32_t mode, uint32_t hit,
                                                                    uint32_t cvt, uint32_t flags, int32_t roll) {
    if (type > 1) return 0;
    begin(type, id, cls, typeFlags, superUnique, hit, cvt, flags, roll, mode);
    d2_client_24490(unitAddr(), mode, 1, host);
    if (type == 0 && mode == 0xC) { for (uint32_t i = 0; i < g_n; ++i) g_rows[i] = (g_rows[i] & 0xFFFF) | (K_KICK << 16); }
    return g_n;
}
// 0x227E0 alone (a hit landing on a unit whose mode does not change): the impact/body pair as 0x23630 submits it.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_unit_impact_sounds(uint32_t type, int32_t cls, uint32_t hit, uint32_t mode) {
    if (type > 1) return 0;
    begin(type, 0, cls, 0, -1, hit, 0, 0, -1, mode);
    int32_t a = 0, b = 0;
    d2_client_227e0(unitAddr(), (uint32_t)(uintptr_t)&a, hit, (int32_t)mode, (uint32_t)(uintptr_t)&b, host);
    keep((uint32_t)a, K_IMPACT); keep((uint32_t)b, K_IMPACT);
    return g_n;
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_unit_mode_sound_at(uint32_t i) { return i < g_n ? g_rows[i] : 0; }
