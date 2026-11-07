import test from 'node:test';
import assert from 'node:assert/strict';
import { itemTooltipSpec, parseItemLines } from '../play/assets/item-tooltip.mjs';

// The engine's d2_session_item_lines output (monspawn item_flag_lines.h) for four fixture items: a plain
// Hand Axe, an ethereal one, one with two sockets, and an ethereal socketed one.
const B = '\x013';
const FIX = {
  normal: { flags: 0x10, raw: ['One-Hand Damage: 3 to 6', 'Durability: 28 of 28'] },
  ethereal: { flags: 0x10 | 0x400000, raw: ['One-Hand Damage: 3 to 6', 'Durability: 28 of 28', `${B}Ethereal (Cannot be Repaired)`] },
  socketed: { flags: 0x10 | 0x800, raw: ['One-Hand Damage: 3 to 6', 'Durability: 28 of 28', `${B}Socketed (2)`] },
  both: { flags: 0x10 | 0x400000 | 0x800,
    raw: ['One-Hand Damage: 3 to 6', 'Durability: 28 of 28', `${B}Ethereal (Cannot be Repaired), Socketed (2)`] },
};
const spec = (f) => { const { lines, colours } = parseItemLines(f.raw);
  return itemTooltipSpec({ name: 'Hand Axe', quality: 2, itemFlags: f.flags, props: lines, propColours: colours }); };

test('normal item: no flag lines, white name', () => {
  const s = spec(FIX.normal);
  assert.deepEqual(s.lines, ['Hand Axe', 'One-Hand Damage: 3 to 6', 'Durability: 28 of 28']);
  assert.deepEqual(s.colours, [0, 0, 0]);
});
test('ethereal item: the ethereal line is the last, blue (D2Client+0x8E180 buffer is appended first, drawn bottom-up); name grey', () => {
  const s = spec(FIX.ethereal);
  assert.deepEqual(s.lines, ['Hand Axe', 'One-Hand Damage: 3 to 6', 'Durability: 28 of 28', 'Ethereal (Cannot be Repaired)']);
  assert.deepEqual(s.colours, [5, 0, 0, 3]);
});
test('socketed item: Socketed (N) is the last line, blue', () => {
  const s = spec(FIX.socketed);
  assert.deepEqual(s.lines.slice(-1), ['Socketed (2)']);
  assert.deepEqual(s.colours, [5, 0, 0, 3]);
});
test('ethereal and socketed: one last line, "Ethereal ..., Socketed (N)"', () => {
  const s = spec(FIX.both);
  assert.deepEqual(s.lines, ['Hand Axe', 'One-Hand Damage: 3 to 6', 'Durability: 28 of 28', 'Ethereal (Cannot be Repaired), Socketed (2)']);
  assert.deepEqual(s.colours, [5, 0, 0, 3]);
});
