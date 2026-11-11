// Host test: tools/realm-tests.sh src/core/realm/client_unit_sound.test.cpp
// The Sounds row a joined client picks for a server sound event: a monster's taunt through its
// sound profile, a skill start through the skill its stat list names, nothing for other events.
#include "client_unit_sound.cpp"
#include <cassert>
#include <cstring>

alignas(8) static uint8_t g_tablesMem[sizeof(D2DataTablesStrc)];   // zeroed; the test fills the fields it reads
static D2DataTablesStrc& g_tables = *reinterpret_cast<D2DataTablesStrc*>(g_tablesMem);
D2DataTablesStrc* sgptDataTables = &g_tables;
static D2MonStatsTxt g_monStats[2];
static D2SuperUniquesTxt g_supers[1];
static D2MonSoundsTxt g_monSounds[8];
static D2SkillsTxt g_skills[3];
static D2StatListStrc g_list;
static D2SkillStrc g_skill;
static bool g_hasList, g_holdsSkill;
static int g_listSkill;
D2SuperUniquesTxt* __stdcall DATATBLS_GetSuperUniquesTxtRecord(int id) { return id == 0 ? &g_supers[0] : nullptr; }
D2MonSoundsTxt* __stdcall DATATBLS_GetMonSoundsTxtRecordFromSoundId(int id) { return id >= 0 && id < 8 ? &g_monSounds[id] : nullptr; }
D2StatListStrc* __stdcall STATLIST_GetStatListFromUnitAndFlag(D2UnitStrc*, int flag) { return flag == 0x44 && g_hasList ? &g_list : nullptr; }
int __stdcall STATLIST_GetStatValue(D2StatListStrc* list, int stat, uint16_t layer) {
    assert(list == &g_list && stat == STAT_MODIFIERLIST_SKILL && layer == 0);
    return g_listSkill;
}
D2SkillStrc* __fastcall SKILLS_GetHighestLevelSkillFromSkillId(D2UnitStrc*, int id) { return g_holdsSkill && id == g_listSkill ? &g_skill : nullptr; }
D2SkillsTxt* __fastcall DATATBLS_GetSkillsTxtRecord(int id) { return id >= 0 && id < 3 ? &g_skills[id] : nullptr; }

static D2StatesTxt g_states[3];
static bool g_carries;
D2StatesTxt* DATATBLS_GetStatesTxtRecord(int id) { return id >= 0 && id < 3 ? &g_states[id] : nullptr; }
BOOL __stdcall STATES_CheckState(D2UnitStrc*, int) { return g_carries; }

using d2::realm::clientUnitSoundRow;
using d2::realm::clientStateOnSoundRow;
using d2::realm::clientMonsterCreateSoundRow;

int main() {
    g_tables.pMonStatsTxt = g_monStats;
    g_tables.nMonStatsTxtRecordCount = 2;
    g_monStats[1].wMonSound = 2; g_monStats[1].wUMonSound = 3;
    g_monSounds[2].dwTaunt = 600;   // the class profile
    g_monSounds[3].dwTaunt = 700;   // the unique profile
    g_monSounds[5].dwTaunt = 800;   // the super unique's own profile
    g_supers[0].dwMonSound = 5;
    D2UnitStrc monster{};
    monster.dwUnitType = UNIT_MONSTER; monster.dwClassId = 1;

    // Taunt: the class profile, a unique's or minion's profile, the super unique's.
    assert(clientUnitSoundRow(&monster, 0x10, 0, -1) == 600);
    assert(clientUnitSoundRow(&monster, 0x10, MONTYPEFLAG_UNIQUE, -1) == 700);
    assert(clientUnitSoundRow(&monster, 0x10, MONTYPEFLAG_MINION, -1) == 700);
    assert(clientUnitSoundRow(&monster, 0x10, MONTYPEFLAG_CHAMPION, -1) == 600);
    assert(clientUnitSoundRow(&monster, 0x10, MONTYPEFLAG_SUPERUNIQUE, 0) == 800);
    // A super unique whose record names no profile falls through to the class's.
    g_supers[0].dwMonSound = 0;
    assert(clientUnitSoundRow(&monster, 0x10, MONTYPEFLAG_SUPERUNIQUE, 0) == 600);
    // A profile without a taunt, an unknown class and a player play no row.
    g_monSounds[2].dwTaunt = 0;
    assert(clientUnitSoundRow(&monster, 0x10, 0, -1) == 0);
    g_monSounds[2].dwTaunt = 600;
    monster.dwClassId = 9;
    assert(clientUnitSoundRow(&monster, 0x10, 0, -1) == 0);
    monster.dwClassId = 1;
    D2UnitStrc player{};
    player.dwUnitType = UNIT_PLAYER;
    assert(clientUnitSoundRow(&player, 0x10, 0, -1) == 0);

    // Skill start: the StSound of the skill the stat list names, when the unit holds it.
    g_skills[2].wStSound = 251;
    assert(clientUnitSoundRow(&player, 0x0C, 0, -1) == 0);           // no skill stat list
    g_hasList = true; g_listSkill = 2;
    assert(clientUnitSoundRow(&player, 0x0C, 0, -1) == 0);           // the skill is not held
    g_holdsSkill = true;
    assert(clientUnitSoundRow(&player, 0x0C, 0, -1) == 251);
    g_skills[2].wStSound = 0;
    assert(clientUnitSoundRow(&player, 0x0C, 0, -1) == 0);

    // Every other event resolves through the event table, not here.
    assert(clientUnitSoundRow(&monster, 0x0D, 0, -1) == 0);
    assert(clientUnitSoundRow(nullptr, 0x10, 0, -1) == 0);

    // A made monster: its profile's Init, not when it is dead or dying.
    g_monSounds[2].dwInit = 2574;
    g_monSounds[3].dwInit = 2575;
    monster.dwAnimMode = MONMODE_NEUTRAL;
    assert(clientMonsterCreateSoundRow(&monster, 0, -1) == 2574);
    assert(clientMonsterCreateSoundRow(&monster, MONTYPEFLAG_UNIQUE, -1) == 2575);
    monster.dwAnimMode = MONMODE_DEATH;
    assert(clientMonsterCreateSoundRow(&monster, 0, -1) == 0);
    monster.dwAnimMode = MONMODE_DEAD;
    assert(clientMonsterCreateSoundRow(&monster, 0, -1) == 0);
    monster.dwAnimMode = MONMODE_NEUTRAL; monster.dwFlags = UNITFLAG_ISDEAD;
    assert(clientMonsterCreateSoundRow(&monster, 0, -1) == 0);
    monster.dwFlags = 0;
    // Without Init: Skill1 for a monster made in its Skill1 mode, never the 0xA84 row, nothing otherwise.
    g_monSounds[2].dwInit = 0; g_monSounds[2].dwSkill[0] = 1077;
    assert(clientMonsterCreateSoundRow(&monster, 0, -1) == 0);
    monster.dwAnimMode = MONMODE_SKILL1;
    assert(clientMonsterCreateSoundRow(&monster, 0, -1) == 1077);
    g_monSounds[2].dwSkill[0] = 0xA84;
    assert(clientMonsterCreateSoundRow(&monster, 0, -1) == 0);
    // An Init row wins over Skill1; a player makes no monster sound.
    g_monSounds[2].dwInit = 2574; g_monSounds[2].dwSkill[0] = 1077;
    assert(clientMonsterCreateSoundRow(&monster, 0, -1) == 2574);
    assert(clientMonsterCreateSoundRow(&player, 0, -1) == 0);

    // A state turned on: its onsound (row 0 is a row), not when the unit already carries it, never a
    // negative row, and a notondead state on a dead unit is silent.
    g_tables.nStatesTxtRecordCount = 3;
    g_states[1].wOnSound = 0; g_states[2].wOnSound = 0xffff;
    g_states[0].wOnSound = 4020;
    monster.dwAnimMode = MONMODE_NEUTRAL;
    assert(clientStateOnSoundRow(&monster, 0) == 4020);
    assert(clientStateOnSoundRow(&monster, 1) == 0);
    assert(clientStateOnSoundRow(&monster, 2) == -1);
    assert(clientStateOnSoundRow(&monster, 3) == -1);
    g_carries = true;
    assert(clientStateOnSoundRow(&monster, 0) == -1);
    g_carries = false;
    g_states[0].nStateFlags[4] = 0x80;
    assert(clientStateOnSoundRow(&monster, 0) == 4020);
    monster.dwAnimMode = MONMODE_DEAD;
    assert(clientStateOnSoundRow(&monster, 0) == -1);
    player.dwAnimMode = PLRMODE_DEAD;
    assert(clientStateOnSoundRow(&player, 0) == -1);
    player.dwAnimMode = PLRMODE_NEUTRAL;
    assert(clientStateOnSoundRow(&player, 0) == 4020);
    g_states[0].nStateFlags[4] = 0;
    assert(clientStateOnSoundRow(&monster, 0) == 4020);
    return 0;
}
