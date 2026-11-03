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
#include "d2c_seam.h"
extern "C" int32_t d2c_40550(const D2cSkillTbl*, const D2cSeam*, int32_t, const uint8_t*);
extern "C" uint32_t d2c_40450(const D2cSkillTbl*, const D2cSeam*, int32_t, const uint8_t*, uint32_t);

extern "C" void* d2client_msl_create_b30f0(const uint32_t* req);   // D2Client+0xB30F0

// The D2Common exports the three skill-gate callees resolve to (the ports in d2client_40450.cpp / d2client_40550.cpp take them as seam calls).
static uint64_t engSeam(void*, uint32_t rva, const uint64_t (&w)[14]) {
    switch (rva) {
    case 0xc1c0: return reinterpret_cast<uintptr_t>(STATLIST_GetStatListFromUnitAndState(reinterpret_cast<D2UnitStrc*>(w[5]), static_cast<int32_t>(w[6])));
    case 0xc4b4: return static_cast<uint32_t>(STATLIST_GetStatValue(reinterpret_cast<D2StatListStrc*>(w[5]), static_cast<int32_t>(w[6]), 0));
    case 0xc316: return static_cast<uint32_t>(SKILLS_EvaluateSkillFormula(reinterpret_cast<D2UnitStrc*>(w[5]), static_cast<uint32_t>(w[6]),
                                                                       static_cast<int32_t>(w[7]), static_cast<int32_t>(w[8])));
    }
    __builtin_trap();
}
static D2cSkillTbl engTbl() {
    return { reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt), sgptDataTables->nSkillsTxtRecordCount,
             sgptDataTables->nStatesTxtRecordCount, sgptDataTables->nItemStatCostTxtRecordCount, 0,
             reinterpret_cast<const uint8_t*>(gdwBitMasks)[8] };
}

// engine wrapper of d2client_40550.cpp (unit in param, skill id in EAX)
extern "C" int32_t d2_client_40550(D2UnitStrc* unit, int32_t id) {
    const D2cSkillTbl t = engTbl(); const D2cSeam s{ engSeam, nullptr };
    return d2c_40550(&t, &s, id, reinterpret_cast<const uint8_t*>(unit));
}

// engine wrapper of d2client_40450.cpp (skill index in EDI, unit in EBX, level on the stack; the calc word is read from the record)
extern "C" int32_t d2_client_40450(int32_t idx, D2UnitStrc* unit, int32_t level) {
    const D2cSkillTbl t = engTbl(); const D2cSeam s{ engSeam, nullptr };
    return static_cast<int32_t>(d2c_40450(&t, &s, idx, reinterpret_cast<const uint8_t*>(unit), static_cast<uint32_t>(level)));
}

// 1.13c D2Client+0x40DF0 (n in EAX, EDX = p2 at entry is the value #10614 receives; six stack args, ret 0x18).
// Request words (disasm, base esp+0x18): [0]=3 [1]=p1 [4]=p6 [5]=p2 [6]=p3 [7]=x [8]=y [0xb]=p4 [0xc]=p5.
extern "C" void d2_client_40df0(int32_t n, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5,
                                uint32_t p6) {
    D2SeedStrc seed;
    SEED_InitLowSeed(&seed, static_cast<int>(p2));
    uint32_t s[23] = {};
    s[0] = 3; s[1] = p1; s[5] = p2; s[6] = p3; s[4] = p6; s[11] = p4; s[12] = p5;
    for (; n > 0; --n) {
        uint64_t r; std::memcpy(&r, &seed, 8);
        uint64_t a = (r & 0xffffffff) * 0x6ac690c5u + (r >> 32);
        int32_t x = static_cast<int32_t>(static_cast<uint32_t>(a) % 0x28) - 0x14;
        r = (a & 0xffffffff) * 0x6ac690c5u + (a >> 32);
        int32_t y = static_cast<int32_t>(static_cast<uint32_t>(r) % 0x28) - 0x14;
        std::memcpy(&seed, &r, 8);
        if (x == 0 && y == 0) x = 0x14;
        s[7] = static_cast<uint32_t>(x); s[8] = static_cast<uint32_t>(y);
        D2UnitStrc* m = static_cast<D2UnitStrc*>(d2client_msl_create_b30f0(s));
        if (m) {
            MISSILE_SetTargetX(m, static_cast<int>(SEED_GetLowSeed(&seed)));
            MISSILE_SetTargetY(m, y * 0x10000 + static_cast<int>(static_cast<uint32_t>(x) & 0xffff));
        }
    }
}
