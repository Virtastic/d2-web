import test from 'node:test';
import assert from 'node:assert/strict';
import { CARD_LINE_COLOURS } from './charselect.mjs';
// The card's ladder line is drawn in text colour 4, the expansion line in colour 2 (the character-select card draw).
test('the ladder line of a character card is colour 4, not the expansion line colour', () => {
  assert.equal(CARD_LINE_COLOURS.ladder, 4);
  assert.equal(CARD_LINE_COLOURS.expansion, 2);
});
