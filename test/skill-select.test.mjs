import test from 'node:test';
import assert from 'node:assert/strict';
import { parseSkillChoices, skillSelectRows, SKILL_SELECT_LAYOUT } from '../play/assets/skill-select.mjs';

test('skill choices parse from the engine record', () => {
  const b = new Uint8Array(16), v = new DataView(b.buffer);
  v.setUint16(0, 126, true); v.setInt8(2, 1); v.setInt8(3, 4); v.setUint16(4, 3, true); v.setUint8(6, 2); v.setUint8(7, 5);
  v.setUint16(8, 127, true); v.setInt8(10, 2); v.setInt8(11, 4); v.setUint16(12, 7, true); v.setUint8(14, 1); v.setUint8(15, 5);
  assert.deepEqual(parseSkillChoices(b, 2), [
    { id: 126, listRow: 1, charClass: 4, iconCel: 3, passive: false, leftSkill: true, level: 5 },
    { id: 127, listRow: 2, charClass: 4, iconCel: 7, passive: true, leftSkill: false, level: 5 }]);
});

test('popup rows follow OpenDiablo2 skill_select_panel.go: bottom-up, empty rows skipped, passives out', () => {
  const s = (id, listRow, extra = {}) => ({ id, listRow, charClass: 4, iconCel: 0, passive: false, leftSkill: true, level: 1, ...extra });
  const choices = [s(0, 0), s(2, 0, { leftSkill: false }), s(126, 1), s(133, 1), s(127, 2, { passive: true }), s(130, 3, { leftSkill: false }), s(9, -1)];
  const right = skillSelectRows(choices, 'right');
  assert.deepEqual(right.map((r) => [r.listRow, r.top, r.skills.map((k) => k.id)]), [[0, 465, [2, 0]], [1, 417, [133, 126]], [3, 369, [130]]]);
  assert.equal(right[0].x + right[0].width, SKILL_SELECT_LAYOUT.rightEndX);
  const left = skillSelectRows(choices, 'left');
  assert.deepEqual(left.map((r) => [r.listRow, r.x, r.skills.map((k) => k.id)]), [[0, 90, [0]], [1, 90, [126, 133]]]);
});
