import { test } from 'node:test';
import assert from 'node:assert/strict';
import { realmCharacterCard } from './realm-character-card.mjs';

test('a CHARLIST2 entry with the ladder bit (0x40) maps to a ladder card', () => {
  const card = realmCharacterCard({ name: 'Ladder', charClass: 1, level: 7, flags: 0x40 });
  assert.equal(card.ladder, true);
  assert.equal(card.expansion, false);
  assert.equal(card.hardcore, false);
  assert.equal(card.dead, false);
});

test('ladder is independent of the other flag bits and absent when the bit is clear', () => {
  const card = realmCharacterCard({ name: 'Mixed', charClass: 3, level: 30, flags: 0x20 | 0x04 | 0x08 });
  assert.equal(card.ladder, false);
  assert.equal(card.expansion, true);
  assert.equal(card.hardcore, true);
  assert.equal(card.dead, true);
  const both = realmCharacterCard({ name: 'All', charClass: 4, level: 60, flags: 0x40 | 0x20 | 0x04 });
  assert.equal(both.ladder, true);
  assert.equal(both.expansion, true);
  assert.equal(both.hardcore, true);
});

test('the card keeps name, class and level from the list entry', () => {
  const card = realmCharacterCard({ name: 'Ghost', charClass: 6, level: 12, flags: 0 });
  assert.equal(card.name, 'Ghost');
  assert.equal(card.playerClass, 6);
  assert.equal(card.level, 12);
});
