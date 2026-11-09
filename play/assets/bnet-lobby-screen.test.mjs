import test from 'node:test';
import assert from 'node:assert/strict';
import { createBnetLobbyScreen, characterInfoLines, difficultiesAllowed, gameErrorText, bandNameStyle, BNET_LOBBY_LAYOUT } from './bnet-lobby-screen.mjs';

// No canvas in node: a context that swallows every call, and one-pixel art for every file.
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) };
const frame = { width: 1, height: 1, offsetX: 0, offsetY: 0, indices: new Uint8Array(1), opaque: new Uint8Array([1]) };
const strings = { 4057: 'Level', 4011: 'Amazon', 4009: 'Necromancer', 11124: 'Last Difficulty Setting Completed:', 11125: 'None', 5156: 'Normal', 5155: 'Nightmare', 5154: 'Hell', 5138: 'A Game Already Exists With That Name' };
const screen = (character, hooks = {}) => createBnetLobbyScreen({ read: (path) => (path.endsWith('.tbl') ? tbl : new Uint8Array(1 << 22)), decodeDc6: () => ({ frames: [frame, frame, frame, frame, frame, { ...frame, width: 10 }] }),
  decodeDatPalette: () => new Uint8Array(1024), string: (i) => strings[i] ?? '', character, documentObject, now: () => 0, ...hooks });
// A font table: the Woo! header, 10 rows high, every printable code point one 6-wide glyph.
const tbl = new Uint8Array(12 + 95 * 14); tbl.set([0x57, 0x6f, 0x6f, 0x21]); tbl[10] = 10;
for (let c = 32; c < 127; c++) { const b = 12 + (c - 32) * 14; tbl[b] = c; tbl[b + 3] = 6; tbl[b + 4] = 10; }
const tick = () => new Promise((r) => setImmediate(r));

test('character box lines follow D2Multi: name, Level N Class, last difficulty completed', () => {
  assert.deepEqual(characterInfoLines({ name: 'Ama', charClass: 0, level: 7, hardcore: true }, (i) => strings[i]),
    [{ text: 'Ama', colour: 1 }, { text: 'Level 7 Amazon', colour: 0 }, { text: 'Last Difficulty Setting Completed:', colour: 0 }, { text: 'None', colour: 0 }]);
  assert.equal(characterInfoLines({ name: 'N', charClass: 2, level: 1, expansion: true, progression: 15 }, (i) => strings[i])[3].text, 'Hell');
  assert.equal(characterInfoLines({ name: 'N', charClass: 2, progression: 4 }, (i) => strings[i])[3].text, 'Normal');
});

test('band names: a plain name is colour 1 with any event flag or a hardcore statstring, else 0', () => {
  assert.equal(bandNameStyle(0, false).colour, 0);
  assert.equal(bandNameStyle(0, true).colour, 1, 'statstring 0x04 alone');
  for (const fl of [0x02, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800]) assert.deepEqual(bandNameStyle(fl, false), { star: false, colour: 1 }, `event 0x${fl.toString(16)}`);
});

test('band names: operator 0x02 takes no star; 0x01 and 0x08 draw the star twice (6 at -1,-1, then 3); 0x04, 0x1000, 0x100000 a colour-0 star', () => {
  assert.deepEqual(bandNameStyle(0x02, false), { star: false, colour: 1 });
  assert.deepEqual(bandNameStyle(0x01, false), { star: true, layers: [[6, -1, -1], [3, 0, 0]] });
  assert.deepEqual(bandNameStyle(0x08, true), { star: true, layers: [[6, -1, -1], [3, 0, 0]] });
  for (const fl of [0x04, 0x1000, 0x100000]) assert.deepEqual(bandNameStyle(fl, false), { star: true, layers: [[0, 0, 0]] }, `event 0x${fl.toString(16)}`);
});

test('difficulty gating: Nightmare after 4 (classic) / 5 (expansion), Hell after 8 / 10', () => {
  assert.deepEqual(difficultiesAllowed({ progression: 0 }), [true, false, false]);
  assert.deepEqual(difficultiesAllowed({ progression: 4 }), [true, true, false]);
  assert.deepEqual(difficultiesAllowed({ progression: 4, expansion: true }), [true, false, false]);
  assert.deepEqual(difficultiesAllowed({ progression: 10, expansion: true }), [true, true, true]);
});

test('MCP rejections map to the D2Lang notice', () => {
  assert.equal(gameErrorText({ mcpStage: 'create', mcpResult: 0x1f, message: 'x' }, (i) => strings[i]), 'A Game Already Exists With That Name');
  assert.equal(gameErrorText(new Error('socket closed'), () => ''), 'socket closed');
  assert.equal(gameErrorText({ noticeString: 5357, message: 'closed' }, (i) => (i === 5357 ? 'Disconnected' : '')), 'Disconnected');
});

test('every create and join result raises the notice the retail switch picks, and unknown results the generic failure', () => {
  const str = (i) => `s${i}`, err = (mcpStage, mcpResult) => ({ mcpStage, mcpResult, message: 'x' });
  const create = (r) => gameErrorText(err('create', r), str);
  assert.deepEqual([0x1e, 0x1f, 0x20, 0x6e, 0x55].map(create), ['s5137', 's5138', 's5139', 's5140', 's5140']);
  const join = (r, c) => gameErrorText(err('join', r), str, c);
  assert.deepEqual([0x29, 0x2a, 0x2b, 0x2c, 0x71, 0x78, 0x79, 0x6e, 0x01].map((r) => join(r)),
    ['s5160', 's5159', 's5161', 's5162', 's5158', 's10101', 's10102', 's5141', 's5141']);
  assert.deepEqual([join(0x73, { expansion: false }), join(0x73, { expansion: true }), join(0x74, {}), join(0x74, { expansion: true })],
    ['s5364', 's21794', 's5363', 's21793']);
  assert.deepEqual([join(0x7d, { ladder: true }), join(0x7d, { ladder: false })], ['s10929', 's10930']);
});

test('lobby -> CREATE -> typed game -> Enter creates it with the form', async () => {
  const calls = [];
  const s = screen({ name: 'Ama', charClass: 0, level: 3 }, { onCreateGame: async (g) => { calls.push(g); } });
  assert.equal(s.mode, 'lobby');
  assert.ok(s.evidence.boxes.find((b) => b.id === 'channel').disabled);
  assert.ok(s.activate('create'));
  assert.equal(s.panel, 'create');
  for (const k of 'my game') s.typeKey(k === ' ' ? 'Space' : `Key${k}`, k);
  s.typeKey('Tab'); for (const k of 'pw') s.typeKey(`Key${k}`, k);
  s.activate('difficulty-1');                          // Nightmare is locked for a fresh character
  assert.equal(s.evidence.form.difficulty, 0);
  s.typeKey('Enter');
  await tick();
  assert.deepEqual(calls, [{ gameName: 'my game', gamePassword: 'pw', description: '', maxPlayers: 4, difficulty: 0, levelDifference: 0xff }]);
  assert.ok(s.busy, 'stays busy until the game replaces the lobby');
});

test('a rejected create shows the notice and keeps the panel; OK dismisses it', async () => {
  const s = screen({ name: 'Ama', charClass: 0 }, { onCreateGame: async () => { throw Object.assign(new Error('rejected'), { mcpStage: 'create', mcpResult: 0x1f }); } });
  s.activate('create'); s.typeKey('KeyA', 'a'); s.activate('create-game');
  await tick();
  assert.equal(s.evidence.notice.body, 'A Game Already Exists With That Name');
  assert.deepEqual(Object.keys(s.targets(800, 600)), ['notice-ok']);
  s.activate('notice-ok');
  assert.equal(s.evidence.notice, null);
  assert.equal(s.panel, 'create');
});

test('JOIN sends name and password; Escape closes the panel, then quits', async () => {
  const joins = [];
  let quit = 0;
  const s = screen({ name: 'Ama', charClass: 0 }, { onJoinGame: async (g) => { joins.push(g); throw Object.assign(new Error('x'), { sessionLost: true }); }, onQuit: () => { quit++; } });
  s.activate('join'); s.typeKey('KeyG', 'g'); s.typeKey('Tab'); s.typeKey('KeyP', 'p');
  s.activate('join-game'); await tick();
  assert.deepEqual(joins, [{ gameName: 'g', gamePassword: 'p' }]);
  assert.equal(s.evidence.notice, null, 'a lost session leaves the notice to the login screen');
  s.escape(); assert.equal(s.panel, null);
  s.escape(); assert.equal(quit, 1);
});

test('a click on a game row fills the name box, enables JOIN and joins that game with the typed password', async () => {
  const joins = [];
  const s = screen({ name: 'Ama', charClass: 0 }, { onJoinGame: async (g) => { joins.push(g); throw Object.assign(new Error('x'), { sessionLost: true }); } });
  s.activate('join');
  s.setGames(Array.from({ length: 12 }, (_, i) => ({ name: `game${i}`, password: i === 2, description: `text ${i}` })));
  assert.equal(s.evidence.boxes.find((b) => b.id === 'join-game').disabled, true, 'no name yet');
  assert.equal(s.press(440, 100, 800, 600), null, 'above the list: no row');
  assert.equal(s.press(440, 392 - 19 * 2 - 2, 800, 600) === 'list', true);
  assert.equal(s.selectedGame().name, 'game6', 'row 6 from the top: (y - 220) / 19');
  assert.equal(s.editValue(), null, 'the click moved the focus off the edit boxes');
  assert.equal(s.evidence.boxes.find((b) => b.id === 'join-game').disabled, false);
  s.scrollGames(3); s.press(440, 229, 800, 600);
  assert.equal(s.selectedGame().name, 'game3', 'the first row on show counts');
  s.press(440, 392, 800, 600); assert.equal(s.selectedGame(), null, 'a click below the last game clears the pick');
  s.pinClock(60000, 0); s.press(440, 229, 800, 600); assert.equal(s.selectedGame().name, 'game3');
  s.activate('field-password'); s.typeKey('KeyP', 'p'); s.activate('join-game'); await tick();
  assert.deepEqual(joins, [{ gameName: 'game3', gamePassword: 'p' }]);
});

test('a dead hardcore character cannot open CREATE or JOIN', () => {
  const s = screen({ name: 'H', charClass: 0, hardcore: true, dead: true });
  s.activate('create'); s.activate('join');
  assert.equal(s.panel, null);
});

test('hit boxes are the descriptors (x, bottom row y, w, h)', () => {
  const s = screen({ name: 'Ama', charClass: 0 });
  const q = BNET_LOBBY_LAYOUT.quit, box = s.evidence.boxes.find((b) => b.id === 'quit');
  assert.deepEqual([box.x, box.y, box.w, box.h], [q.x, q.y - q.h + 1, q.w, q.h]);
  const token = s.press(q.x + 5, q.y - 5, 800, 600);
  assert.equal(token, 'quit');
});

test('every view draws: lobby, chat, CREATE, JOIN, notice', () => {
  const s = screen({ name: 'Ama', charClass: 0, level: 3 });
  s.draw(ctx, 800, 600);
  s.typeKey('Enter'); assert.equal(s.mode, 'chat');
  s.typeKey('KeyH', 'h'); s.typeKey('Enter'); assert.deepEqual(s.evidence.chat, ['<Ama> h']);
  s.draw(ctx, 800, 600);
  s.activate('create'); s.activate('level-box'); s.draw(ctx, 800, 600);
  s.activate('join'); s.draw(ctx, 800, 600);
  s.fail(new Error('x')); s.draw(ctx, 800, 600);
});

test('chat: typing and Enter go out through the chat socket; the server\'s lines land in the log', () => {
  const out = []; let entered = 0;
  const s = screen({ name: 'Ama', charClass: 0, level: 3 }, { onSendChat: (t) => { out.push(t); return true; }, onEnterChat: () => { entered++; } });
  s.activate('enter-chat'); assert.equal(s.mode, 'chat'); assert.equal(entered, 1);
  s.typeKey('KeyH', 'h'); s.typeKey('KeyI', 'i'); s.typeKey('Enter');
  assert.deepEqual(out, []);                       // no live socket yet: echoed locally
  assert.deepEqual(s.evidence.chat, ['<Ama> hi']);
  s.chatLive(true);
  s.typeKey('KeyY', 'y'); s.typeKey('Enter');
  assert.deepEqual(out, ['y']); assert.deepEqual(s.evidence.chat, ['<Ama> hi', '<Ama> y']);   // Battle.net does not echo the speaker: drawn here
  s.chatLine({ text: '<Bob> yo', colour: 0 }); assert.equal(s.evidence.chat.at(-1), '<Bob> yo');
  s.activate('whisper'); s.typeKey('KeyB', 'b'); assert.equal(s.evidence.chat.length, 3);
  s.activate('whisper'); assert.deepEqual(out, ['y']);                   // WHISPER needs a picked user
  s.pickUser('*Bob'); s.activate('whisper'); assert.deepEqual(out.at(-1), '/whisper *Bob b');
  s.typeKey('KeyC', 'c'); s.activate('emote'); assert.deepEqual(out.at(-1), '/me c');
  s.pickUser('Bob'); s.activate('squelch'); assert.deepEqual(out.at(-1), '/me c');   // inside the 1 s gate
});

test('join panel asks for the game list on open, then on retail\'s free-running 15 s idle timer while it stays open', () => {
  let asked = 0;
  const s = screen({ name: 'Ama', charClass: 0, level: 3 }, { onRefreshGames: () => { asked++; } });
  s.pinClock(1000, 0); s.tick(); assert.equal(asked, 0, 'no panel, no request');
  s.activate('join'); assert.equal(asked, 1, 'asked when it opens');
  s.tick(); assert.equal(asked, 2, 'the timer was never started: the first idle look is due at once');
  s.pinClock(15999, 0); s.tick(); assert.equal(asked, 2);
  s.pinClock(16000, 0); s.tick(); assert.equal(asked, 3, '15 s after the timer last fired');
  s.pinClock(20000, 0); s.draw(ctx, 800, 600); assert.equal(asked, 3, 'draw ticks too, nothing due');
  s.pinClock(31000, 0); s.draw(ctx, 800, 600); assert.equal(asked, 4, 'draw runs the due refresh');
  s.activate('panel-cancel'); s.pinClock(60000, 0); s.tick(); assert.equal(asked, 4, 'closed: the timer is not looked at');
  s.activate('join'); assert.equal(asked, 5, 'reopened: the open request');
  s.tick(); assert.equal(asked, 6, 'and the stale timer is due at once');
  s.activate('panel-cancel'); s.pinClock(65000, 0); s.activate('join'); assert.equal(asked, 7); s.tick(); assert.equal(asked, 7, 'reopened 5 s after the last firing: it waits out the rest');
  s.pinClock(75000, 0); s.tick(); assert.equal(asked, 8);
});

const games = (n) => Array.from({ length: n }, (_, i) => ({ name: `game${i}`, description: `text ${i}` }));
const pinned = (hooks) => { const s = screen({ name: 'Ama', charClass: 0 }, hooks); s.activate('join'); return s; };

test('a refresh keeps the pick by name, or the same row number when the game is gone; the description empties', () => {
  const s = pinned(); s.setGames(games(12)); s.press(440, 248, 800, 600);
  assert.equal(s.selectedGame().name, 'game1');
  s.setGames([{ name: 'fresh', description: '' }, ...games(12)]);
  assert.equal(s.selectedGame().name, 'game1', 'followed to its new row');
  s.setGames(games(12).filter((g) => g.name !== 'game1'));
  assert.equal(s.selectedGame().name, 'game3', 'same row number (2), another game');
});

test('the same row twice within 500 ms joins; a slower second click only selects', async () => {
  const joins = []; let t = 1000;
  const s = pinned({ onJoinGame: async (g) => { joins.push(g); throw Object.assign(new Error('x'), { sessionLost: true }); } });
  s.pinClock(t, 0); s.setGames(games(12));
  s.press(440, 229, 800, 600); s.pinClock(t += 600, 0); s.press(440, 229, 800, 600); await tick();
  assert.deepEqual(joins, [], 'too slow');
  s.pinClock(t += 300, 0); s.press(440, 229, 800, 600); await tick();
  assert.deepEqual(joins, [{ gameName: 'game0', gamePassword: '' }]);
});

test('the list scrollbar: arrows step a row, the track pages, a grabbed thumb follows the pointer, release lets go', () => {
  const s = pinned(); s.setGames(games(14));
  assert.equal(s.press(597, 392, 800, 600), 'scrollbar'); s.release('scrollbar', 597, 392, 800, 600);
  s.press(440, 229, 800, 600); assert.equal(s.selectedGame().name, 'game1', 'one row down');
  s.press(597, 300, 800, 600); s.release('scrollbar', 597, 300, 800, 600);
  s.press(440, 229, 800, 600); assert.equal(s.selectedGame().name, 'game5', 'clamped at the last position (14 - 9 rows)');
  s.press(597, 229, 800, 600); s.release('scrollbar', 597, 229, 800, 600);
  s.press(440, 229, 800, 600); assert.equal(s.selectedGame().name, 'game4', 'one row up');
  const d = pinned(); d.setGames(games(14));
  assert.equal(d.press(597, 245, 800, 600), 'scrollbar'); d.hover(597, 400, 800, 600);
  d.press(440, 229, 800, 600); assert.equal(d.selectedGame().name, 'game5', 'the grabbed thumb followed the pointer to the end');
  d.release('scrollbar', 597, 400, 800, 600); d.hover(597, 229, 800, 600); d.press(440, 229, 800, 600);
  assert.equal(d.selectedGame().name, 'game5', 'after release the thumb no longer follows');
});

test('no scrollbar press when every game fits', () => {
  const s = pinned(); s.setGames(games(5));
  assert.equal(s.press(597, 300, 800, 600), null);
});

import { displayName as _dn, accountName as _an } from './bnet-lobby-screen.mjs';
test('realm Char*account names show the character and keep the account', () => {
  assert.equal(_dn('Bob*acct'), 'Bob'); assert.equal(_an('Bob*acct'), 'acct');
  assert.equal(_dn('*acct'), '*acct'); assert.equal(_an('*acct'), 'acct');
  assert.equal(_dn('Bob'), 'Bob'); assert.equal(_an('Bob'), 'Bob');
});

test('a join of a listed name carries that row\'s difficulty and description, a name the list does not show carries none', async () => {
  const joins = [];
  const s = screen({ name: 'Ama', charClass: 0 }, { onJoinGame: async (g) => { joins.push(g); throw Object.assign(new Error('x'), { sessionLost: true }); } });
  s.activate('join');
  s.setGames([{ name: 'Cows', password: false, difficulty: 3, description: 'cow level' }]);
  s.press(440, 229, 800, 600); s.typeKey('Enter');
  await tick();
  assert.deepEqual(joins[0], { gameName: 'Cows', gamePassword: '' });
  const typed = screen({ name: 'Ama', charClass: 0 }, { onJoinGame: async (g) => { joins.push(g); throw Object.assign(new Error('x'), { sessionLost: true }); } });
  typed.activate('join');
  typed.setGames([{ name: 'Cows', difficulty: 3, description: '' }]);
  typed.typeKey('KeyC', 'c'); typed.typeKey('Enter');
  await tick();
  assert.deepEqual(joins[1], { gameName: 'c', gamePassword: '' });
});

test('CREATE clamps a typed player count as retail does: 0 is 1, 9 and up is 8', async () => {
  const sent = [];
  for (const typed of ['0', '00', '9', '50', '3']) {
    const s = screen({ name: 'Ama', charClass: 0, level: 3 }, { onCreateGame: async (g) => { sent.push(g.maxPlayers); } });
    s.activate('create');
    s.typeKey('KeyA', 'a'); s.typeKey('Tab'); s.typeKey('Tab'); s.typeKey('Tab');
    s.typeKey('Backspace'); s.typeKey(`Digit${typed[0]}`, typed[0]); for (const k of typed.slice(1)) s.typeKey(`Digit${k}`, k);
    s.typeKey('Enter'); await tick();
  }
  assert.deepEqual(sent, [1, 1, 8, 8, 3]);
});





test('chat keeps working with HELP / LADDER open, and a press on the chat line takes the keys back from CREATE', () => {
  const out = [], created = [];
  const s = screen({ name: 'Ama', charClass: 0, level: 3 }, { onSendChat: (t) => { out.push(t); return true; }, onCreateGame: async (g) => { created.push(g); } });
  s.activate('enter-chat'); s.chatLive(true);
  s.activate('ladder'); assert.equal(s.panel, 'ladder');
  assert.equal(s.editAt(40, 435, 800, 600), true, 'the chat line takes a tap while LADDER is up');
  s.typeKey('KeyA', 'a'); s.typeKey('Enter'); assert.deepEqual(out, ['a']);
  s.activate('create'); s.typeKey('KeyN', 'n');
  assert.equal(s.evidence.form.name, 'n', 'CREATE owns the keys at first');
  assert.ok(s.activate('field-chat'), 'the chat line is a control under CREATE');
  s.typeKey('KeyB', 'b'); s.typeKey('Enter');
  assert.deepEqual(out, ['a', 'b']); assert.deepEqual(created, []); assert.equal(s.evidence.form.name, 'n');
  s.activate('field-name'); s.typeKey('KeyZ', 'z'); assert.equal(s.evidence.form.name, 'nz', 'a press on a CREATE box gives them back');
});

test('the user band paints the figure the statstring names, in the band cell, once per user', () => {
  const drawn = [], seen = [];
  const rec = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : k === 'drawImage' ? (c, x, y) => drawn.push([c.tag, x, y]) : () => {}), set: () => true });
  const doc = { createElement: () => ({ width: 0, height: 0, getContext: () => rec }) };
  const layer = { canvas: { tag: 'L' }, left: -10, top: -50 };
  const s = screen({ name: 'Ama', charClass: 0, level: 3 }, { documentObject: doc, figure: (stat) => { seen.push(stat); return { layers: [layer] }; } });
  s.activate('enter-chat');
  s.chatEvent({ event: 0x01, username: 'Ama*Acct', raw: 'PX2Dr,Ama,\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05' + '\xff'.repeat(11) + '\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00', flags: 0 });
  drawn.length = 0; s.draw(rec, 800, 600);
  assert.equal(seen.length, 1);
  const B = BNET_LOBBY_LAYOUT.band;
  assert.ok(drawn.some(([t, x, y]) => t === 'L' && x === B.x + 30 - 10 && y === B.portraitY - 50), 'the layer lands at the cell anchor plus its offsets');
});
