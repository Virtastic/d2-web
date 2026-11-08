import test from 'node:test';
import assert from 'node:assert/strict';
import { partyRows, ownState, hitTest, clickResult, tooltipFor, partyBars, inParty, hostileBlocked, cutText, headerFont, levelText,
  rowColour, PANEL, BOXES, REL, PARTY_NONE, MOST_ROWS } from './party-panel.mjs';

const roster = (...ps) => ({ list: () => ps });
const P = (id, name, extra = {}) => ({ id, name, classId: 1, level: 20, ...extra });

test('rows leave out the local player and stop at eight', () => {
  const ps = [P(1, 'Me'), ...Array.from({ length: 10 }, (_, i) => P(10 + i, 'P' + i))];
  const rows = partyRows(roster(...ps), 'me');
  assert.equal(rows.length, MOST_ROWS);
  assert.deepEqual(rows.map((r) => r.id), [10, 11, 12, 13, 14, 15, 16, 17]);
  assert.equal(rows[0].cls, 1); assert.equal(rows[0].party, PARTY_NONE); assert.equal(rows[0].relation, 0);
});

test('a local override sits over the roster until the server answers', () => {
  const rows = partyRows(roster(P(1, 'Me'), P(2, 'Al')), 'Me', new Map([[2, { relation: REL.ASKED, flags: 1 }]]));
  assert.equal(rows[0].relation, 4); assert.equal(rows[0].flags, 1);
});

test('name colours: green own party, orange another, red hostile', () => {
  const own = { party: 3 };
  assert.equal(rowColour({ party: 3 }, own), 2); assert.equal(rowColour({ party: 5 }, own), 8);
  assert.equal(rowColour({ party: PARTY_NONE }, own), 0); assert.equal(rowColour({ party: 3, hostile: true }, own), 1);
});

test('hit tests follow the row pitch and the box anchors', () => {
  const rows = partyRows(roster(P(1, 'Me'), P(2, 'Al'), P(3, 'Bo')), 'Me');
  const own = ownState(roster(P(1, 'Me')), 'Me', true);
  const at = (x, bottom, dy = 6) => hitTest(PANEL.x + x, PANEL.y + bottom - dy, rows, own);
  assert.equal(at(0xc0, 0x5a).button, true); assert.equal(at(0xc0, 0x5a).row.id, 2);
  assert.equal(at(0xc0, 0x5a + 0x26).row.id, 3);
  assert.equal(at(0x10c, 0x5a).box.kind, 'hear');
  assert.equal(at(0x10, 0x5a + 7).box.kind, 'hostile');
  assert.equal(hitTest(PANEL.x + 0x118, PANEL.y + 0x1a5 - 4, rows, own).close, true);
  assert.equal(hitTest(PANEL.x + 5, PANEL.y + 300, rows, own), null);
});

test('the party button is gone once the player is in my party', () => {
  const rows = partyRows(roster(P(1, 'Me'), P(2, 'Al', { partyState: REL.MEMBER })), 'Me');
  assert.equal(hitTest(PANEL.x + 0xc0, PANEL.y + 0x5a - 6, rows, { level: 20 }), null);
});

test('party button clicks send invite, cancel, accept and leave', () => {
  const own = { id: 1, level: 20, inTown: true };
  const mk = (relation) => partyRows(roster(P(1, 'Me'), P(2, 'Al', { partyState: relation })), 'Me')[0];
  const send = (rel) => clickResult({ button: true, row: mk(rel) }, own);
  assert.deepEqual([...send(0).packet], [0x5e, 6, 2, 0, 0, 0]); assert.equal(send(0).patch.relation, 4);
  assert.deepEqual([...send(4).packet], [0x5e, 7, 2, 0, 0, 0]); assert.equal(send(4).patch.relation, 0);
  assert.deepEqual([...send(2).packet], [0x5e, 8, 2, 0, 0, 0]); assert.equal(send(2).patch.relation, 1);
  assert.deepEqual([...send(3).packet], [0x5e, 9, 2, 0, 0, 0]);
  assert.deepEqual([...clickResult({ leave: true }, own).packet], [0x5e, 9, 1, 0, 0, 0]);
});

test('a relation box toggles its flag and sends the new state; hostility needs level 9 and town', () => {
  const own = { id: 1, level: 20, inTown: true };
  const row = partyRows(roster(P(1, 'Me'), P(2, 'Al')), 'Me')[0];
  const loot = BOXES.find((b) => b.kind === 'loot');
  const on = clickResult({ box: loot, row }, own);
  assert.deepEqual([...on.packet], [0x5d, 1, 1, 2, 0, 0, 0]); assert.equal(on.patch.flags, 1);
  const off = clickResult({ box: loot, row: { ...row, flags: 1 } }, own);
  assert.deepEqual([...off.packet], [0x5d, 1, 0, 2, 0, 0, 0]);
  const host = BOXES.find((b) => b.kind === 'hostile');
  assert.deepEqual([...clickResult({ box: host, row }, own).packet], [0x5d, 4, 1, 2, 0, 0, 0]);
  assert.equal(clickResult({ box: host, row }, { ...own, inTown: false }), null);
  assert.equal(clickResult({ box: host, row: { ...row, level: 5 } }, own), null);
  assert.equal(hostileBlocked({ level: 5, hostileToMe: true }, own), false);
});

test('tooltips pick the string by state', () => {
  const own = { level: 20, inTown: true };
  const host = BOXES.find((b) => b.kind === 'hostile'), loot = BOXES.find((b) => b.kind === 'loot');
  assert.deepEqual(tooltipFor({ box: host, row: { level: 20, flags: 0 } }, [], own), { first: 0x100a, second: 0x100b, x: 0xa0 });
  assert.deepEqual(tooltipFor({ box: host, row: { level: 20, flags: 8 } }, [], own), { first: 0x100c, second: 0x100d, x: 0xa0 });
  assert.deepEqual(tooltipFor({ box: host, row: { level: 3, flags: 0 } }, [], own), { first: 0x101a, second: 0, x: 0xa0 });
  assert.deepEqual(tooltipFor({ box: host, row: { level: 20, flags: 0 } }, [], { ...own, inTown: false }), { first: 0x1019, second: 0, x: 0x6e });
  assert.equal(tooltipFor({ box: loot, row: { flags: 1 } }, [], own).first, 0x1010);
  assert.equal(tooltipFor({ button: true, row: { relation: 2 } }, [], own).first, 0x1006);
  assert.equal(tooltipFor(null, [], own), null);
});

test('a bar separates two parties and a lone party id counts as none', () => {
  const rows = [{ party: 4 }, { party: 4 }, { party: 9 }, { party: 9 }, { party: PARTY_NONE }];
  assert.deepEqual(partyBars(rows, { party: 4 }), [2, 4]);
  assert.equal(inParty(rows, { party: 4 }), true);
  assert.equal(inParty([{ party: 7 }], { party: 4 }), false);
});

test('names are cut with dots, the header font shrinks and the level text is padded', () => {
  const m = (s) => s.length * 6;
  assert.equal(cutText(m, 'Short', 0x88), 'Short');
  assert.equal(cutText(m, 'AVeryVeryLongPlayerName', 96), 'AVeryVeryLon...');
  assert.ok(m(cutText(m, 'AVeryVeryLongPlayerName', 96)) < 96);
  assert.deepEqual(['abcdefghij', 'abcdefghijkl', 'abcdefghijklmn'].map(headerFont), ['font16', 'font8', 'font6']);
  assert.equal(levelText((id) => ({ 0xd28: 'Lvl', 0xf9d: ':', 0xf9b: ' ' })[id], 9), 'Lvl: ' + ' 9');
});
