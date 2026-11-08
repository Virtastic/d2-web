import test from 'node:test';
import assert from 'node:assert/strict';
import { miniPanelTooltip, keyLabel } from '../play/assets/minipanel-tooltip.mjs';
// Strings and positions are the retail hover capture's (800x600, font 1, colour 0, centred).
const T = { 4169: 'Character', 4170: 'Inventory', 4171: 'Skill Tree', 4173: 'Automap', 4174: 'Message Log', 4175: 'Quest Log',
  4176: 'Game Menu (Esc)', 4178: ' (%s)', 3791: 'Tab', 3766: 'Mouse 3', 3814: 'Num Pad 5', 3829: 'F5' };
const lang = (i) => T[i];
const slots = { 'Character Screen': 0, 'Inventory Screen': 1, 'Skill Tree': 8, Automap: 45, 'Message Log': 4, 'Quest Log': 5 };
const tips = (bindings) => [326, 347, 368, 389, 410, 431, 452].map((left, i) =>
  miniPanelTooltip(i, left, 800, 600, left + 10, 540, { lang, slotOf: (n) => slots[n], bindings }));
// bindings[slot] = [first column, second column]; the suffix lists the second column first.
const dflt = { 0: ['C', 'A'], 1: ['B', 'I'], 8: [null, 'T'], 45: ['Mouse 3', 'Tab'], 4: [null, 'M'], 5: [null, 'Q'], 20: [null, 'Esc'] };
const rebound = { 0: ['C', 'F5'], 1: [null, 'I'], 8: ['S', 'T'], 45: ['Mouse 3', null], 4: [null, null], 5: ['Num Pad 5', null], 20: ['X', 'Y'] };
test('default bindings: capture strings and positions', () => {
  const want = ['Character (A) (C)', 'Inventory (I) (B)', 'Skill Tree (T)', 'Automap (Tab) (Mouse 3)', 'Message Log (M)', 'Quest Log (Q)', 'Game Menu (Esc)'];
  tips(dflt).forEach((t, i) => assert.deepEqual(t, { text: want[i], x: 323 + 21 * i, y: 524, color: 0 }));
});
test('rebound slots: unbound adds nothing, Game Menu unchanged', () => {
  const want = ['Character (F5) (C)', 'Inventory (I)', 'Skill Tree (T) (S)', 'Automap (Mouse 3)', 'Message Log', 'Quest Log (Num Pad 5)', 'Game Menu (Esc)'];
  tips(rebound).forEach((t, i) => assert.equal(t.text, want[i]));
});
test('retail hit edges are strict', () => {
  const b = { lang, slotOf: (n) => slots[n], bindings: dflt };
  assert.equal(miniPanelTooltip(0, 326, 800, 600, 326, 540, b), null);
  assert.equal(miniPanelTooltip(0, 326, 800, 600, 346, 540, b), null);
  assert.equal(miniPanelTooltip(0, 326, 800, 600, 336, 531, b), null);
  assert.equal(miniPanelTooltip(0, 326, 800, 600, 336, 553, b), null);
  assert.ok(miniPanelTooltip(0, 326, 800, 600, 336, 532, b));
});
test('key names come from the language table', () => {
  assert.equal(keyLabel('Tab', (i) => `<${i}>`), '<3791>');
  assert.equal(keyLabel('Mouse 3', (i) => `<${i}>`), '<3766>');
  assert.equal(keyLabel('Num Pad 5', lang), 'Num Pad 5');
  assert.equal(keyLabel('F5', (i) => `<${i}>`), '<3829>');
  assert.equal(keyLabel('A', (i) => `<${i}>`), 'A');
});
