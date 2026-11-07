import test from 'node:test';
import assert from 'node:assert/strict';
import { createBnetLobbyScreen, characterInfoLines, difficultiesAllowed, gameErrorText, BNET_LOBBY_LAYOUT } from './bnet-lobby-screen.mjs';

// No canvas in node: a context that swallows every call, and one-pixel art for every file.
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) };
const frame = { width: 1, height: 1, offsetX: 0, offsetY: 0, indices: new Uint8Array(1), opaque: new Uint8Array([1]) };
const strings = { 4057: 'Level', 4011: 'Amazon', 4009: 'Necromancer', 11124: 'Last Difficulty Setting Completed:', 11125: 'None', 5156: 'Normal', 5155: 'Nightmare', 5154: 'Hell', 5138: 'A Game Already Exists With That Name' };
const screen = (character, hooks = {}) => createBnetLobbyScreen({ read: (path) => (path.endsWith('.tbl') ? tbl : new Uint8Array(1 << 22)), decodeDc6: () => ({ frames: [frame, frame, frame, frame] }),
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

test('difficulty gating: Nightmare after 4 (classic) / 5 (expansion), Hell after 8 / 10', () => {
  assert.deepEqual(difficultiesAllowed({ progression: 0 }), [true, false, false]);
  assert.deepEqual(difficultiesAllowed({ progression: 4 }), [true, true, false]);
  assert.deepEqual(difficultiesAllowed({ progression: 4, expansion: true }), [true, false, false]);
  assert.deepEqual(difficultiesAllowed({ progression: 10, expansion: true }), [true, true, true]);
});

test('MCP rejections map to the D2Lang notice', () => {
  assert.equal(gameErrorText({ mcpStage: 'create', mcpResult: 0x1f, message: 'x' }, (i) => strings[i]), 'A Game Already Exists With That Name');
  assert.equal(gameErrorText(new Error('socket closed'), () => ''), 'socket closed');
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
  assert.deepEqual(out, ['y']); assert.deepEqual(s.evidence.chat, ['<Ama> hi']);
  s.chatLine({ text: '<Bob> yo', colour: 0 }); assert.equal(s.evidence.chat.at(-1), '<Bob> yo');
  s.activate('whisper'); s.typeKey('KeyB', 'b'); s.typeKey('Enter'); assert.deepEqual(out.at(-1), '/w b');
});
