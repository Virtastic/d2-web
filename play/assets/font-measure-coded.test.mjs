import test from 'node:test';
import assert from 'node:assert/strict';
import { createFont, setTextWidthBackend } from './frontend.mjs';

const tbl = { header: { height: 10 }, chars: new Map([...'abcXY: '].map((c, i) => [c.codePointAt(0), { imageIndex: 0, width: 3 + i, height: 8 }])) };
const dc6 = { frames: [{ width: 0, height: 0, offsetX: 0, offsetY: 0 }] };
const doc = { createElement: () => ({}) };
const font = () => createFont({ dc6, tbl, palette: new Uint8Array(1024), documentObject: doc });

// A JS port of D2Win+0x120C0 / 0x12080 standing in for the wasm exports.
const engine = (calls) => (adv, text, coded) => {
  calls.push(coded);
  let w = 0, lim = 3;
  const n = text.length;
  for (let i = 0; i < n; i++, lim++) {
    const ch = text.charCodeAt(i);
    if (ch === 10) continue;
    if (coded && n > lim && ch === 0xff && text[i + 1] === 'c' && text.charCodeAt(i + 2) >= 0x30 && text.charCodeAt(i + 2) <= 0x36) { i += 2; lim += 2; continue; }
    w += adv[ch];
  }
  return w;
};

test('engine colour-code width equals the JS strip for codes 0-6', () => {
  const lines = ['ÿc4abÿc0: XY', 'abc', 'ÿc1aÿc2b', 'ÿc4a'];
  setTextWidthBackend(null);
  const js = lines.map((l) => font().measureCoded(l));
  const calls = [];
  setTextWidthBackend(engine(calls));
  const eng = lines.map((l) => font().measureCoded(l));
  setTextWidthBackend(null);
  assert.deepEqual(eng, js);
  assert.equal(calls.length, lines.length);
  assert.ok(calls.every(Boolean));
});

test('codes outside 0-6 or trailing codes keep the JS strip', () => {
  const calls = [];
  setTextWidthBackend(engine(calls));
  for (const l of ['ÿc8ab', 'ÿc:ab', 'abÿc4']) font().measureCoded(l);
  setTextWidthBackend(null);
  assert.ok(!calls.some(Boolean));
});
