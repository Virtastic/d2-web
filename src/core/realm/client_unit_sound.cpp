#include "client_unit_sound.h"
#include "../monspawn/monster_sound_profile.h"
#include <D2DataTbls.h>
#include <D2Monsters.h>
#include <D2Skills.h>
#include <D2StatList.h>
#include <D2States.h>
#include <Units/Player.h>
#include <DataTbls/MonsterTbls.h>
#include <DataTbls/SkillsTbls.h>
#include <Units/Units.h>

namespace d2::realm {

// The MonSounds record of a monster's sound profile (1.13c D2Client+0x22A70).
static D2MonSoundsTxt* monster_sounds(D2UnitStrc* unit, uint32_t typeFlags, int32_t superUnique) {
    if (unit->dwUnitType != UNIT_MONSTER) return nullptr;
    D2MonStatsTxt* stats = DATATBLS_GetMonStatsTxtRecord(static_cast<int>(unit->dwClassId));
    if (!stats) return nullptr;
    const bool isSuper = (typeFlags & MONTYPEFLAG_SUPERUNIQUE) != 0;
    int32_t superProfile = -1;
    if (isSuper) {
        if (D2SuperUniquesTxt* super = DATATBLS_GetSuperUniquesTxtRecord(superUnique))
            superProfile = static_cast<int32_t>(super->dwMonSound);
    }
    const int32_t profile = d2::monster_sound::selectProfileId(isSuper,
        (typeFlags & (MONTYPEFLAG_UNIQUE | MONTYPEFLAG_MINION)) != 0, superProfile,
        static_cast<int32_t>(stats->wUMonSound), static_cast<int32_t>(stats->wMonSound));
    return profile >= 0 ? DATATBLS_GetMonSoundsTxtRecordFromSoundId(profile) : nullptr;
}

static uint32_t positive_row(uint32_t row) { return row > 0 && row <= INT32_MAX ? row : 0; }

// 1.13c D2Client+0x23F20, cases 0x10 and 0x0C.
uint32_t clientUnitSoundRow(D2UnitStrc* unit, uint32_t event, uint32_t typeFlags, int32_t superUnique) {
    if (!unit) return 0;
    if (event == 0x10) {
        D2MonSoundsTxt* sounds = monster_sounds(unit, typeFlags, superUnique);
        return sounds ? positive_row(sounds->dwTaunt) : 0;
    }
    if (event == 0x0C) {
        D2StatListStrc* list = STATLIST_GetStatListFromUnitAndFlag(unit, 0x44);
        if (!list) return 0;
        const int32_t skillId = STATLIST_GetStatValue(list, STAT_MODIFIERLIST_SKILL, 0);
        if (!SKILLS_GetHighestLevelSkillFromSkillId(unit, skillId)) return 0;
        D2SkillsTxt* txt = DATATBLS_GetSkillsTxtRecord(skillId);
        return txt && txt->wStSound > 0 ? txt->wStSound : 0;
    }
    return 0;
}

// 1.13c D2Client+0x23170, on a monster the client has just made. The neutral test (+0xA5440): a dead unit
// or one in its death or dead mode plays no Init. Without an Init row, a monster made in its Skill1 mode
// plays MonSounds Skill1 (+0x22CA0 slot 8); the 0xA84 row there waits on a clock this client does not keep.
uint32_t clientMonsterCreateSoundRow(D2UnitStrc* unit, uint32_t typeFlags, int32_t superUnique) {
    if (!unit) return 0;
    D2MonSoundsTxt* sounds = monster_sounds(unit, typeFlags, superUnique);
    if (!sounds) return 0;
    const bool neutral = (unit->dwFlags & UNITFLAG_ISDEAD) || unit->dwAnimMode == MONMODE_DEATH || unit->dwAnimMode == MONMODE_DEAD;
    if (sounds->dwInit && !neutral) return positive_row(sounds->dwInit);
    if (unit->dwAnimMode == MONMODE_SKILL1 && sounds->dwSkill[0] != kMonSoundSkill1Clocked) return positive_row(sounds->dwSkill[0]);
    return 0;
}

// 1.13c D2Client+0x73760: the notondead test reads bit 39 of the record's flags (byte +0x14, bit 7); the
// already-set test is D2Common's state check.
int32_t clientStateOnSoundRow(D2UnitStrc* unit, int state) {
    if (!unit || !sgptDataTables || state < 0 || state >= sgptDataTables->nStatesTxtRecordCount) return -1;
    D2StatesTxt* txt = DATATBLS_GetStatesTxtRecord(state);
    if (!txt) return -1;
    const bool dead = (unit->dwUnitType == UNIT_MONSTER && unit->dwAnimMode == MONMODE_DEAD) ||
        (unit->dwUnitType == UNIT_PLAYER && unit->dwAnimMode == PLRMODE_DEAD);
    if ((txt->nStateFlags[4] & 0x80) && dead) return -1;
    if (STATES_CheckState(unit, state)) return -1;
    const int16_t row = static_cast<int16_t>(txt->wOnSound);
    return row >= 0 ? row : -1;
}

} // namespace d2::realm
