// A bring-your-own-files host mounts each archive as a Blob. The background warm was URL-only, so the
// area, character-screen and town warms did nothing there and every byte was a main-thread miss.
import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import vm from 'node:vm';

const SOURCE = readFileSync(new URL('../streamfs.js', import.meta.url), 'utf8');

function page() {
  const sent = [], ref = {};
  const ctx = { window: {}, self: { crossOriginIsolated: true }, SharedArrayBuffer, Blob, performance,
    URL: { createObjectURL(blob) { ref.source = blob.text(); return 'blob:test'; } },
    Worker: class { constructor() { ref.worker = this; } postMessage(m) { sent.push(m); } } };
  vm.runInNewContext(SOURCE, ctx);
  ctx.window.StreamFS.init();
  return { S: ctx.window.StreamFS, sent, ref };
}

test('a warm of a Blob mount slices the Blob in the worker and posts the chunk, with no fetch', async () => {
  const { ref } = page();
  const posted = [];
  const w = { fetch() { throw new Error('a Blob warm must not fetch'); }, TypeError, setTimeout, console, postMessage: (m) => posted.push(m) };
  vm.runInNewContext(await ref.source, w);
  await w.onmessage({ data: { init: 1, ctrl: new SharedArrayBuffer(8), data: new SharedArrayBuffer(16) } });
  await w.onmessage({ data: { mountBlob: { id: 3, blob: new Blob([Uint8Array.of(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)]) } } });
  await w.onmessage({ data: { warm: 1, key: 'id3:4', id: 3, start: 4, end: 8 } });
  assert.equal(posted.length, 2);   // ready, then the chunk
  assert.equal(posted[1].warmed, 'id3:4');
  assert.deepEqual(Array.from(posted[1].buf), [4, 5, 6, 7]);
});

test('a registered Blob is warmable and a read after the warm landed is a hit, not a miss', async () => {
  const { S, sent, ref } = page();
  const blob = new Blob([new Uint8Array(100).map((_, i) => i)]);
  const m = S.registerBlob('/retail/d2data.mpq', blob, 100);
  assert.equal(S.registerBlob('/retail/d2data.mpq', blob, 100), m);   // the mount reuses it: one id, one cache key space
  assert.equal(sent.filter((x) => x.mountBlob).length, 1);
  const done = S.fetched('/retail/d2data.mpq', 10, 20);
  const warm = sent.find((x) => x.warm);
  assert.deepEqual({ key: warm.key, id: warm.id, start: warm.start, end: warm.end }, { key: m.abs + ':0', id: m.src.id, start: 0, end: 100 });
  ref.worker.onmessage({ data: { warmed: warm.key, buf: new Uint8Array(100).map((_, i) => i) } });
  await done;
  const before = S.stats().misses;
  assert.deepEqual(Array.from(S.readAt('/retail/d2data.mpq', 10, 4)), [10, 11, 12, 13]);
  assert.equal(S.stats().misses, before);
  assert.equal(S.stats().warmed, 1);
});

test('control: a Blob that was never registered is not warmed', () => {
  const { S, sent } = page();
  S.warm('/retail/none.mpq', 0, 10);
  assert.equal(sent.filter((x) => x.warm).length, 0);
});
