export function armSkillTreeSelection(skillId, button, selectSkill) {
  if (button !== 0 || !Number.isInteger(skillId) || skillId < 0 || typeof selectSkill !== 'function') return null;
  selectSkill(skillId, 'right');
  return `skill-tree:${skillId}`;
}

export function allocateSkillTreePointOnRelease(token, releasedSkillId, choices, points, addSkill) {
  const match = /^skill-tree:(\d+)$/.exec(token ?? '');
  if (!match || Number(match[1]) !== releasedSkillId || !Number.isInteger(points) || points <= 0 ||
      typeof addSkill !== 'function') return false;
  const skillId = Number(match[1]);
  if (!choices?.some((skill) => skill.id === skillId && skill.canLearn)) return false;
  addSkill(skillId);
  return true;
}
