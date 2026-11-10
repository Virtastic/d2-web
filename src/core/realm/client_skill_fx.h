#pragma once
// The pure decisions of the client's skill start (1.13c D2Client): the two server records that
// start a skill on a unit, the monster action table, and which sounds and overlay a start plays.
// No engine types here, so the rules are tested natively; the state owner applies them to units.
#include <cstdint>

namespace d2::realm {

// A skill start as the client's handlers hand it to the action dispatch.
struct SkillStartRequest {
    uint8_t unitType = 0;
    uint32_t guid = 0;
    uint32_t skill = 0;
    uint8_t level = 0;
    bool onUnit = false;      // 0x4C: on a unit (client action 0x16); 0x4D: at a spot (0x15)
    uint8_t targetType = 0;
    uint32_t targetGuid = 0;
    uint16_t x = 0, y = 0;
};

inline uint32_t skillFxU32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
        (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

// 0x4C (16 bytes): type +1, guid +2, skill word +6, level +8, target type +9, target guid +10.
// 0x4D (17 bytes): type +1, guid +2, skill dword +6, level +10, x word +11, y word +13.
inline bool decodeSkillStart(const uint8_t* p, int length, SkillStartRequest* out) {
    if (!p || !out) return false;
    SkillStartRequest r;
    if (p[0] == 0x4c && length == 16) {
        r.onUnit = true;
        r.skill = static_cast<uint32_t>(p[6]) | (static_cast<uint32_t>(p[7]) << 8);
        r.level = p[8];
        r.targetType = p[9];
        r.targetGuid = skillFxU32(p + 10);
    } else if (p[0] == 0x4d && length == 17) {
        r.skill = skillFxU32(p + 6);
        r.level = p[10];
        r.x = static_cast<uint16_t>(p[11] | (p[12] << 8));
        r.y = static_cast<uint16_t>(p[13] | (p[14] << 8));
    } else {
        return false;
    }
    r.unitType = p[1];
    r.guid = skillFxU32(p + 2);
    *out = r;
    return true;
}

// The monster branch of the action dispatch maps an action code to the mode it sets.
inline int monsterActionMode(uint32_t code) {
    static const int8_t modes[30] = { 2, 2, 16, 16, 7, 7, 3, 1, 0, 12, 4, 4, 8, 8, 9, 9,
        5, 5, 6, 16, 13, 14, 14, 15, 15, 8, 10, 10, 11, 11 };
    return code < 30 ? modes[code] : -1;
}

// The hit class's high nibble picks the Overlay row a struck unit plays once; the low nibble
// must be set too. -1: no overlay.
inline int hitOverlayRow(uint32_t hitClass) {
    if (!(hitClass & 0xf0u) || !(hitClass & 0x0fu)) return -1;
    switch (hitClass & 0xf0u) {
    case 0x10: return 0x36;
    case 0x20: return 0x51;
    case 0x30: return 0x24;
    case 0x40: return 0x01;
    case 0x70: return 0x93;
    case 0x80: return 0x15;
    case 0x90: return 0x71;
    default: return -1;
    }
}

// Per weapon hit class (low nibble): the swing's sound row and its delay in animation frames.
struct WeaponSwing { uint16_t sound; uint16_t delay; };
inline WeaponSwing weaponSwing(uint32_t hitClass) {
    static const WeaponSwing table[16] = { {0, 0}, {251, 6}, {262, 6}, {268, 6}, {274, 8}, {280, 8},
        {286, 6}, {292, 8}, {268, 6}, {298, 6}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} };
    return table[hitClass & 0xfu];
}

// The swing delay in game ticks at the unit's animation speed (256 = one frame per tick).
inline int32_t swingDelayTicks(uint32_t hitClass, uint32_t animSpeed) {
    if (!animSpeed) animSpeed = 1;
    return static_cast<int32_t>((static_cast<uint32_t>(weaponSwing(hitClass).delay) * 0x100u + 0x80u) / animSpeed);
}

// The Skills row fields the start's sounds and overlay read.
struct SkillFxRow {
    bool stSoundDelay = false;   // the start sound waits for the swing
    bool weaponSound = false;    // the weapon's swing sound plays too
    int16_t stSound = 0, stSoundClass = 0, castOverlay = 0, itemCastSound = 0, itemCastOverlay = 0;
    int8_t charClass = -1;
};
struct SkillFxUnit {
    bool isPlayer = false;
    int32_t classId = 0;
    uint32_t hitClass = 0;       // the weapon in hand (0 unarmed) or the monster's own
    uint32_t animSpeed = 256;
};
// kind: 1 the skill's start sound, 3 the weapon's swing, 4 the class's own start sound.
struct SkillFxSound { int32_t kind; int32_t row; int32_t delay; };
struct SkillFxPlan {
    int sounds = 0;
    SkillFxSound sound[3] = {};
    int32_t overlay = 0;         // the Overlay row to start unless the unit already carries it; 0 none
};

// What a started skill plays. `fromItem`: the skill comes from an item (its owner is not -1),
// which swaps in the row's item cast sound and overlay when they are set.
inline SkillFxPlan skillStartFx(const SkillFxRow& row, const SkillFxUnit& unit, bool fromItem, int32_t overlayCount) {
    SkillFxPlan plan;
    const auto delayed = [&]() -> int32_t {
        if (!row.stSoundDelay) return 0;
        const int32_t d = swingDelayTicks(unit.hitClass, unit.animSpeed) - 3;
        return d < 0 ? 0 : d;
    };
    const bool item = fromItem && row.itemCastSound > 0;
    const int16_t start = item ? row.itemCastSound : row.stSound;
    if (start > 0) plan.sound[plan.sounds++] = { 1, start, delayed() };
    if (row.weaponSound) {
        const WeaponSwing swing = weaponSwing(unit.hitClass);
        if (swing.sound) plan.sound[plan.sounds++] = { 3, swing.sound, swingDelayTicks(unit.hitClass, unit.animSpeed) };
    }
    if (!item && row.stSoundClass > 0 && unit.isPlayer && unit.classId == row.charClass)
        plan.sound[plan.sounds++] = { 4, row.stSoundClass, delayed() };
    int32_t overlay = row.castOverlay;
    if (fromItem && row.itemCastOverlay > 0) overlay = row.itemCastOverlay;
    if (overlay > 0 && overlay < overlayCount) plan.overlay = overlay;
    return plan;
}

// A one-shot overlay advances AnimRate << 4 per tick over Frames << 8, drawn from the tick
// after it starts; this is the tick it is gone.
inline uint32_t overlayEndTick(uint32_t startTick, uint32_t frames, uint32_t animRate) {
    const uint32_t step = (animRate ? animRate : 16u) << 4, total = (frames ? frames : 1u) << 8;
    return startTick + 1u + (total + step - 1u) / step;
}

} // namespace d2::realm
