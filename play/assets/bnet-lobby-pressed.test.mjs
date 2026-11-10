// Every Battle.net lobby button shows its held (click-down) cel while the mouse is held on it, with the label 2 px left and down; the
// pointer leaving it shows the up cel again, and a release outside activates nothing. Draw-list evidence on stand-in art, plus the retail
// DC6s' frame counts when the archives are present (the wide ladder buttons' held pair is frames 2 and 3 of widebuttonblank).
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { parseDrawListVector } from './drawlist.mjs';
import { openRetail } from '../../tools/dcc-decode-bench.mjs';
import { decodeDc6 as decodeRetailDc6 } from './d2-formats.mjs';
import { createBnetLobbyScreen, BNET_LOBBY_LAYOUT as L, BNET_LOBBY_ASSETS as A } from './bnet-lobby-screen.mjs';

const root = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (_, k) => (k === 'getImageData' ? () => ({ data: new Uint8ClampedArray(4) }) : () => {}), set: () => true });
const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ctx }) };
const cel = (width, height) => ({ width, height, offsetX: 0, offsetY: 0, indices: new Uint8Array(width * height), opaque: new Uint8Array(width * height).fill(1) });
// widebuttonblank is 256 + 16 wide, twice; everything else a stand-in with enough frames for a held cel.
const WIDE = new Uint8Array(1 << 22), PLAIN = new Uint8Array(1 << 22);
const decodeDc6 = (bytes) => ({ frames: Array.from({ length: 8 }, (_, i) => cel(bytes === WIDE ? (i % 2 ? 16 : 256) : 20, 20)) });
const tbl = new Uint8Array(12 + 95 * 14); tbl.set([0x57, 0x6f, 0x6f, 0x21]); tbl[10] = 10;
for (let c = 32; c < 127; c++) { const b = 12 + (c - 32) * 14; tbl[b] = c; tbl[b + 3] = 6; tbl[b + 4] = 10; }
const strings = new Proxy({}, { get: (_, k) => (/^\d+$/.test(k) ? 'LABEL' : undefined) });
const make = (hooks = {}) => createBnetLobbyScreen({ read: (p) => (p.endsWith('.tbl') ? tbl : /widebutton/i.test(p) ? WIDE : PLAIN),
  decodeDc6, decodeDatPalette: () => new Uint8Array(1024), string: (i) => strings[i], character: { name: 'Ama', charClass: 0, level: 3 },
  documentObject, now: () => 0, ...hooks });
const draw = (s) => { const out = []; globalThis.__d2DrawList = out; try { s.draw(ctx, 800, 600); } finally { delete globalThis.__d2DrawList; } return out; };

const STAT = `PX2DUSEast,Bob,${String.fromCharCode(...Uint8Array.from({ length: 30 }, (_, i) => (i === 0 || i === 1 ? 1 : i === 2 ? 1 : i === 13 ? 40 : 0xff)))}`;
const buttons = new Map();
(function walk(o) { if (o && typeof o === 'object') { if (o.type === 'button') buttons.set(o.id, o); else for (const v of Object.values(o)) walk(v); } })(L);
const base = (path) => path.split(/[\\/]/).pop().toLowerCase().replace(/\.dc6$/, '');
// The records of a button's own art: [{frame, x, y}] and the label glyphs' first position.
const artOf = (list, b) => list.filter((r) => r.op === 'cel' && base(r.path) === base(A[b.art]) && r.y === b.y && (r.x === b.x || (b.art === 'wideButton' && r.x === b.x + 256)));
const labelOf = (list, b) => list.filter((r) => /font/i.test(r.path) && r.x >= b.x - 4 && r.x < b.x + b.w && r.y > b.y - b.h && r.y <= b.y + 4 && r.frame !== undefined)[0];

// Each state is a screen with the buttons of one view live; `ids` are the buttons that view owns.
const VIEWS = {
  prechat: { open: (s) => s, ids: ['enter-chat', 'help', 'quit'] },
  chat: { open: (s) => { s.activate('enter-chat'); s.chatEvent({ event: 7, username: '', text: 'Chan', raw: 'Chan' }); s.chatEvent({ event: 1, username: 'Bob', text: STAT, raw: STAT });
    s.typeKey('KeyH', 'h'); s.pickUser('Bob'); return s; }, ids: ['send', 'whisper', 'chat-help', 'squelch', 'emote', 'create', 'join', 'channel', 'ladder', 'quit'] },
  create: { open: (s) => { s.activate('enter-chat'); s.activate('create'); s.typeKey('KeyA', 'a'); return s; }, ids: ['create-game', 'panel-cancel', 'players-up', 'players-down'] },
  join: { open: (s) => { s.activate('enter-chat'); s.activate('join'); s.typeKey('KeyA', 'a'); return s; }, ids: ['join-game', 'panel-cancel'] },
  channel: { open: (s) => { s.activate('enter-chat'); s.setChannels(['A', 'B']); s.activate('channel'); s.typeKey('KeyA', 'a'); return s; }, ids: ['channel-ok', 'panel-cancel'] },
  help: { open: (s) => { s.activate('enter-chat'); s.activate('chat-help'); return s; }, ids: ['help-ok'] },
  ladder: { open: (s) => { s.activate('enter-chat'); s.activate('ladder'); return s; }, ids: ['ladder-standard', 'ladder-hardcore', 'ladder-expansion', 'ladder-expansion-hardcore', 'panel-cancel'] },
  ladderList: { open: (s) => { s.activate('enter-chat'); s.activate('ladder'); s.activate('ladder-standard'); return s; }, ids: ['ladder-list-ok', 'ladder-list-back'], hooks: { onLadder: async () => [] } },
  notice: { open: (s) => { s.activate('enter-chat'); s.activate('create'); s.typeKey('KeyA', 'a'); s.activate('create-game'); return s; }, ids: ['notice-ok'], hooks: { onCreateGame: async () => { throw new Error('refused'); } }, wait: true },
};

for (const [name, v] of Object.entries(VIEWS)) {
  test(`lobby ${name}: held buttons draw the pressed cel and a label 2 px left and down`, async () => {
    const s = make(v.hooks); v.open(s); if (v.wait) await new Promise((r) => setImmediate(r));
    const live = new Set(s.evidence.boxes.filter((b) => !b.disabled).map((b) => b.id));
    for (const id of v.ids) {
      const b = buttons.get(id); assert.ok(b, id); assert.ok(live.has(id), `${name}/${id} is on show and enabled`);
      const [cx, cy] = [b.x + Math.trunc(b.w / 2), b.y - Math.trunc(b.h / 2)];
      s.hover(5, 5, 800, 600); s.pinClock?.(0, 0);
      const idle = draw(s); const wide = b.art === 'wideButton', step = wide ? 2 : 1;
      assert.deepEqual(artOf(idle, b).map((r) => r.frame).sort(), wide ? [b.base, b.base + 1] : [b.base], `${id} idle`);
      s.hover(cx, cy, 800, 600);
      const token = s.press(cx, cy, 800, 600); assert.ok(token, `${name}/${id} takes the press`);
      const down = draw(s);
      const latch = ['create', 'join', 'channel', 'ladder'].includes(id);
      assert.deepEqual(artOf(down, b).map((r) => r.frame).sort(), wide ? [b.base + 2, b.base + 3] : [b.base + 1], `${name}/${id} held cel`);
      const l0 = labelOf(idle, b), l1 = labelOf(down, b);
      if (l0 && l1) assert.deepEqual([l1.x - l0.x, l1.y - l0.y], [-2, 2], `${name}/${id} label shift`);
      // Pointer leaves the button while held: the up cel again (a latching button stays down while its panel is open).
      if (!latch) {
        s.hover(5, 5, 800, 600);
        assert.deepEqual(artOf(draw(s), b).map((r) => r.frame).sort(), wide ? [b.base, b.base + 1] : [b.base], `${name}/${id} pointer off`);
        assert.equal(s.release(token, 5, 5, 800, 600), false, `${name}/${id} release outside activates nothing`);
        assert.deepEqual(artOf(draw(s), b).map((r) => r.frame).sort(), wide ? [b.base, b.base + 1] : [b.base], `${name}/${id} released outside`);
      }
      s.cancel();
    }
  });
}

test('lobby: press then release inside a button activates it once, press elsewhere then release on it does not', () => {
  const calls = [];
  const s = make({ onQuit: () => calls.push('quit') }); const q = buttons.get('quit'); const [cx, cy] = [q.x + 10, q.y - 5];
  s.hover(cx, cy, 800, 600); assert.equal(s.press(cx, cy, 800, 600), 'quit'); assert.equal(s.release('quit', cx, cy, 800, 600), true);
  s.activate('enter-chat');
  s.hover(5, 5, 800, 600); const none = s.press(5, 5, 800, 600); s.hover(cx, cy, 800, 600);
  assert.equal(s.release(none, cx, cy, 800, 600), false);
});

test('the retail art holds the pressed frames the lobby draws', { skip: (() => { try { return !openRetail(); } catch { return true; } })() }, () => {
  const rd = openRetail();
  for (const b of buttons.values()) {
    if (!b.art || !A[b.art] || /^(difficulty|level-box)/.test(b.id)) continue;
    const n = decodeRetailDc6(rd(A[b.art])).frames.length;
    assert.ok(n > b.base + (b.art === 'wideButton' ? 3 : 1), `${b.id}: ${A[b.art]} has ${n} frames, base ${b.base}`);
  }
});

// The same looks against the retail captures (vectors/drawlist/bnet-press-*.txt, bnet-prechat-press-*.txt: the mouse held down on one button of the
// headless front end, the draw list of the frame). Each vector holds the pressed button's own cel at the held frame (base + 1, a wide button base + 2 and + 3),
// a dim button the idle frame; the retail test file (bnet-lobby-retail.test.mjs) proves the whole frame, label shift included, record for record.
const PRESSED = [['press-quit', 'quit', true], ['press-send', 'send', true], ['press-whisper', 'whisper', true], ['press-squelch', 'squelch', true],
  ['press-unsquelch', 'unsquelch', true], ['press-sq-whisper', 'whisper', true], ['press-emote', 'emote', true], ['press-help', 'chat-help', true],
  ['press-channel', 'channel', true], ['press-ladder', 'ladder', true], ['press-dim-squelch', 'squelch', false], ['press-dim-unsquelch', 'unsquelch', false],
  ['press-arrow1', 'arrow-1', true], ['press-arrow3', 'arrow-3', true], ['prechat-press-enter', 'enter-chat', true], ['prechat-press-help', 'help', true]];
for (const [file, id, down] of PRESSED) {
  const path = join(root, 'vectors', 'drawlist', `bnet-${file}.txt`);
  test(`retail ${file}: ${id} draws ${down ? 'the held' : 'the idle'} cel`, { skip: !existsSync(path) }, () => {
    const b = buttons.get(id); assert.ok(b, id);
    const wide = b.art === 'wideButton', want = down ? (wide ? [b.base + 2, b.base + 3] : [b.base + 1]) : (wide ? [b.base, b.base + 1] : [b.base]);
    const recs = parseDrawListVector(readFileSync(path, 'utf8')).filter((r) => r.op === 'cel');
    assert.deepEqual(artOf(recs, b).map((r) => r.frame).sort(), want);
  });
}
