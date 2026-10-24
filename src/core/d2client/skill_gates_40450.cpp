// 1.13c D2Client skill-record gates 0x40450 / 0x40550 and the random-offset effect burst 0x40DF0,
// as real functions: callees are the D2Common exports the retail import slots resolve to
// (function list d2common.tsv: #10871 STATLIST_GetStatListFromUnitAndState, #10680 sub_39140,
// #10786 SKILLS_EvaluateSkillFormula, #10614 SEED_InitLowSeed, #10513 SEED_GetLowSeed,
// #10637/#10019 MISSILE_SetTargetX/Y) and D2Client+0xB30F0 (d2client_msl_create_b30f0).
// Registers the cached decompile drops are explicit trailing parameters.
#include <cstdint>
#include <cstring>
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/D2Seed.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/D2StatList.h"
#include "../d2moo/include/D2BitManip.h"
#include "../d2moo/include/Units/Missile.h"
#include "skill_gates_40450.h"

extern "C" int32_t sub_39140(int32_t list, int32_t statId);        // D2Common+0x39140 (#10680)
extern "C" void* d2client_msl_create_b30f0(const uint32_t* req);   // D2Client+0xB30F0

// Row of sgptDataTables+0xB98 (stride 0x23C, count +0xBA0), null when out of range.
static const uint8_t* skillRec(int32_t id) {
    if (id < 0 || id >= sgptDataTables->nSkillsTxtRecordCount) return nullptr;
    return reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) + static_cast<uint32_t>(id) * 0x23c;
}
static int16_t s16(const uint8_t* r, int o) { int16_t v; std::memcpy(&v, r + o, 2); return v; }
// The flagged branch's gate: state id (+0x80) in States.txt range, stat id (+0x54) in ItemStatCost range.
static bool gated(const uint8_t* r) {
    return (static_cast<uint8_t>(gdwBitMasks[8]) & r[4]) && s16(r, 0x80) >= 0 &&
           s16(r, 0x80) < sgptDataTables->nStatesTxtRecordCount && s16(r, 0x54) >= 0 &&
           s16(r, 0x54) < sgptDataTables->nItemStatCostTxtRecordCount;
}

// 1.13c D2Client+0x40550 (unit in param, skill id in EAX)
extern "C" int32_t d2_client_40550(D2UnitStrc* unit, int32_t id) {
    const uint8_t* r = skillRec(id);
    if (!r) return -1;
    if (gated(r)) {
        D2StatListStrc* l = STATLIST_GetStatListFromUnitAndState(unit, s16(r, 0x80));
        if (l) {
            int32_t lv = sub_39140(static_cast<int32_t>(reinterpret_cast<intptr_t>(l)), s16(r, 0x54));
            if (lv >= 2) { if (lv > 2) lv = 3; return s16(r, 0xe8 + lv * 2); }
        }
    }
    return s16(r, 0xea);
}

// 1.13c D2Client+0x40450 (skill index in EDI; unit/calc/level are the caller's pass-through for the tail call)
extern "C" int32_t d2_client_40450(int32_t idx, D2UnitStrc* unit, uint32_t calc, int32_t level) {
    const uint8_t* r = skillRec(idx);
    if (!r) return 0;
    if (gated(r)) {
        D2StatListStrc* l = STATLIST_GetStatListFromUnitAndState(unit, s16(r, 0x80));
        if (l) sub_39140(static_cast<int32_t>(reinterpret_cast<intptr_t>(l)), s16(r, 0x54));
    }
    return SKILLS_EvaluateSkillFormula(unit, calc, idx, level);  // every path ends in #10786
}

// 1.13c D2Client+0x40DF0 (n in EAX; seedLow = the value #10614 receives in a register)
extern "C" void d2_client_40df0(int32_t n, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5,
                                uint32_t p6, uint32_t seedLow) {
    D2SeedStrc seed;
    SEED_InitLowSeed(&seed, static_cast<int>(seedLow));
    uint32_t s[23] = {};
    s[0] = 3; s[1] = p1; s[5] = p2; s[6] = p3; s[4] = p6; s[15] = p4; s[16] = p5;
    for (; n > 0; --n) {
        uint64_t r; std::memcpy(&r, &seed, 8);
        uint64_t a = (r & 0xffffffff) * 0x6ac690c5u + (r >> 32);
        int32_t x = static_cast<int32_t>(static_cast<uint32_t>(a) % 0x28) - 0x14;
        r = (a & 0xffffffff) * 0x6ac690c5u + (a >> 32);
        int32_t y = static_cast<int32_t>(static_cast<uint32_t>(r) % 0x28) - 0x14;
        std::memcpy(&seed, &r, 8);
        if (x == 0 && y == 0) x = 0x14;
        s[11] = static_cast<uint32_t>(x); s[12] = static_cast<uint32_t>(y);
        D2UnitStrc* m = static_cast<D2UnitStrc*>(d2client_msl_create_b30f0(s));
        if (m) {
            MISSILE_SetTargetX(m, static_cast<int>(SEED_GetLowSeed(&seed)));
            MISSILE_SetTargetY(m, y * 0x10000 + static_cast<int>(static_cast<uint32_t>(x) & 0xffff));
        }
    }
}
