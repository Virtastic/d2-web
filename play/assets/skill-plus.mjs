// Retail's skill-point affordance, layered over a skill-tree icon.  The tree owns
// the icon rectangle; this module only adds level.DC6 at its documented inset.
// OpenDiablo2's skilltree/skillicon layout gives a 48x48 icon, so level.DC6's
// 30x30 frame is inset by (9,9).
export const SKILL_PLUS_ART = 'data\\global\\ui\\PANEL\\level.dc6';
export const SKILL_PLUS_FRAME = 0;
export const SKILL_PLUS_INSET = 9;

// Whether retail would accept a point here is the ENGINE's answer, not this client's
// (d2_session_class_skills' canLearn: the exact conjunction D2Game's 0x3B handler tests before it
// spends a point -- class skill, prerequisites, required attributes, maximum level and the points
// the next level costs). This module used to re-derive three of those five in JS, which offered a
// "+" on a skill retail rejects for its PREREQUISITES.
export function isSkillPointCandidate(skill) {
  return skill?.canLearn === true;
}

export function drawSkillPointPlus(chrome, ctx, act, iconRect) {
  return chrome.cel(ctx, act, SKILL_PLUS_ART, SKILL_PLUS_FRAME,
    iconRect.x + SKILL_PLUS_INSET, iconRect.y + SKILL_PLUS_INSET + 30 - 1);
}

export function createSkillPointOverlay() {
  return Object.freeze({ isCandidate: isSkillPointCandidate, draw: drawSkillPointPlus });
}
