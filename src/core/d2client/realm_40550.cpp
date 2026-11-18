// 1.13c D2Client+0x40550
// The client missile a skill fires: -1 for a skill id outside the table. A progressive skill (the Skills flag
// byte +4 ANDed with gdwBitMasks byte +8) whose aura state and aura stat are inside their tables, and whose unit
// holds that state's stat list, fires cltmissile[charges] where charges is the list's value of the aura stat
// (layer 0): below 2 it is cltmissilea, above 2 it is 3, and the row's word at +0xE8 + 2 * charges answers.
// Everything else answers cltmissilea.
#include <D2DataTbls.h>
#include <D2StatList.h>
#include <Units/Units.h>
#include "../d2moo/compat/test_seam.h"
#include "realm_skill_do.h"

// gdwBitMasks is the table of single bits; its byte +8 is the low byte of entry 2.
uint32_t d2client_realm_40550_progressMask() { return 4u; }

int d2client_realm_40550(D2UnitStrc* unit, int skillId) {
    const D2SkillsTxt* row = DATATBLS_GetSkillsTxtRecord(skillId);
    if (!row) return -1;
    const uint8_t flag = reinterpret_cast<const uint8_t*>(row)[4];
    if ((D2_CALL(d2client_realm_40550_progressMask) & flag) != 0) {
        const int state = row->nAuraState;
        const int stat = static_cast<int16_t>(row->wAuraStat[0]);
        if (state >= 0 && state < sgptDataTables->nStatesTxtRecordCount && stat >= 0 && stat < sgptDataTables->nItemStatCostTxtRecordCount) {
            D2StatListStrc* list = D2_CALL(STATLIST_GetStatListFromUnitAndState, unit, state);
            if (list) {
                int charges = D2_CALL(STATLIST_GetStatValue, list, stat, static_cast<uint16_t>(0));
                if (charges < 2) return static_cast<int16_t>(row->wCltMissileA);
                if (charges > 2) charges = 3;
                return static_cast<int16_t>((&row->wCltMissile)[charges]);
            }
        }
    }
    return static_cast<int16_t>(row->wCltMissileA);
}
