import test from 'node:test';
import assert from 'node:assert/strict';
import { createRealmSessionPresenter } from './realm-session-events.mjs';
import { createUnitOverlayFx } from './unit-overlay-fx.mjs';
import { readFileSync } from 'node:fs';

const ev = (op, a = 0, b = 0, c = 0, d = 0, more = {}) => ({ op, a, b, c, d, name1: '', name2: '', ...more });

test('each session event reaches the presenter a local game uses', () => {
  const seen = [];
  const p = createRealmSessionPresenter({
    unitSound: (s) => seen.push(['sound', s.type, s.id, s.sound]),
    line: (text) => seen.push(['line', text]),
    clearOverhead: (type, id) => seen.push(['overhead', type, id]),
    uiSound: (id) => seen.push(['ui', id]),
    gameFlags: (g) => seen.push(['flags', g.difficulty, g.flags, g.expansion, g.ladder]),
    openInventory: () => seen.push(['inventory']),
    recheckItems: (type, id) => seen.push(['items', type, id]),
  });
  const n = p.present([
    ev(0x2c, 1, 77, 0x12), ev(0x5a, 2, 4, 0, 0, { text: 'Hero joined our world. Diablo\'s minions grow stronger.' }),
    ev(0x5a, 7, 1, 0, 0, { text: '' }), ev(0x76, 0, 5), ev(0x88a70, 19), ev(0x01, 1, 0x804, 1, 0),
    ev(0x3f, 0, 9, 0xffff, 1), ev(0x3f, 0xff, 9, 0xffff, 0), ev(0x48, 0, 5), ev(0x42, 0, 5)]);
  assert.equal(n, 10);
  assert.deepEqual(seen, [
    ['sound', 1, 77, 0x12], ['line', 'ÿc4Hero joined our world. Diablo\'s minions grow stronger.'],
    ['overhead', 0, 5], ['ui', 19], ['flags', 1, 0x804, true, false], ['inventory'], ['items', 0, 5]]);
  assert.equal(p.count(0x5a), 2);
});

test('a presenter with no sinks takes every event without failing', () => {
  const p = createRealmSessionPresenter();
  assert.equal(p.present([ev(0x2c, 1, 2, 3), ev(0x5a, 0, 0, 0, 0, { text: 'x' }), ev(0x8a, 1, 2, 0x48)]), 3);
  assert.equal(p.present(null), 0);
  assert.equal(p.overlayCount, 1);
});

test('a townsperson with something to say carries the overlay once, through the local overlay player', () => {
  const head = 'Overlay\tFilename\tFrames\tPreDraw\tTrans\tAnimRate';
  const rows = [head, ...Array.from({ length: 0x49 }, (_, i) => `o${i}\t${i === 0x48 ? 'talk' : 'null'}\t4\t0\t0\t256`)];
  const fx = createUnitOverlayFx(() => new TextEncoder().encode(rows.join('\n')));
  const p = createRealmSessionPresenter();
  p.present([ev(0x8a, 1, 40, 0x48)]);
  const npc = { type: 1, id: 40 }, other = { type: 1, id: 41 };
  assert.equal(p.applyOverlays(fx, 100, [npc, other]), 1);
  assert.equal(npc.overlay.file, 'talk'); assert.equal(npc.overlay.overlay, 0x48); assert.equal(other.overlay, undefined);
  let tick = 100;
  while (p.overlayCount && tick < 400) p.applyOverlays(fx, ++tick, [{ type: 1, id: 40 }]);
  assert.ok(tick < 400, 'the overlay ends');
  assert.equal(p.applyOverlays(fx, tick + 1, [npc]), 0);
});

test('the page takes the session events once per delivery and has no second line composer', () => {
  const shell = readFileSync(new URL('../gameplay-shell.mjs', import.meta.url), 'utf8');
  const frame = shell.slice(shell.indexOf('function feedRealmPlayerFrame('));
  assert.ok(frame.indexOf('presentRealmSession();') > frame.indexOf('feedRealmSocial(bytes);'), 'presented after the packets are applied');
  assert.match(shell, /d2gsClientTakeSessionEvents\?\.\(\)/);
  assert.match(shell, /realmSessionPresenter\.applyOverlays\(unitOverlayFx,/);
  assert.match(shell, /unitSounds\?\.play\(\{ unitType: type, unitId: id, classId, soundEvent: sound/);
  assert.doesNotMatch(shell, /rosterEventLine|tradeEventLine/);
});
