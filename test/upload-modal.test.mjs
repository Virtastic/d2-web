// The front door's upload popup (play/assets/upload-modal.mjs) shows each stage of reading the player's files: the folder
// walk, file N of M, an archive being unzipped, a copy into the browser's storage, then done, or an error that stays up
// until it is dismissed. Each case drives the real readers in byo-source.mjs with faked pickers, inputs and storage, and
// asserts the popup's views in order. The zips are built here from published fields: no game bytes.
import test from 'node:test';
import assert from 'node:assert/strict';
import zlib from 'node:zlib';
import { createUploadModal, runUpload, uploadView, uploadOkHit } from '../play/assets/upload-modal.mjs';
import { createCollection, pickFolder, pickFiles } from '../play/assets/byo-source.mjs';

function makeZip(entries) {
  const locals = [], dir = [];
  let off = 0;
  for (const e of entries) {
    const name = Buffer.from(e.name), raw = zlib.deflateRawSync(e.data), crc = zlib.crc32(e.data);
    const lh = Buffer.alloc(30);
    lh.writeUInt32LE(0x04034b50, 0); lh.writeUInt16LE(20, 4); lh.writeUInt16LE(8, 8);
    lh.writeUInt32LE(crc >>> 0, 14); lh.writeUInt32LE(raw.length, 18); lh.writeUInt32LE(e.data.length, 22); lh.writeUInt16LE(name.length, 26);
    const ch = Buffer.alloc(46);
    ch.writeUInt32LE(0x02014b50, 0); ch.writeUInt16LE(20, 4); ch.writeUInt16LE(20, 6); ch.writeUInt16LE(8, 10);
    ch.writeUInt32LE(crc >>> 0, 16); ch.writeUInt32LE(raw.length, 20); ch.writeUInt32LE(e.data.length, 24); ch.writeUInt16LE(name.length, 28);
    ch.writeUInt32LE(off, 42);
    const local = Buffer.concat([lh, name, raw]);
    locals.push(local); dir.push(Buffer.concat([ch, name])); off += local.length;
  }
  const body = Buffer.concat(locals), cd = Buffer.concat(dir), eocd = Buffer.alloc(22);
  eocd.writeUInt32LE(0x06054b50, 0); eocd.writeUInt16LE(entries.length, 8); eocd.writeUInt16LE(entries.length, 10);
  eocd.writeUInt32LE(cd.length, 12); eocd.writeUInt32LE(body.length, 16);
  return Buffer.concat([body, cd, eocd]);
}
const junk = (n, seed) => Buffer.from(Array.from({ length: n }, (_, i) => (i * 31 + seed) & 255));
const file = (name, n, seed = 1) => new File([junk(n, seed)], name);

/** No pickers, no OPFS: the file input hands back `picked`. With `opfs`, navigator.storage opens that directory. */
function setGlobals({ picked = [], opfs = null, folder = null } = {}) {
  globalThis.window = globalThis;
  delete globalThis.showOpenFilePicker; delete globalThis.showDirectoryPicker;
  if (folder) globalThis.showDirectoryPicker = async () => folder;
  globalThis.FileSystemFileHandle = class {};
  Object.defineProperty(globalThis, 'navigator', {
    value: opfs ? { storage: { persist: async () => true, getDirectory: async () => opfs } } : {}, configurable: true, writable: true,
  });
  globalThis.document = { createElement() { return { click() { this.files = picked; setTimeout(() => this.onchange?.(), 0); } }; } };
}
/** A folder handle: `tree` maps a name to a File (a file) or to another tree (a sub-folder). */
const folderHandle = (tree, { deny = false } = {}) => ({
  kind: 'directory',
  async *entries() {
    if (deny) throw Object.assign(new Error('denied'), { name: 'NotAllowedError' });
    for (const [n, v] of Object.entries(tree))
      yield [n, v instanceof File ? { kind: 'file', name: n, getFile: async () => v } : folderHandle(v)];
  },
});
/** A flat OPFS directory that keeps what is written. */
function fakeOpfs() {
  const files = new Map();
  const dir = {
    async getDirectoryHandle() { return dir; },
    async getFileHandle(name) {
      return { kind: 'file', name, getFile: async () => new File([files.get(name) ?? new Uint8Array()], name),
        async createWritable() { const parts = []; return { write: async (b) => { parts.push(new Uint8Array(await b.arrayBuffer())); },
          close: async () => { files.set(name, Buffer.concat(parts)); }, abort: async () => {} }; } };
    },
    async removeEntry(name) { files.delete(name); },
  };
  return dir;
}

// The door's add: the collection reads the files, then reports how many of the eleven game files are in hand.
const doorAdd = (col) => async (src, step) => { await col.add(src, step); return { found: Object.values(col.state.slots).filter((s) => s.have).length, total: 11 }; };
const shown = (modal) => modal.log.map((v) => `${v.phase}|${v.title}|${v.text}`);

test('folder: reading the folder, then each game file in turn, then done', async () => {
  setGlobals({ folder: folderHandle({ 'D2Data.mpq': file('D2Data.mpq', 64), 'readme.txt': file('readme.txt', 4), Data: { 'd2exp.mpq': file('d2exp.mpq', 64, 2) } }) });
  const modal = createUploadModal(), col = createCollection();
  await runUpload(modal, (step) => pickFolder({ step }), doorAdd(col));
  assert.deepEqual(shown(modal), [
    'reading|Reading Files|Reading the folder',
    'processing|Reading Files|Processing file 1 of 2: D2Data.mpq',
    'processing|Reading Files|Processing file 2 of 2: d2exp.mpq',
    'done|Done|0 of 11 files found.',
  ]);
  assert.equal(modal.view.ok, true);
  assert.equal(modal.dismiss(), true);
  assert.equal(modal.open, false);
});

test('folder the browser will not read: reading, then an error that stays until OK', async () => {
  setGlobals({ folder: folderHandle({}, { deny: true }) });
  const modal = createUploadModal();
  await runUpload(modal, (step) => pickFolder({ step }), doorAdd(createCollection()));
  assert.deepEqual(shown(modal), ['reading|Reading Files|Reading the folder', 'error|Error|The browser did not allow reading these files.']);
  assert.equal(uploadOkHit(modal.view, { x: 400, y: 400 }), true, 'OK is centred in the popup');
  assert.equal(uploadOkHit(modal.view, { x: 334, y: 400 }), false);
  assert.equal(uploadOkHit(modal.view, { x: 300, y: 300 }), false);
  assert.equal(modal.dismiss(), true);
  assert.equal(modal.view, null);
});

test('single file: file 1 of 1, then done; a cancelled pick shows nothing', async () => {
  setGlobals({ picked: [file('d2char.mpq', 80)] });
  const modal = createUploadModal();
  await runUpload(modal, (step) => pickFiles({ step }), doorAdd(createCollection()));
  assert.deepEqual(shown(modal), ['processing|Reading Files|Processing file 1 of 1: d2char.mpq', 'done|Done|0 of 11 files found.']);

  setGlobals({ picked: [] });
  const none = createUploadModal();
  await runUpload(none, (step) => pickFiles({ step }), doorAdd(createCollection()));
  assert.deepEqual(none.log, []);
  assert.equal(none.open, false);
});

test('zip: unzipping the archive and each game file inside, then processing them, then done', async () => {
  const zip = makeZip([{ name: 'Diablo II/D2Data.MPQ', data: junk(300, 3) }, { name: 'Diablo II/notes.txt', data: junk(5, 4) },
    { name: 'Diablo II/d2sfx.mpq', data: junk(200, 5) }]);
  setGlobals({ picked: [new File([zip], 'Diablo II.zip')] });
  const modal = createUploadModal();
  await runUpload(modal, (step) => pickFiles({ step }), doorAdd(createCollection()));
  assert.deepEqual(shown(modal), [
    'unzipping|Unzipping|Unzipping Diablo II.zip',
    'unzipping|Unzipping|Unzipping Diablo II.zip: D2Data.MPQ',
    'unzipping|Unzipping|Unzipping Diablo II.zip: d2sfx.mpq',
    'processing|Reading Files|Processing file 1 of 2: D2Data.MPQ',
    'processing|Reading Files|Processing file 2 of 2: d2sfx.mpq',
    'done|Done|0 of 11 files found.',
  ]);
});

test('a folder with no game file in it: reading, then an error naming that', async () => {
  setGlobals({ folder: folderHandle({ 'readme.txt': file('readme.txt', 4) }) });
  const modal = createUploadModal();
  await runUpload(modal, (step) => pickFolder({ step }), doorAdd(createCollection()));
  assert.deepEqual(shown(modal), ['reading|Reading Files|Reading the folder', 'error|Error|No Diablo II files were found in what you chose.']);
});

test('two zips count as archive 1 of 2 and 2 of 2', async () => {
  setGlobals({ picked: [new File([makeZip([{ name: 'a.mpq', data: junk(40, 6) }])], 'one.zip'), new File([makeZip([{ name: 'b.mpq', data: junk(40, 7) }])], 'two.zip')] });
  const modal = createUploadModal();
  await runUpload(modal, (step) => pickFiles({ step }), doorAdd(createCollection()));
  assert.deepEqual(shown(modal).slice(0, 4), [
    'unzipping|Unzipping|Unzipping one.zip (1 of 2)', 'unzipping|Unzipping|Unzipping one.zip (1 of 2): a.mpq',
    'unzipping|Unzipping|Unzipping two.zip (2 of 2)', 'unzipping|Unzipping|Unzipping two.zip (2 of 2): b.mpq',
  ]);
});

test('damaged zip (it reads as empty): unzipping, then an error; OK is ignored while a read runs and closes the error', async () => {
  setGlobals({ picked: [new File([junk(100, 8)], 'broken.zip')] });
  const modal = createUploadModal();
  let busyDismiss = null;
  await runUpload(modal, (step) => { const p = pickFiles({ step }); busyDismiss = modal.dismiss(); return p; }, doorAdd(createCollection()));
  assert.equal(busyDismiss, false, 'a running read cannot be dismissed');
  assert.deepEqual(shown(modal), ['unzipping|Unzipping|Unzipping broken.zip', 'error|Error|No Diablo II files were found in what you chose.']);
  assert.equal(modal.view.busy, undefined);
  assert.equal(modal.dismiss(), true);
  assert.equal(modal.open, false);
});

test('storing: a pickerless browser copies each picked file into its storage, then processes them', async () => {
  const opfs = fakeOpfs();
  setGlobals({ picked: [file('d2data.mpq', 100), file('d2exp.mpq', 300, 2)], opfs });
  const modal = createUploadModal();
  await runUpload(modal, (step) => pickFiles({ step }), doorAdd(createCollection()));
  assert.deepEqual(shown(modal), [
    'storing|Storing Files|Storing d2data.mpq in this browser: 25%',
    'storing|Storing Files|Storing d2exp.mpq in this browser: 100%',
    'processing|Reading Files|Processing file 1 of 2: d2data.mpq',
    'processing|Reading Files|Processing file 2 of 2: d2exp.mpq',
    'done|Done|0 of 11 files found.',
  ]);
});

test('the views: every running stage shows the dots and no OK; done and error show OK and no dots', () => {
  for (const s of [{ kind: 'reading' }, { kind: 'unzip', name: 'a.zip', n: 1, m: 1 }, { kind: 'store', name: 'x', pct: 5 }, { kind: 'process', name: 'x', n: 1, m: 3 }])
    assert.deepEqual([uploadView(s).busy, !!uploadView(s).ok], [true, false], s.kind);
  assert.deepEqual(uploadView({ kind: 'done', found: 11, total: 11 }), { phase: 'done', title: 'Done', ok: true, text: 'All files found.' });
  assert.deepEqual(uploadView({ kind: 'error' }), { phase: 'error', title: 'Error', text: 'These files could not be read.', ok: true });
  assert.throws(() => uploadView({ kind: 'nope' }), /upload step nope/);
});
