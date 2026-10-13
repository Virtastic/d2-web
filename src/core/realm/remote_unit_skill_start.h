#pragma once

#include <cstdint>

namespace d2::realm {

// Apply only the recovered remote-player half of D2GS 0x4C for the natural
// Fire Bolt/Fireball slice. Ops resolves the
// packet identities to canonical CUnits and implements the native state writes.
// A rejected preflight gate returns 0 without changing cast state; a skill
// lookup miss can leave the preceding skill assignment in place. -1 is reserved
// for a required adapter allocation failure.
template <typename Cast, typename Ops>
int applyRemoteUnitSkillStart(const Cast& cast, Ops& ops) {
    if (cast.opcode != 0x4c || cast.unitType != 0) return 0;
    if (cast.targetType != 0 && cast.targetType != 1) return 0;

    auto* caster = ops.resolvePlayer(cast.guid);
    if (!caster || !ops.isRemotePlayer(caster, cast.guid)) return 0;
    auto* target = ops.resolveTarget(cast.targetType, cast.targetGuid);
    if (!target || target == caster || !ops.hasCanonicalIdentity(caster, cast.guid, 0) ||
        !ops.hasCanonicalIdentity(target, cast.targetGuid, cast.targetType) ||
        !ops.bothUnitsInActiveRooms(caster, target) || !ops.casterCanStart(caster) ||
        !ops.skillSupported(cast.skill)) return 0;

    if (!ops.ensureSkillList(caster)) return -1;
    ops.assignNaturalSkill(caster, cast.skill, cast.skillLevel);
    auto* skill = ops.findSkill(caster, cast.skill);
    if (!skill) return 0;

    ops.resetPath(caster);
    const int x = ops.x(target);
    const int y = ops.y(target);
    ops.setPathTarget(caster, x, y);
    ops.updateFacing(caster, x, y);
    ops.setTargetUnit(caster, target);
    ops.setUsedSkill(caster, skill);
    ops.clearCastEventFlag(caster);
    return ops.startSkillMode(caster, skill) ? 1 : 0;
}

} // namespace d2::realm
