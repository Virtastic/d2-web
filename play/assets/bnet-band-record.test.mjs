import test from 'node:test';
import assert from 'node:assert/strict';
import { advanceBandRecord, bandTipLines } from './bnet-band-record.mjs';
import { bandNameText } from './bnet-lobby-screen.mjs';

const d2 = (tag, { hc = false, level = 40 } = {}) => {
  const gear = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11], col = new Array(11).fill(0);
  return `${tag}USEast,Bob,\x84\x80${String.fromCharCode(...gear)}\x02${String.fromCharCode(...col)}${String.fromCharCode(level)}${String.fromCharCode(hc ? 4 : 0, 0)}\x00\x00\x00\x00\x00`;
};
const lvl = (n) => `Level ${n}`;

test('a plain Diablo II user keeps character name, level and realm', () => {
  const r = advanceBandRecord(null, d2('PX2D', { hc: true, level: 77 }), 0);
  assert.deepEqual([r.tag, r.level, r.realm, r.hardcore, r.token], ['PX2D', 77, 'USEast', true, 7]);
  assert.deepEqual(bandTipLines(r, '*acct', lvl, 'Hardcore'), ['Expansion', 'Level 77', 'Hardcore', 'Realm: USEast', 'Account Name: *acct']);
});

test('stale tag: an update that returns before the tag is stored keeps the earlier tag, level and realm, and clears hardcore', () => {
  const a = advanceBandRecord(null, d2('PX2D', { hc: true, level: 50 }), 0);
  for (const fl of [1, 4, 8, 0x1000, 0x100000]) {
    const r = advanceBandRecord(a, d2('PX2D', { hc: true, level: 50 }), fl);
    assert.equal(r.tag, 'PX2D'); assert.equal(r.level, 50); assert.equal(r.hardcore, false);
    assert.equal(bandNameText('Bob*acct', r.tag), 'Bob', 'a stale Diablo II tag still draws the character name');
  }
  const fresh = advanceBandRecord(null, d2('PX2D'), 1);
  assert.equal(bandNameText('Bob*acct', fresh.tag), '*acct', 'a new user with no earlier tag draws the account text');
  assert.equal(advanceBandRecord(a, 'PX', 0).tag, 'PX2D', 'a statstring under four bytes keeps the tag');
  assert.equal(advanceBandRecord(a, '', 0).level, 50);
});

test('a new tag replaces the stale one; no-character and empty Diablo II strings become UNKN', () => {
  const a = advanceBandRecord(null, d2('PX2D'), 0);
  assert.equal(advanceBandRecord(a, '3RAW', 0).tag, '3RAW');
  assert.equal(advanceBandRecord(a, 'PX2D', 0).tag, 'UNKN');
  assert.equal(advanceBandRecord(a, 'PX2D', 0).token, 0x18);
  assert.equal(advanceBandRecord(a, 'PX2D', 2).token, 0x13, 'an operator keeps its token');
  assert.equal(advanceBandRecord(a, 'PX2D', 2).tag, 'UNKN');
  assert.equal(advanceBandRecord(a, 'PX2DUS,Bob,\x00\x00\x00', 0).tag, 'UNKN', 'no character byte: UNKN for a non-operator');
  assert.equal(advanceBandRecord(a, 'PX2DUS,Bob,\x00\x00\x00', 2).tag, 'PX2D', 'an operator keeps the tag');
  assert.equal(advanceBandRecord(null, 'ZZZZ', 0).tag, 'UNKN');
  assert.equal(advanceBandRecord(null, 'ZZZZ', 2).tag, 'ZZZZ');
});

test('another product: icon token, hover line and account text', () => {
  const cases = [['RATS', 0x0d, 'Starcraft'], ['RTSJ', 0x0d, 'Starcraft'], ['RHSS', 0x0d, 'Starcraft Shareware'], ['TAHC', 0x16, 'Chat'], ['LTRD', 0x0a, 'Diablo'],
    ['RHSD', 0x0a, 'Diablo Shareware'], ['TSTD', 0x0a, 'Diablo'], ['NB2W', 0x0f, 'Warcraft II'], ['PXES', 0x0e, 'Brood War'], ['PX3W', 0x10, 'Frozen Throne'], ['3RAW', 0x10, 'Warcraft III'], ['ZZZZ', 7, 'Unknown']];
  for (const [tag, token, line] of cases) {
    const r = advanceBandRecord(null, tag + 'x', 0);
    assert.equal(r.token, token, tag);
    assert.deepEqual(bandTipLines(r, '*acct', lvl, 'Hardcore'), [line, 'Account Name: *acct'], tag);
    assert.equal(bandNameText('Bob*acct', r.tag), '*acct', tag);
  }
});

test('flag tokens: hover line, and Moderator also shows the level block', () => {
  const s = d2('VD2D', { level: 12 });
  assert.deepEqual(bandTipLines(advanceBandRecord(null, s, 2), '*a', lvl, 'H'), ['Moderator', 'Level 12', 'Realm: USEast', 'Account Name: *a']);
  for (const [fl, line] of [[8, 'Sysop'], [1, 'Blizzard Rep'], [4, 'Speaker'], [0x1000, 'Referee']]) assert.deepEqual(bandTipLines(advanceBandRecord(null, s, fl), '*a', lvl, 'H'), [line, 'Account Name: *a']);
  assert.deepEqual(bandTipLines(advanceBandRecord(null, 'VD2D', 0), '*a', lvl, 'H'), ['Open Character', 'Account Name: *a']);
  assert.deepEqual(bandTipLines({ tag: 'VD2D', level: 0, realm: 'x', hardcore: false, token: 7 }, '*a', lvl, 'H'), ['Account Name: *a'], 'level 0 hides the whole block');
});
