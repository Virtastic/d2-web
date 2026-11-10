// The pure rules of a realm game's skill start: the two server records, the monster action
// table, the hit overlay, the swing timing, and what a start sounds and draws.
#include "../src/core/realm/client_skill_fx.h"
#include "../src/core/realm/d2gs_client_pkt_route.h"
#include <cstdio>

using namespace d2::realm;

static int failures = 0;
#define CHECK(c) do { if (!(c)) { std::printf("FAIL line %d: %s\n", __LINE__, #c); failures++; } } while (0)

int main() {
    // A skill on a unit: player 0x4d uses skill 0x24 level 5 on monster 9.
    const uint8_t onUnit[16] = { 0x4c, 0, 0x4d, 0, 0, 0, 0x24, 0, 5, 1, 9, 0, 0, 0, 0, 0 };
    SkillStartRequest r;
    CHECK(decodeSkillStart(onUnit, 16, &r));
    CHECK(r.onUnit && r.unitType == 0 && r.guid == 0x4d && r.skill == 0x24 && r.level == 5);
    CHECK(r.targetType == 1 && r.targetGuid == 9);
    // A skill at a spot: monster 7 uses skill 0x31 level 3 at (10000, 20000).
    const uint8_t atSpot[17] = { 0x4d, 1, 7, 0, 0, 0, 0x31, 0, 0, 0, 3, 0x10, 0x27, 0x20, 0x4e, 0, 0 };
    CHECK(decodeSkillStart(atSpot, 17, &r));
    CHECK(!r.onUnit && r.unitType == 1 && r.guid == 7 && r.skill == 0x31 && r.level == 3);
    CHECK(r.x == 10000 && r.y == 20000);
    // A short record, a long one and another opcode are refused and leave the request alone.
    SkillStartRequest keep; keep.guid = 99;
    CHECK(!decodeSkillStart(onUnit, 15, &keep) && !decodeSkillStart(atSpot, 16, &keep));
    const uint8_t other[16] = { 0x4b };
    CHECK(!decodeSkillStart(other, 16, &keep) && keep.guid == 99);
    CHECK(!decodeSkillStart(nullptr, 16, &keep) && !decodeSkillStart(onUnit, 16, nullptr));

    // Monster actions: which arm runs and the mode it sets.
    CHECK(monsterAction(4) == MonsterAction::AttackAt && monsterAction(5) == MonsterAction::AttackOn);
    CHECK(monsterAction(0xc) == MonsterAction::AttackAt && monsterAction(0xd) == MonsterAction::AttackOn);
    CHECK(monsterAction(0x1c) == MonsterAction::AttackAt && monsterAction(0x1d) == MonsterAction::AttackOn);
    CHECK(monsterAction(6) == MonsterAction::GetHit && monsterAction(0x12) == MonsterAction::Unhandled);
    CHECK(monsterActionMode(4) == 7 && monsterActionMode(5) == 7);     // block
    CHECK(monsterActionMode(0xa) == 4 && monsterActionMode(0xb) == 4); // first attack
    CHECK(monsterActionMode(0xc) == 8 && monsterActionMode(0xe) == 9); // cast, first skill
    CHECK(monsterActionMode(0x1c) == 11 && monsterActionMode(0x15) == 14);
    CHECK(monsterActionMode(6) == 3 && monsterActionMode(30) == -1);

    // The hit overlay needs both nibbles of the hit class.
    CHECK(hitOverlayRow(0x11) == 0x36 && hitOverlayRow(0x21) == 0x51 && hitOverlayRow(0x3f) == 0x24);
    CHECK(hitOverlayRow(0x42) == 0x01 && hitOverlayRow(0x71) == 0x93 && hitOverlayRow(0x81) == 0x15);
    CHECK(hitOverlayRow(0x93) == 0x71);
    CHECK(hitOverlayRow(0x10) == -1 && hitOverlayRow(0x01) == -1 && hitOverlayRow(0) == -1);
    CHECK(hitOverlayRow(0x51) == -1 && hitOverlayRow(0x61) == -1 && hitOverlayRow(0xa1) == -1);

    // The swing: frames scaled by the unit's animation speed, rounded; speed 0 counts as 1.
    CHECK(weaponSwing(1).sound == 251 && weaponSwing(0).sound == 0 && weaponSwing(0x38).sound == 268);
    CHECK(swingDelayTicks(1, 256) == 6 && swingDelayTicks(4, 256) == 8);
    CHECK(swingDelayTicks(4, 128) == 17 && swingDelayTicks(1, 512) == 3);
    CHECK(swingDelayTicks(1, 0) == 1664 && swingDelayTicks(0, 256) == 0);

    // A melee skill: its start sound waits three ticks short of the swing, the swing sounds on it.
    SkillFxRow row; row.stSound = 10; row.stSoundDelay = true; row.weaponSound = true; row.castOverlay = 5;
    SkillFxUnit unit; unit.isPlayer = true; unit.classId = 3; unit.hitClass = 1;
    SkillFxPlan p = skillStartFx(row, unit, false, 100);
    CHECK(p.sounds == 2 && p.sound[0].kind == 1 && p.sound[0].row == 10 && p.sound[0].delay == 3);
    CHECK(p.sound[1].kind == 3 && p.sound[1].row == 251 && p.sound[1].delay == 6 && p.overlay == 5);
    // Bare hands: no swing sound, and the start sound's delay does not go below zero.
    unit.hitClass = 0;
    p = skillStartFx(row, unit, false, 100);
    CHECK(p.sounds == 1 && p.sound[0].row == 10 && p.sound[0].delay == 0);
    // A cast: at once; the class's own start sound only for a player of the skill's class.
    SkillFxRow cast; cast.stSound = 30; cast.stSoundClass = 31; cast.charClass = 3; cast.castOverlay = 7;
    p = skillStartFx(cast, unit, false, 100);
    CHECK(p.sounds == 2 && p.sound[0].row == 30 && p.sound[0].delay == 0 && p.sound[1].kind == 4 && p.sound[1].row == 31);
    unit.classId = 1;
    CHECK(skillStartFx(cast, unit, false, 100).sounds == 1);
    unit.classId = 3; unit.isPlayer = false;
    CHECK(skillStartFx(cast, unit, false, 100).sounds == 1);
    unit.isPlayer = true;
    // From an item: the row's item sound and overlay replace the skill's when set, and the class sound is dropped.
    cast.itemCastSound = 40; cast.itemCastOverlay = 9;
    p = skillStartFx(cast, unit, true, 100);
    CHECK(p.sounds == 1 && p.sound[0].row == 40 && p.overlay == 9);
    p = skillStartFx(cast, unit, false, 100);
    CHECK(p.sounds == 2 && p.sound[0].row == 30 && p.overlay == 7);
    cast.itemCastSound = 0; cast.itemCastOverlay = 0;
    p = skillStartFx(cast, unit, true, 100);
    CHECK(p.sounds == 2 && p.sound[0].row == 30 && p.overlay == 7);
    // No sound, and an overlay row outside the table: nothing.
    SkillFxRow none; none.castOverlay = 100;
    p = skillStartFx(none, unit, false, 100);
    CHECK(p.sounds == 0 && p.overlay == 0);
    none.castOverlay = 99;
    CHECK(skillStartFx(none, unit, false, 100).overlay == 99);

    CHECK(overlayEndTick(10, 4, 16) == 15 && overlayEndTick(10, 4, 8) == 19 && overlayEndTick(0, 0, 0) == 2);

    if (failures) return 1;
    std::printf("realm skill fx: ok\n");
    return 0;
}
