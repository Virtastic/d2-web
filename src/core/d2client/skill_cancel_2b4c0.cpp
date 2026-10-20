// Prior art used: ; D2MOO D2Skills and D2States provide
// the native skill/state owners (no D2Client implementation exists there).
// Retail's instructions recover EDI=unit and EDX=skillId, both absent from
// the cached decompile signature. Common 1.13c #10630 selects the highest
// skill, #10397 reads its flags, #10137 writes them, #10945 toggles a state.
#include "skill_cancel_2b4c0.h"
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/D2States.h"

// 1.13c D2Client+0x2B4C0
extern "C" void d2_client_2b4c0(D2UnitStrc* unit, int32_t skillId) {
    auto* skill = SKILLS_GetHighestLevelSkillFromUnitAndId(unit, skillId);
    const auto flags = SKILLS_GetFlags(skill);
    if (flags & 1)
        SKILLS_SetFlags(skill, flags | 2);
    STATES_ToggleState(unit, 54, 0);
}
