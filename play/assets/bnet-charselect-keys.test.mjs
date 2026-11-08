// Realm character select: Enter is OK, Escape is EXIT, and a dead Hardcore character is not refused in realm mode.
// Needs the retail archives (skipped without).
import test from 'node:test';
import assert from 'node:assert/strict';
import { openRetail } from '../../tools/dcc-decode-bench.mjs';
import { decodeDc6, decodeDatPalette } from './d2-formats.mjs';
import { createCharacterSelect } from './charselect.mjs';

let rd = null;
try { rd = openRetail(); } catch { /* no archives */ }
globalThis.ImageData ??= class { constructor(data, w, h) { Object.assign(this, { data, width: w, height: h }); } };
const ctx = new Proxy({}, { get: (t, k) => (k in t ? t[k] : (t[k] = () => ({ data: new Uint8ClampedArray(4) }))), set: (t, k, v) => { t[k] = v; return true; } });
const documentObject = { createElement: () => ({ width: 1, height: 1, getContext: () => ctx }) };
const build = (realm, log) => createCharacterSelect({
  read: rd, decodeDc6, decodeDatPalette, documentObject, realm: () => realm,
  characters: [{ name: 'Ghost', className: 'Paladin', level: 5, hardcore: true, dead: true }],
  onPlay: (n) => log.push(['play', n]), onPlayRefused: (r) => log.push(['refused', r.stringId]),
  onCreate: () => log.push(['create']), onCancel: () => log.push(['exit']),
});
test('Enter plays, Escape exits', { skip: !rd }, () => {
  const log = [], s = build(true, log);
  s.selectByName('Ghost');
  assert.equal(s.key('Enter'), 'play');
  assert.equal(s.key('Escape'), 'exit');
  assert.equal(s.key('KeyA'), null);
  assert.deepEqual(log, [['play', 'Ghost'], ['exit']]);
});
test('dead Hardcore character: refused offline, not in a realm', { skip: !rd }, () => {
  const off = [], on = [];
  const a = build(false, off); a.selectByName('Ghost'); assert.equal(a.play(), false);
  const b = build(true, on); b.selectByName('Ghost'); assert.equal(b.play(), true);
  assert.deepEqual(off, [['refused', 0x14B8]]);
  assert.deepEqual(on, [['play', 'Ghost']]);
});
