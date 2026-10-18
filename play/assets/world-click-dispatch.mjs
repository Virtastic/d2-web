/** What a held left-button world aim does at its current target. Retail re-issues the left-hand
 *  action while the button is held, so a unit under the pointer is struck again on every engine
 *  advance (the engine's own ENDANIM gate decides when the next swing may start); bare ground
 *  walks. A ground item is only picked up on the press, not by holding over it. */
export function heldWorldAction(resolved) {
  return resolved?.unitId ? 'strike' : 'walk';
}

/** Which engine command a left click on a unit runs. Retail's left button runs the selected
 *  left-hand skill: only the plain Attack (0) keeps the primary-attack path, which starts
 *  SKILL_ATTACK (A1). Anything else -- Throw (2, dwSkillMode = Skills.txt nAnim = PLRMODE_THROW)
 *  and Kick (1, nMonAnim = KK) -- must go through the engine's own left skill so it selects that
 *  skill's mode instead of a melee swing. */
export function leftHandCommand(leftSkillId) {
  return Number(leftSkillId) > 0 ? 'left-skill' : 'primary-attack';
}
