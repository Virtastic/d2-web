// A .zip the player picks (phones and tablets, where the input has no folder picker) is read in the browser: the
// .mpq/.exe/PlugY entries at any depth come out with their names and bytes, and nothing else does. The zip is built
// here from published fields (local header, central directory, zip64 records), so the fixture is synthetic: no game
// bytes. The .7z path needs libarchive's wasm in a browser and is checked there, not here.
import test from 'node:test';
import assert from 'node:assert/strict';
import zlib from 'node:zlib';
import { unpackArchive, isArchiveName, crc32 } from '../play/assets/byo-archive.mjs';

const WANT = /\.(mpq|exe)$/i;

/** A zip with the given entries. e: { name, data: Buffer, method: 0|8, flags, crc, encrypted }. zip64 writes the 64-bit records. */
function makeZip(entries, { zip64 = false } = {}) {
  const locals = [], dir = [];
  let off = 0;
  for (const e of entries) {
    const name = Buffer.from(e.name, 'utf8');
    const raw = e.method === 8 ? zlib.deflateRawSync(e.data) : e.data;
    const crc = e.crc ?? zlib.crc32(e.data);
    const flags = e.encrypted ? 1 : 0;
    const lh = Buffer.alloc(30);
    lh.writeUInt32LE(0x04034b50, 0); lh.writeUInt16LE(20, 4); lh.writeUInt16LE(flags, 6); lh.writeUInt16LE(e.method, 8);
    lh.writeUInt32LE(crc >>> 0, 14); lh.writeUInt32LE(raw.length, 18); lh.writeUInt32LE(e.data.length, 22);
    lh.writeUInt16LE(name.length, 26);
    const local = Buffer.concat([lh, name, raw]);
    const extra = zip64 ? Buffer.alloc(4 + 24) : Buffer.alloc(0);
    if (zip64) {
      extra.writeUInt16LE(1, 0); extra.writeUInt16LE(24, 2);
      extra.writeBigUInt64LE(BigInt(e.data.length), 4); extra.writeBigUInt64LE(BigInt(raw.length), 12); extra.writeBigUInt64LE(BigInt(off), 20);
    }
    const ch = Buffer.alloc(46);
    ch.writeUInt32LE(0x02014b50, 0); ch.writeUInt16LE(20, 4); ch.writeUInt16LE(20, 6); ch.writeUInt16LE(flags, 8); ch.writeUInt16LE(e.method, 10);
    ch.writeUInt32LE(crc >>> 0, 16);
    ch.writeUInt32LE(zip64 ? 0xFFFFFFFF : raw.length, 20); ch.writeUInt32LE(zip64 ? 0xFFFFFFFF : e.data.length, 24);
    ch.writeUInt16LE(name.length, 28); ch.writeUInt16LE(extra.length, 30); ch.writeUInt16LE(0, 32);
    ch.writeUInt32LE(zip64 ? 0xFFFFFFFF : off, 42);
    dir.push(Buffer.concat([ch, name, extra]));
    locals.push(local);
    off += local.length;
  }
  const body = Buffer.concat(locals), cd = Buffer.concat(dir);
  if (!zip64) {
    const eocd = Buffer.alloc(22);
    eocd.writeUInt32LE(0x06054b50, 0); eocd.writeUInt16LE(entries.length, 8); eocd.writeUInt16LE(entries.length, 10);
    eocd.writeUInt32LE(cd.length, 12); eocd.writeUInt32LE(body.length, 16);
    return Buffer.concat([body, cd, eocd]);
  }
  const z64 = Buffer.alloc(56), loc = Buffer.alloc(20), eocd = Buffer.alloc(22);
  const z64At = body.length + cd.length;
  z64.writeUInt32LE(0x06064b50, 0); z64.writeBigUInt64LE(44n, 4); z64.writeUInt16LE(45, 12); z64.writeUInt16LE(45, 14);
  z64.writeBigUInt64LE(BigInt(entries.length), 24); z64.writeBigUInt64LE(BigInt(entries.length), 32);
  z64.writeBigUInt64LE(BigInt(cd.length), 40); z64.writeBigUInt64LE(BigInt(body.length), 48);
  loc.writeUInt32LE(0x07064b50, 0); loc.writeBigUInt64LE(BigInt(z64At), 8); loc.writeUInt32LE(1, 16);
  eocd.writeUInt32LE(0x06054b50, 0); eocd.writeUInt16LE(0xFFFF, 8); eocd.writeUInt16LE(0xFFFF, 10);
  eocd.writeUInt32LE(0xFFFFFFFF, 12); eocd.writeUInt32LE(0xFFFFFFFF, 16);
  return Buffer.concat([body, cd, z64, loc, eocd]);
}

const archive = (bytes, name = 'game.zip') => new File([bytes], name, { type: 'application/zip' });
const text = async (f) => new TextDecoder().decode(new Uint8Array(await f.arrayBuffer()));

/** Run unpackArchive with a memory `place` that keeps each entry as a File, the way byo-source does without OPFS. */
async function extract(file, wanted = (n) => WANT.test(n)) {
  const got = [];
  await unpackArchive(file, {
    wanted,
    place: async (name, size, open) => {
      const parts = [], r = (await open()).getReader();
      for (;;) { const { done, value } = await r.read(); if (done) break; parts.push(value); }
      got.push(new File(parts, name));
    },
  });
  return got;
}

const bytes = (n, seed) => Buffer.from(Array.from({ length: n }, (_, i) => (i * 31 + seed) & 0xFF));

test('crc32 matches the published check value', () => {
  assert.equal(crc32(new TextEncoder().encode('123456789')), 0xCBF43926);
  const a = new TextEncoder().encode('12345'), b = new TextEncoder().encode('6789');
  assert.equal(crc32(b, crc32(a)), 0xCBF43926);
});

test('the file names the archive path accepts', () => {
  assert.equal(isArchiveName('Diablo.ZIP'), true);
  assert.equal(isArchiveName('d2.7z'), true);
  assert.equal(isArchiveName('d2data.mpq'), false);
  assert.equal(isArchiveName('zip'), false);
});

test('MPQ entries at any depth, in any case, come out with their names and bytes', async () => {
  const patch = bytes(1000, 7), data = bytes(70000, 3), speech = bytes(10, 9), exe = bytes(64, 1);
  const zip = makeZip([
    { name: 'Diablo II/readme.txt', data: Buffer.from('not a game file'), method: 0 },
    { name: 'Diablo II/', data: Buffer.alloc(0), method: 0 },
    { name: 'Diablo II/Data/Patch_D2.MPQ', data: patch, method: 8 },
    { name: 'Diablo II/deep/er/D2Data.mpq', data, method: 0 },
    { name: 'Diablo II/deep/D2Speech.Mpq', data: speech, method: 8 },
    { name: 'setup/Install.EXE', data: exe, method: 0 },
    { name: '__MACOSX/Diablo II/._D2Exp.mpq', data: Buffer.from('shadow'), method: 0 },
  ]);
  const got = await extract(archive(zip));
  assert.deepEqual(got.map((f) => f.name), ['Patch_D2.MPQ', 'D2Data.mpq', 'D2Speech.Mpq', 'Install.EXE']);
  assert.deepEqual(Buffer.from(await got[0].arrayBuffer()), patch);
  assert.deepEqual(Buffer.from(await got[1].arrayBuffer()), data);
  assert.deepEqual(Buffer.from(await got[2].arrayBuffer()), speech);
  assert.deepEqual(Buffer.from(await got[3].arrayBuffer()), exe);
});

test('a deflated entry larger than one chunk streams in pieces and arrives whole', async () => {
  const big = bytes(300000, 5);
  const zip = makeZip([{ name: 'd2exp.mpq', data: big, method: 8 }]);
  let chunks = 0;
  await unpackArchive(archive(zip), {
    wanted: (n) => WANT.test(n),
    place: async (name, size, open) => {
      const r = (await open()).getReader(), parts = [];
      for (;;) { const { done, value } = await r.read(); if (done) break; chunks++; parts.push(value); }
      assert.equal(size, big.length);
      assert.deepEqual(Buffer.concat(parts), big);
    },
  });
  assert.ok(chunks > 1, `expected several chunks, got ${chunks}`);
});

test('zip64 directory records (archives of 4 GiB and more) read the same way', async () => {
  const d = bytes(5000, 11);
  const zip = makeZip([
    { name: 'a/readme.txt', data: Buffer.from('x'), method: 0 },
    { name: 'big/d2sfx.mpq', data: d, method: 8 },
    { name: 'big/d2char.mpq', data: bytes(77, 2), method: 0 },
  ], { zip64: true });
  const got = await extract(archive(zip));
  assert.deepEqual(got.map((f) => f.name), ['d2sfx.mpq', 'd2char.mpq']);
  assert.deepEqual(Buffer.from(await got[0].arrayBuffer()), d);
});

test('an entry whose bytes do not match its directory CRC is skipped; the others still come out', async () => {
  const good = bytes(400, 4);
  const zip = makeZip([
    { name: 'bad/d2music.mpq', data: bytes(400, 8), method: 8, crc: 0x12345678 },
    { name: 'd2video.mpq', data: good, method: 8 },
  ]);
  const got = await extract(archive(zip));
  assert.deepEqual(got.map((f) => f.name), ['d2video.mpq']);
  assert.deepEqual(Buffer.from(await got[0].arrayBuffer()), good);
});

test('an encrypted entry is skipped, not guessed at', async () => {
  const zip = makeZip([{ name: 'd2data.mpq', data: bytes(50, 1), method: 0, encrypted: true }]);
  assert.deepEqual(await extract(archive(zip)), []);
});

test('without DecompressionStream the stored entries still come out and the deflated ones are skipped', async () => {
  const saved = globalThis.DecompressionStream;
  delete globalThis.DecompressionStream;
  try {
    const stored = bytes(64, 6);
    const zip = makeZip([{ name: 'd2data.mpq', data: bytes(64, 2), method: 8 }, { name: 'd2char.mpq', data: stored, method: 0 }]);
    const got = await extract(archive(zip));
    assert.deepEqual(got.map((f) => f.name), ['d2char.mpq']);
    assert.deepEqual(Buffer.from(await got[0].arrayBuffer()), stored);
  } finally { globalThis.DecompressionStream = saved; }
});

test('an archive with no game file gives nothing; so does a file that is not a zip', async () => {
  const none = makeZip([{ name: 'notes.txt', data: Buffer.from('nothing here'), method: 0 }]);
  assert.deepEqual(await extract(archive(none)), []);
  assert.deepEqual(await extract(archive(Buffer.from('MPQ\x1a this is not a zip at all'), 'd2.zip')), []);
  assert.deepEqual(await extract(archive(Buffer.alloc(0), 'empty.zip')), []);
});

test('a place() that fails skips only its own entry', async () => {
  const zip = makeZip([{ name: 'd2data.mpq', data: bytes(20, 1), method: 0 }, { name: 'd2char.mpq', data: bytes(20, 2), method: 0 }]);
  const seen = [];
  await unpackArchive(archive(zip), {
    wanted: (n) => WANT.test(n),
    place: async (name) => { seen.push(name); if (name === 'd2data.mpq') throw new Error('disk full'); },
  });
  assert.deepEqual(seen, ['d2data.mpq', 'd2char.mpq']);
});

test('a name the caller does not want is never opened', async () => {
  const zip = makeZip([{ name: 'd2data.mpq', data: bytes(20, 1), method: 0 }]);
  const seen = [];
  await unpackArchive(archive(zip), { wanted: () => false, place: async (name) => { seen.push(name); } });
  assert.deepEqual(seen, []);
});

// ---- the picker: a zip the player picks goes into the same path as an .mpq they pick -------------------------
// Faked: the file input (click() hands `files` to onchange), navigator.storage with an OPFS tree, and no pickers,
// as on Android Chrome. Without storage the archive's entries are kept as Files for this visit.

function fakeOpfs() {
  const node = () => ({ dirs: new Map(), files: new Map() });
  const dirHandle = (n) => ({
    async getDirectoryHandle(name, { create } = {}) {
      if (!n.dirs.has(name)) { if (!create) throw new Error('NotFound'); n.dirs.set(name, node()); }
      return dirHandle(n.dirs.get(name));
    },
    async getFileHandle(name, { create } = {}) {
      if (!n.files.has(name)) { if (!create) throw new Error('NotFound'); n.files.set(name, Buffer.alloc(0)); }
      return fileHandle(n, name);
    },
    async removeEntry(name) { n.files.delete(name); n.dirs.delete(name); },
  });
  const fileHandle = (n, name) => ({
    kind: 'file', name,
    async getFile() { return new File([n.files.get(name)], name); },
    async createWritable() {
      const parts = [];
      return {
        async write(b) { parts.push(Buffer.from(b instanceof Blob ? await b.arrayBuffer() : b)); },
        async close() { n.files.set(name, Buffer.concat(parts)); },
        async abort() {},
      };
    },
  });
  const root = node();
  return { root, handle: dirHandle(root) };
}

function setPicked(files, { opfs = null } = {}) {
  globalThis.window = globalThis;
  delete globalThis.showOpenFilePicker; delete globalThis.showDirectoryPicker;
  globalThis.FileSystemFileHandle = class {};
  Object.defineProperty(globalThis, 'navigator', {
    value: opfs ? { storage: { persist: async () => true, getDirectory: async () => opfs.handle } } : {},
    configurable: true, writable: true,
  });
  globalThis.document = {
    createElement() { return { click() { this.files = files; setTimeout(() => this.onchange && this.onchange(), 0); } }; },
  };
}

test('pickerless with OPFS: a zip\'s MPQ is copied into its own folder and handed on as a handle, like a picked .mpq', async () => {
  const d2data = bytes(2000, 21), d2exp = bytes(900, 22);
  const zip = makeZip([
    { name: 'Diablo II/Data/D2Data.MPQ', data: d2data, method: 8 },
    { name: 'Diablo II/readme.txt', data: Buffer.from('hi'), method: 0 },
  ]);
  const opfs = fakeOpfs();
  setPicked([archive(zip, 'Diablo II.zip'), new File([d2exp], 'd2exp.mpq')], { opfs });
  const { pickFiles } = await import('../play/assets/byo-source.mjs');
  const got = await pickFiles();
  assert.equal(got.files.length, 0);
  assert.equal(got.handles.length, 2);
  assert.deepEqual(Buffer.from(await got.handles[0].getFile().then((f) => f.arrayBuffer())), d2data);
  assert.equal(got.handles[0].name, 'D2Data.MPQ');
  assert.deepEqual(Buffer.from(await got.handles[1].getFile().then((f) => f.arrayBuffer())), d2exp);
  // the archive's entry sits in its own folder, so it cannot overwrite a picked file of the same name
  const d2 = opfs.root.dirs.get('d2-byo');
  assert.ok(d2.dirs.get('archive-0').files.has('D2Data.MPQ'));
  assert.ok(d2.files.has('d2exp.mpq'));
});

test('without OPFS: the zip\'s MPQ is kept as a File, and a zip with no game file adds nothing', async () => {
  const zip = makeZip([{ name: 'x/D2Char.mpq', data: bytes(30, 5), method: 0 }]);
  setPicked([archive(zip, 'a.zip')]);
  const { pickFiles } = await import('../play/assets/byo-source.mjs');
  const got = await pickFiles();
  assert.deepEqual(got.handles, []);
  assert.deepEqual(got.files.map((f) => f.name), ['D2Char.mpq']);
  assert.deepEqual(Buffer.from(await got.files[0].arrayBuffer()), bytes(30, 5));

  setPicked([archive(makeZip([{ name: 'readme.txt', data: Buffer.from('x'), method: 0 }]), 'empty.zip')]);
  const none = await pickFiles();
  assert.deepEqual(none, { handles: [], files: [] });
});

test('desktop picker with OPFS: a zip\'s MPQ is still copied to OPFS, so it is remembered across visits', async () => {
  const d2data = bytes(1500, 31);
  const zip = makeZip([{ name: 'Diablo II/Data/D2Data.MPQ', data: d2data, method: 8 }]);
  const opfs = fakeOpfs();
  setPicked([], { opfs });
  const zipFile = archive(zip, 'Diablo II.zip');
  globalThis.showOpenFilePicker = async () => [{ name: 'Diablo II.zip', kind: 'file', getFile: async () => zipFile }];
  const { pickFiles } = await import('../play/assets/byo-source.mjs');
  const got = await pickFiles();
  delete globalThis.showOpenFilePicker;
  assert.equal(got.files.length, 0, 'nothing is left as a session-only File');
  assert.equal(got.handles.length, 1);
  assert.deepEqual(Buffer.from(await got.handles[0].getFile().then((f) => f.arrayBuffer())), d2data);
  assert.ok(opfs.root.dirs.get('d2-byo').dirs.get('archive-0').files.has('D2Data.MPQ'));
});
