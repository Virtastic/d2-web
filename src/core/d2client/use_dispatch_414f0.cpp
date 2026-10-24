// D2Client skill-effect dispatchers 0x414F0 / 0x415A0, as real functions over the real callees
// (0x40550, 0x40450, 0x40DF0, 0x40F00, 0xA1340, 0x2A0B0, 0x1630/0x1660, D2Common #10392/#10786).
// Hidden registers the cached decompile drops are filled from the sibling callers.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/Path/Path.h"
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/D2Inventory.h"
#include "skill_gates_40450.h"

extern "C" {
uint32_t Fog_Ordinal_10265(uint32_t);                                  // string by id
void Fog_Ordinal_10024(const void*, const void*, uint32_t);                         // fatal-message log
__attribute__((weak)) uint8_t DAT_6fb8359f[1];                      // D2Client+0xB8359F format string
uint32_t FUN_6fb51340(int32_t* outH, const uint8_t* self, int32_t* outW);  // D2Client+0xA1340
void FUN_6fada0b0(const uint8_t* in_EAX, uint32_t unaff_EDI);             // D2Client+0x2A0B0
void FUN_6faf0f00(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t);  // D2Client+0x40F00
}

static const uint8_t* skillRow(int32_t idx) {
    if (idx < 0 || idx >= sgptDataTables->nSkillsTxtRecordCount) return nullptr;
    return reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) + static_cast<uint32_t>(idx) * 0x23c;
}

// 1.13c D2Client+0x414F0 (ecx = unit, edx = skill id, stack = param_3)
// 0x40450 is called with the same registers; its result is the burst count handed to 0x40DF0 in EAX.
// GUESS: 0x40450's calc is row+0x64 (as the 0x415A0 fallback), level = param_3; 0x40DF0's seedLow = n.
extern "C" int32_t d2_client_414f0(D2UnitStrc* unit, int32_t idx, uint32_t param_3) {
    const uint8_t* row = skillRow(idx);
    if (!row) return 0;
    int32_t variant = d2_client_40550(unit, idx);
    if (variant < 0) return 0;
    int32_t outW = 0, outH = 0;
    if (!FUN_6fb51340(&outH, reinterpret_cast<const uint8_t*>(unit), &outW)) return 0;
    uint32_t calc; std::memcpy(&calc, row + 0x64, 4);
    int32_t n = d2_client_40450(idx, unit, calc, static_cast<int32_t>(param_3));
    if (!n) return 0;
    d2_client_40df0(n, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(unit)), static_cast<uint32_t>(outW),
                    static_cast<uint32_t>(outH), static_cast<uint32_t>(idx), param_3,
                    static_cast<uint32_t>(variant), static_cast<uint32_t>(n));
    return 1;
}

// 1.13c D2Client+0x415A0 (ecx = unit, edx = skill id, stack = param_3)
// GUESS: the hidden 0x40F00 arg uStack_8 is the 0x40550 variant; 0x2A0B0's EDI is 0.
extern "C" int32_t d2_client_415a0(D2UnitStrc* unit, int32_t idx, uint32_t param_3) {
    const uint8_t* row = skillRow(idx);
    if (!row) return 0;
    int32_t variant = d2_client_40550(unit, idx);
    if (variant < 0) return 0;
    FUN_6fada0b0(reinterpret_cast<const uint8_t*>(unit), 0);
    if (!unit->pDynamicPath) {
        Fog_Ordinal_10024(DAT_6fb8359f, 0, Fog_Ordinal_10265(0x946));
        _exit(-1);
    }
    D2UnitStrc* tgt = PATH_GetTargetUnit(unit->pDynamicPath);   // #10392
    if (!tgt) return 0;
    uint32_t calc; std::memcpy(&calc, row + 0x64, 4);
    int32_t len = d2_client_40450(idx, unit, calc, static_cast<int32_t>(param_3));
    if (len < 1) len = SKILLS_EvaluateSkillFormula(unit, calc, idx, static_cast<int32_t>(param_3));  // #10786
    FUN_6faf0f00(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(unit)), static_cast<uint32_t>(variant),
                 static_cast<uint32_t>(UNITS_GetXPosition(tgt)), static_cast<uint32_t>(UNITS_GetYPosition(tgt)),
                 param_3, len, idx);
    return 1;
}
