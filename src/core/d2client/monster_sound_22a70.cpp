// 1.13c D2Client+0x22A70: the MonSounds profile id for a unit.
// EAX = pUnit, plain ret. Class from 0x22490 (index in ESI); a class >= the signed MonStats record
// count is out of range. For a monster, a SuperUniques profile (0x02) is accepted only when positive,
// then UMonSound for type flag 0x08 or the minion bit 0x10 (0x4C0D0), again only when positive;
// otherwise ordinal 10917 with the class index. Profile 0 is a valid fallback.
//
// Decision logic only; every callee runs through cb(op, arg), in entry order:
//   0 = 0xC604 D2Common!#10038 DATATBLS_GetSuperUniquesTxtRecord(arg = (int16)wBossHcIdx)
//   1 = 0xCABA D2Common!#10876 DATATBLS_GetMonSoundsTxtRecordFromSoundId(arg)
//   2 = 0xCF76 D2Common!#10917 DATATBLS_GetMonSoundsTxtRecordFromMonsterId(arg = class)
//   3 = 0x22490 unit class (arg = pUnit, entered with it in ESI)
//   4 = 0x1230 pMonStatsTxt record by index (arg = class, entered in EAX)
//   5 = 0x4C0D0 pData bit 4 (arg = pUnit, entered with it in ECX)
// ops 0 and 1 return the SuperUniques/MonSounds record or 0; ops 2/3 return scalars.
#include <cstdint>
#include <cstddef>
#include "../d2moo/include/Units/Units.h"          // D2UnitStrc
#include "../d2moo/include/Units/Monster.h"        // D2MonsterDataStrc
#include "../d2moo/include/DataTbls/MonsterTbls.h" // D2SuperUniquesTxt, D2MonStatsTxt

static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00, "D2UnitStrc dwUnitType");
static_assert(offsetof(D2UnitStrc, pMonsterData) == 0x14 + 1 * (sizeof(void*) - 4), "D2UnitStrc pMonsterData");
static_assert(offsetof(D2MonsterDataStrc, nTypeFlag) == 0x16 + 1 * (sizeof(void*) - 4), "D2MonsterDataStrc nTypeFlag");
static_assert(offsetof(D2MonsterDataStrc, wBossHcIdx) == 0x26 + 1 * (sizeof(void*) - 4), "D2MonsterDataStrc wBossHcIdx");
static_assert(offsetof(D2SuperUniquesTxt, dwMonSound) == 0x18, "D2SuperUniquesTxt dwMonSound");
static_assert(offsetof(D2MonStatsTxt, wUMonSound) == 0x16, "D2MonStatsTxt wUMonSound");

enum : uint32_t {
    D2C22A70_SUPERUNIQUE = 0, D2C22A70_SOUND_FROM_ID = 1, D2C22A70_SOUND_FROM_MONSTER = 2,
    D2C22A70_UNIT_CLASS = 3, D2C22A70_MONSTATS = 4, D2C22A70_PDATA_BIT4 = 5,
};

// monStatsCount is the signed dword at sgptDataTables+0xA80 (the caller reads the global; D2Client+0x1690).
extern "C" uint32_t d2_client_22a70(const D2UnitStrc* pUnit, int32_t monStatsCount,
                                    uint32_t (*cb)(uint32_t op, uint32_t arg))
{
    if (!pUnit) return cb(D2C22A70_SOUND_FROM_ID, 0);
    const uint32_t u = (uint32_t)(uintptr_t)pUnit;
    const int32_t nClass = (int32_t)cb(D2C22A70_UNIT_CLASS, u);
    if (nClass >= monStatsCount) return cb(D2C22A70_SOUND_FROM_ID, 0);
    if (pUnit->dwUnitType == 1) {
        const D2MonsterDataStrc* md = pUnit->pMonsterData;
        if (md && (md->nTypeFlag & 0x02)) {
            const uint32_t su = cb(D2C22A70_SUPERUNIQUE, (uint32_t)(int32_t)(int16_t)md->wBossHcIdx);
            if (su) {
                const int32_t id = (int32_t)((const D2SuperUniquesTxt*)(uintptr_t)su)->dwMonSound;
                if (id > 0) return cb(D2C22A70_SOUND_FROM_ID, (uint32_t)id);
            }
        }
        md = pUnit->pMonsterData;   // retail re-reads both fields before the second test
        if ((pUnit->dwUnitType == 1 && md && (md->nTypeFlag & 0x08)) || cb(D2C22A70_PDATA_BIT4, u)) {
            const uint32_t ms = cb(D2C22A70_MONSTATS, (uint32_t)nClass);
            if (!ms) return cb(D2C22A70_SOUND_FROM_ID, 0);
            const int32_t id = (int32_t)(int16_t)((const D2MonStatsTxt*)(uintptr_t)ms)->wUMonSound;
            if (id > 0) return cb(D2C22A70_SOUND_FROM_ID, (uint32_t)id);
        }
    }
    return cb(D2C22A70_SOUND_FROM_MONSTER, (uint32_t)nClass);
}
