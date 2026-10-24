// D2Client skill-effect dispatcher 0x41780 (scatter), a real function over the real callees; sibling of
// use_dispatch_414f0.cpp (0x415A0). Calls 0x40550, 0x40450, 0x2A0B0, 0xA1340, 0x1630/0x1660, 0x41100, #10392/#10786.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/Path/Path.h"
#include "../d2moo/include/Units/Units.h"
#include "skill_gates_40450.h"

extern "C" {
uint32_t Fog_Ordinal_10265(uint32_t);
void Fog_Ordinal_10024(const void*, const void*, uint32_t);
__attribute__((weak)) uint8_t DAT_6fb8359f[1];
uint32_t FUN_6fb51340(int32_t* outH, const uint8_t* self, int32_t* outW);  // D2Client+0xA1340
void FUN_6fada0b0(const uint8_t* in_EAX, uint32_t unaff_EDI);             // D2Client+0x2A0B0
void d2client_scatter_41100(void* unit, int32_t seedLow, int32_t cx, int32_t cy, int32_t r, int32_t a5, int32_t a6);
}

// 1.13c D2Client+0x41780 (ecx = unit, edx = skill id, stack = param_3)
// GUESS: the dropped 0x41100 registers are seed = 0x40550 variant, r = the 0x40450/#10786 count (as 0x415A0's len).
// When no target object exists, cx/cy come from 0xA1340 (outW, outH); else from the target's position.
extern "C" int32_t d2_client_41780(D2UnitStrc* unit, int32_t idx, uint32_t param_3) {
    if (idx < 0 || idx >= sgptDataTables->nSkillsTxtRecordCount) return 0;
    const uint8_t* row = reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) + static_cast<uint32_t>(idx) * 0x23c;
    int32_t variant = d2_client_40550(unit, idx);
    if (variant < 0) return 0;
    uint32_t calc; std::memcpy(&calc, row + 0x64, 4);
    int32_t n = d2_client_40450(idx, unit, calc, static_cast<int32_t>(param_3));
    if (n < 1) n = SKILLS_EvaluateSkillFormula(unit, calc, idx, static_cast<int32_t>(param_3));  // #10786
    FUN_6fada0b0(reinterpret_cast<const uint8_t*>(unit), 0);
    if (!unit->pDynamicPath) {
        Fog_Ordinal_10024(DAT_6fb8359f, 0, Fog_Ordinal_10265(0x946));
        _exit(-1);
    }
    int32_t cx, cy;
    D2UnitStrc* tgt = PATH_GetTargetUnit(unit->pDynamicPath);   // #10392
    if (!tgt) {
        if (!FUN_6fb51340(&cy, reinterpret_cast<const uint8_t*>(unit), &cx)) return 0;
    } else {
        cx = static_cast<int32_t>(UNITS_GetXPosition(tgt));
        cy = static_cast<int32_t>(UNITS_GetYPosition(tgt));
    }
    d2client_scatter_41100(unit, variant, cx, cy, n, idx, static_cast<int32_t>(param_3));
    return 1;
}
