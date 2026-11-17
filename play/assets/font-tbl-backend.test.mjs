import test from 'node:test';
import assert from 'node:assert/strict';
import { decodeFontTbl, setFontTableBackend } from './frontend.mjs';

function tbl(codes) {
  const b = new Uint8Array(12 + codes.length * 14);
  b.set([0x57, 0x6f, 0x6f, 0x21]); b[8] = codes.length & 255; b[9] = codes.length >> 8; b[10] = 16; b[11] = 9;
  codes.forEach((c, i) => { const o = 12 + i * 14; b[o] = c & 255; b[o + 1] = c >> 8; b[o + 3] = 5 + i; b[o + 4] = 16; b[o + 8] = i; });
  return b;
}

test('the engine-decoded font table equals the JavaScript parse', () => {
  const bytes = tbl([32, 65, 66, 300]);
  setFontTableBackend(null);
  const js = decodeFontTbl(bytes);
  // A JS stand-in for d2_win_font_tbl_decode: the 5-byte records of the 14-byte file records.
  const engine = (b) => {
    const n = b[8] | (b[9] << 8), out = new Uint8Array(n * 5);
    for (let i = 0; i < n; i++) { const o = 12 + i * 14; out.set([b[o], b[o + 1], b[o + 3], b[o + 4], b[o + 8]], i * 5); }
    return out;
  };
  let called = 0;
  setFontTableBackend((b) => { called++; return engine(b); });
  const viaEngine = decodeFontTbl(bytes);
  setFontTableBackend(null);
  assert.equal(called, 1);
  assert.deepEqual([...viaEngine.chars], [...js.chars]);
  assert.equal(viaEngine.count, js.count);
  assert.deepEqual(viaEngine.header, js.header);
});

test('a backend that declines falls back to the JavaScript parse', () => {
  setFontTableBackend(() => null);
  const t = decodeFontTbl(tbl([65]));
  setFontTableBackend(null);
  assert.equal(t.chars.get(65).width, 5);
});
