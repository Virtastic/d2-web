#pragma once

#include <cstdint>

struct D2UnitStrc;

namespace d2::realm {

// The Sounds row a joined client plays for a unit sound event the server sent (0x2C), when the
// event names a row through the unit's own tables (1.13c D2Client+0x23F20):
//   0x10, the taunt of a monster: MonSounds.Taunt of the monster's sound profile (the super
//         unique's, a unique's or minion's UMonSound, else the class's MonSound);
//   0x0C, a skill start: SkillsTxt.StSound of the skill the unit's skill stat list (flag 0x44)
//         names, when the unit holds that skill.
// `typeFlags` and `superUnique` are the monster's from its assign packet. 0 for every other event
// and when a lookup fails (every Sounds row the two columns name is positive).
uint32_t clientUnitSoundRow(D2UnitStrc* unit, uint32_t event, uint32_t typeFlags, int32_t superUnique);

// The Sounds row a joined client plays when it makes a monster (1.13c D2Client+0x23170): the monster's
// MonSounds.Init unless the monster is dead or dying, else Skill1 for a monster made in its Skill1 mode;
// 0 when there is none. The caller plays it once per unit.
uint32_t clientMonsterCreateSoundRow(D2UnitStrc* unit, uint32_t typeFlags, int32_t superUnique);

// The Sounds row a joined client plays when a state packet (0xA7, 0xA8, 0xAA) turns `state` on for a unit
// (1.13c D2Client+0x73760), read before the state is set: States.txt onsound when the unit does not carry
// the state yet; -1 for none, and for a notondead state on a dead monster or player (it is set silently).
int32_t clientStateOnSoundRow(D2UnitStrc* unit, int state);

// MonSounds Skill1 row 0xA84: 1.13c D2Client+0x22CA0 plays it only once a clock this client does not keep has
// run 25 ticks, so a joined client leaves it silent.
constexpr uint32_t kMonSoundSkill1Clocked = 0xA84;
inline bool clientMonsterSkill1SoundPlays(uint32_t row) { return row != 0 && row != kMonSoundSkill1Clocked; }

} // namespace d2::realm
