// D2Client sub_41690 (0x41690) and its callee 0x41000, real functions over the real callees.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/Path/Path.h"
#include "../d2moo/include/Units/Units.h"
#include "skill_gates_40450.h"

#define W __attribute__((weak))
extern "C" {
uint32_t Fog_Ordinal_10265(uint32_t);
void Fog_Ordinal_10024(const void*, const void*, uint32_t);
W uint8_t DAT_6fb8359f[1];
W uint32_t DAT_6fb86520[64], DAT_6fb86620[64];
W void Ordinal_10614(void);
W void Ordinal_10637(void* missile, int32_t level);
void* d2client_msl_create_b30f0(const uint32_t* req);          // D2Client+0xB30F0
void FUN_6fada0b0(const uint8_t* in_EAX, uint32_t unaff_EDI);  // D2Client+0x2A0B0

// 1.13c D2Client+0x41000 (unaff_EBX = skill id; param_5 unused by retail)
void FUN_6faf1000(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4,
                  uint32_t param_5, int32_t param_6, int32_t unaff_EBX) {
    (void)param_5;
    Ordinal_10614();
    uint32_t s[0x17]; memset(s, 0, sizeof s);
    s[0] = 3; s[1] = param_1; s[4] = param_2; s[5] = param_3; s[6] = param_4;
    for (int32_t i = 0; i < 0x100; i += param_6 * 4) {
        s[8] = *(const uint32_t*)((const uint8_t*)DAT_6fb86520 + i);
        s[7] = *(const uint32_t*)((const uint8_t*)DAT_6fb86620 + i);
        void* m = d2client_msl_create_b30f0(s);
        int32_t lvl = 0;
        if (unaff_EBX >= 0 && unaff_EBX < sgptDataTables->nSkillsTxtRecordCount)
            lvl = *(const int32_t*)(reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) + (uint32_t)unaff_EBX * 0x23c + 0x14c);
        Ordinal_10637(m, lvl + 1);
    }
}

// 1.13c D2Client+0x41690 (ecx = unit, edx = skill id, stack = param_3)
// Same shape as 0x415A0 with the ring spawner 0x41000 in place of 0x40F00.
int32_t d2_client_41690(D2UnitStrc* unit, int32_t idx, uint32_t param_3) {
    if (idx < 0 || idx >= sgptDataTables->nSkillsTxtRecordCount) return 0;
    const uint8_t* row = reinterpret_cast<const uint8_t*>(sgptDataTables->pSkillsTxt) + (uint32_t)idx * 0x23c;
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
    FUN_6faf1000(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(unit)), static_cast<uint32_t>(variant),
                 static_cast<uint32_t>(UNITS_GetXPosition(tgt)), static_cast<uint32_t>(UNITS_GetYPosition(tgt)),
                 param_3, len, idx);
    return 1;
}
}
