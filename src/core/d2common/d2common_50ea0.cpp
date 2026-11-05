// The effective level of a unit's skill: the skill node's base level, plus the bonus levels when the node is not a
// fixed-level grant (owner word -1), floored at 0 and capped below the table maximum (the cap itself is returned at or above it).
#include <cstdint>

struct D2SkillLevelSeam {
    uint32_t (*pfnFindSkill)(uint32_t pUnit, int32_t nSkill);   // the unit's node for a skill id, 0 when absent
    int32_t (*pfnBonusLevels)(uint32_t pNode, uint32_t pUnit);  // the bonus levels of a node
    int32_t (*pfnNodeLevel)(uint32_t pNode);                    // the node's base level
    int32_t (*pfnNodeOwner)(uint32_t pNode);                    // the node's owner word (-1 = not a granted skill)
    int32_t nCap;                                               // the level cap word of the experience table
};

// 1.13c D2Common+0x50EA0
int32_t D2Common_50EA0_SkillLevel(const D2SkillLevelSeam* s, uint32_t pUnit, int32_t nSkill)
{
    if (!pUnit) return 0;
    const uint32_t pNode = s->pfnFindSkill(pUnit, nSkill);
    if (!pNode) return 0;
    int32_t v = s->pfnNodeLevel(pNode);
    if (s->pfnNodeOwner(pNode) == -1) v = static_cast<int32_t>(static_cast<uint32_t>(v) + static_cast<uint32_t>(s->pfnBonusLevels(pNode, pUnit)));
    if (v < 1) v = 0;
    return v < s->nCap ? v : s->nCap;
}
