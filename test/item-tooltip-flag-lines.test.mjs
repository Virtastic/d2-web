import test from 'node:test';
import assert from 'node:assert/strict';
import { itemTooltipSpec, parseItemLines } from '../play/assets/item-tooltip.mjs';

// The engine's d2_session_item_lines output (monspawn item_flag_lines.h) for four fixture items: a plain
// Hand Axe, an ethereal one, one with two sockets, and an ethereal socketed one.
const B = '\x013';
const FIX = {
  normal: { flags: 0x10, raw: ['One-Hand Damage: 3 to 6', 'Durability: 28 of 28'] },
  ethereal: { flags: 0x10 | 0x400000, raw: [`${B}Ethereal (Cannot be Repaired)`, 'One-Hand Damage: 3 to 6', 'Durability: 28 of 28'] },
  socketed: { flags: 0x10 | 0x800, raw: [`${B}Socketed (2)`, 'One-Hand Damage: 3 to 6', 'Durability: 28 of 28'] },
  both: { flags: 0x10 | 0x400000 | 0x800,
    raw: [`${B}Ethereal (Cannot be Repaired)`, `${B}Socketed (2)`, 'One-Hand Damage: 3 to 6', 'Durability: 28 of 28'] },
};
const spec = (f) => { const { lines, colours } = parseItemLines(f.raw);
  return itemTooltipSpec({ name: 'Hand Axe', quality: 2, itemFlags: f.flags, props: lines, propColours: colours }); };

test('normal item: no flag lines, white name', () => {
  const s = spec(FIX.normal);
  assert.deepEqual(s.lines, ['Hand Axe', 'One-Hand Damage: 3 to 6', 'Durability: 28 of 28']);
  assert.deepEqual(s.colours, [0, 0, 0]);
});
test('ethereal item: the ethereal line follows the name, blue, before Damage; name grey', () => {
  const s = spec(FIX.ethereal);
  assert.deepEqual(s.lines.slice(0, 3), ['Hand Axe', 'Ethereal (Cannot be Repaired)', 'One-Hand Damage: 3 to 6']);
  assert.deepEqual(s.colours, [5, 3, 0, 0]);
});
test('socketed item: Socketed (N) line, blue', () => {
  const s = spec(FIX.socketed);
  assert.deepEqual(s.lines.slice(0, 2), ['Hand Axe', 'Socketed (2)']);
  assert.deepEqual(s.colours.slice(0, 2), [5, 3]);
});
test('ethereal and socketed: ethereal first, then socketed, then the base lines', () => {
  const s = spec(FIX.both);
  assert.deepEqual(s.lines, ['Hand Axe', 'Ethereal (Cannot be Repaired)', 'Socketed (2)', 'One-Hand Damage: 3 to 6', 'Durability: 28 of 28']);
  assert.deepEqual(s.colours, [5, 3, 3, 0, 0]);
});
