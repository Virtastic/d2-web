// A music track read whole was a 1.1 s main-thread freeze (StormLib decompressing 6 MB of sectors). The
// sliced read takes it in parts and yields between them; a build without the part exports reads whole.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createMpqAssetReader } from '../play/assets/mpq-assets.mjs';

function fakeModule(files, { parts = true, failAt = -1 } = {}) {
  const HEAPU8 = new Uint8Array(1 << 16), log = { parts: 0, open: 0, closed: 0, whole: 0 };
  let current = new Uint8Array(), file = null, pos = 0;
  const put = (bytes) => { current = bytes; HEAPU8.set(bytes, 1024); };
  const calls = {
    d2_asset_read: (n) => { log.whole++; if (!files[n]) return 0; put(files[n]); return 1; },
    d2_asset_len: () => current.length,
    d2_asset_ptr: () => 1024n,
    d2_asset_error: () => 'no such asset',
    d2_asset_open: (n) => { if (!files[n]) return 0; log.open++; file = files[n]; pos = 0; return 7; },
    d2_asset_size: (h) => h === 7 ? file.length : 0xFFFFFFFF,
    d2_asset_part: (h, n) => { if (h !== 7 || log.parts++ === failAt || pos + n > file.length) return 0; put(file.subarray(pos, pos += n)); return 1; },
    d2_asset_close: (h) => { if (h === 7) log.closed++; },
  };
  const Module = { HEAPU8, ccall: (name, _r, _t, args) => calls[name](...args) };
  if (parts) Module._d2_asset_open = () => {};
  return { Module, log };
}
const track = Uint8Array.from({ length: 10000 }, (_, i) => i * 7);

test('a sliced read takes the file in parts, yielding between them', async () => {
  const { Module, log } = fakeModule({ 'music\\a.wav': track });
  const pending = createMpqAssetReader(Module).sliced('music\\a.wav', 4096);
  assert.equal(log.parts, 1, 'only the first part before the first yield');
  assert.deepEqual(await pending, track);
  assert.deepEqual(log, { parts: 3, open: 1, closed: 1, whole: 0 });
});

test('a failed part rejects and still closes the file; a missing file rejects', async () => {
  const { Module, log } = fakeModule({ 'music\\a.wav': track }, { failAt: 1 });
  const read = createMpqAssetReader(Module);
  await assert.rejects(read.sliced('music\\a.wav', 4096), /no such asset/);
  assert.equal(log.closed, 1);
  await assert.rejects(read.sliced('music\\none.wav'), /no such asset/);
});

test('without the part exports the sliced read is the whole read', async () => {
  const { Module, log } = fakeModule({ 'music\\a.wav': track }, { parts: false });
  assert.deepEqual(await createMpqAssetReader(Module).sliced('music\\a.wav'), track);
  assert.deepEqual(log, { parts: 0, open: 0, closed: 0, whole: 1 });
});
