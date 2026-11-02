// Row 6 (phones): on a browser with no showOpenFilePicker or showDirectoryPicker (Android Chrome), the MPQs a
// player picks through an <input> are copied into OPFS after navigator.storage.persist(), and a later boot reopens
// the copies through the remembered-handle path. Faked here: navigator.storage, an OPFS directory tree, IndexedDB
// and the file input. Without OPFS, or with a picker, the old path runs unchanged.
import test from 'node:test';
import assert from 'node:assert/strict';

const bytesOf = (s) => new TextEncoder().encode(s);
const text = async (f) => new TextDecoder().decode(new Uint8Array(await f.arrayBuffer()));

// ---- fakes -----------------------------------------------------------------------------------------------
function quota(name) { const e = new Error(`${name} quota`); e.name = 'QuotaExceededError'; return e; }

/** An OPFS directory: entries are { bytes } files; `log` records the order of calls; `failWrite` names a file that cannot be written. */
function fakeOpfs({ failWrite = null } = {}) {
  const log = [], files = new Map();
  class FileHandle {
    constructor(name) { this.kind = 'file'; this.name = name; }
    async getFile() { if (!files.has(this.name)) throw new Error('gone'); return new File([files.get(this.name)], this.name); }
    async createWritable() {
      log.push('write:' + this.name);
      if (this.name === failWrite) throw quota(this.name);
      const parts = [];
      return {
        write: async (b) => { parts.push(new Uint8Array(await b.arrayBuffer())); },
        close: async () => { files.set(this.name, Buffer.concat(parts)); log.push('close:' + this.name); },
        abort: async () => {},
      };
    }
  }
  const dir = {
    async getFileHandle(name, { create } = {}) {
      if (!files.has(name)) { if (!create) throw new Error('NotFound'); files.set(name, new Uint8Array()); }
      return new FileHandle(name);
    },
    async removeEntry(name) { files.delete(name); log.push('remove:' + name); },
    async *entries() { for (const n of [...files.keys()]) yield [n, new FileHandle(n)]; },
  };
  const root = { getDirectoryHandle: async (name) => { log.push('dir:' + name); return dir; } };
  return { root, dir, log, files, FileHandle };
}

function setGlobals({ opfs = null, persist = true, picker = false, remembered = [] } = {}) {
  const log = opfs ? opfs.log : [];
  globalThis.window = globalThis;
  delete globalThis.showOpenFilePicker; delete globalThis.showDirectoryPicker;
  if (picker) { globalThis.showOpenFilePicker = async () => { log.push('picker'); return []; }; }
  globalThis.FileSystemFileHandle = class {};
  const storage = opfs ? {
    getDirectory: async () => { log.push('getDirectory'); return opfs.root; },
    ...(persist ? { persist: async () => { log.push('persist'); return true; } } : {}),
  } : undefined;
  Object.defineProperty(globalThis, 'navigator', { value: { storage }, configurable: true, writable: true });
  // the file input: click() hands `files` to onchange on the next tick
  globalThis.document = {
    createElement() {
      return { click() { this.files = globalThis.__pickedFiles || []; setTimeout(() => this.onchange && this.onchange(), 0); } };
    },
  };
  const db = new Map([['set', remembered]]);
  globalThis.indexedDB = {
    open() {
      const req = { result: { createObjectStore() {}, transaction(store) {
        const tx = { objectStore: () => ({ get: () => ({ result: db.get('set') }), put: (v, k) => db.set(k, v), delete: (k) => db.delete(k) }) };
        setTimeout(() => tx.oncomplete && tx.oncomplete(), 0);
        return tx;
      } } };
      queueMicrotask(() => { req.onupgradeneeded && req.onupgradeneeded(); req.onsuccess && req.onsuccess(); });
      return req;
    },
  };
}

const picked = () => [new File([bytesOf('the data')], 'd2data.mpq'), new File([bytesOf('the expansion')], 'd2exp.mpq')];

// ---- tests -----------------------------------------------------------------------------------------------
test('no OPFS: pickFiles hands back the picked Files exactly as before (no persist, no copy)', async () => {
  setGlobals({ opfs: null });
  globalThis.__pickedFiles = picked();
  const { pickFiles } = await import('../play/assets/byo-source.mjs?no-opfs');
  const out = await pickFiles({ progress: () => assert.fail('no progress without OPFS') });
  assert.deepEqual(out.handles, []);
  assert.equal(out.files.length, 2);
  assert.equal(out.files[0], globalThis.__pickedFiles[0]);
});

test('OPFS, no picker: persist() first, then each file copied into d2-byo/, its handle returned', async () => {
  const opfs = fakeOpfs();
  setGlobals({ opfs });
  globalThis.__pickedFiles = picked();
  const { pickFiles } = await import('../play/assets/byo-source.mjs?opfs-copy');
  const progress = [];
  const out = await pickFiles({ progress: (s) => progress.push(s) });
  assert.deepEqual(opfs.log.slice(0, 3), ['persist', 'getDirectory', 'dir:d2-byo']);
  assert.deepEqual(opfs.log.slice(3), ['write:d2data.mpq', 'close:d2data.mpq', 'write:d2exp.mpq', 'close:d2exp.mpq']);
  assert.deepEqual(out.files, []);
  assert.deepEqual(out.handles.map((h) => h.name), ['d2data.mpq', 'd2exp.mpq']);
  assert.equal(await text(await out.handles[0].getFile()), 'the data');
  assert.equal(await text(await out.handles[1].getFile()), 'the expansion');
  assert.deepEqual(progress, ['Copying your files to this browser: 38%', 'Copying your files to this browser: 100%']);
});

test('a browser that refuses persist() still gets the copy', async () => {
  const opfs = fakeOpfs();
  setGlobals({ opfs, persist: false });
  globalThis.__pickedFiles = picked();
  const { pickFiles } = await import('../play/assets/byo-source.mjs?no-persist');
  const out = await pickFiles();
  assert.equal(out.handles.length, 2);
  assert.ok(!opfs.log.includes('persist'));
});

test('a picker present (desktop Chromium): the picker runs and OPFS is never touched', async () => {
  const opfs = fakeOpfs();
  setGlobals({ opfs, picker: true });
  globalThis.__pickedFiles = picked();
  const { pickFiles } = await import('../play/assets/byo-source.mjs?picker');
  const out = await pickFiles();
  assert.deepEqual(out, { handles: [], files: [] });
  assert.deepEqual(opfs.log, ['picker']);
});

test('a copy the browser refuses keeps that file as a File for this visit; the rest are kept', async () => {
  const opfs = fakeOpfs({ failWrite: 'd2exp.mpq' });
  setGlobals({ opfs });
  globalThis.__pickedFiles = picked();
  const { pickFiles } = await import('../play/assets/byo-source.mjs?quota');
  const out = await pickFiles({ progress: () => {} });
  assert.deepEqual(out.handles.map((h) => h.name), ['d2data.mpq']);
  assert.deepEqual(out.files.map((f) => f.name), ['d2exp.mpq']);
  assert.equal(await text(out.files[0]), 'the expansion');
  assert.equal(opfs.files.has('d2exp.mpq'), false, 'the failed copy is removed');
});

test('OPFS that will not open (getDirectory rejects): the Files pass through as before', async () => {
  const opfs = fakeOpfs();
  setGlobals({ opfs });
  globalThis.navigator.storage.getDirectory = async () => { throw new Error('SecurityError'); };
  globalThis.__pickedFiles = picked();
  const { pickFiles } = await import('../play/assets/byo-source.mjs?no-dir');
  const out = await pickFiles();
  assert.deepEqual(out.handles, []);
  assert.equal(out.files.length, 2);
});

test('a later boot: remembered OPFS handles reopen with no permission prompt and feed the collection', async () => {
  const opfs = fakeOpfs();
  setGlobals({ opfs });
  globalThis.__pickedFiles = picked();
  const { pickFiles, restore, createCollection } = await import('../play/assets/byo-source.mjs?later');
  const { handles } = await pickFiles();
  // The collection remembers them (remember() is what the door calls with the accepted handles).
  setGlobals({ opfs, remembered: handles });
  const again = await restore(false);
  assert.equal(again.count, 2);
  assert.equal(again.handles.length, 2, 'OPFS handles have no queryPermission and are always readable');
  const col = createCollection();
  await col.add({ handles: again.handles });
  assert.deepEqual(col.entries.map((e) => e.file.name), ['d2data.mpq', 'd2exp.mpq']);
  assert.equal(await text(col.entries[0].file), 'the data');
});

test('desktop-style remembered handles keep their permission check (a denied one is dropped)', async () => {
  setGlobals({ opfs: null });
  const denied = { kind: 'file', name: 'd2data.mpq', queryPermission: async () => 'denied', requestPermission: async () => 'denied' };
  setGlobals({ opfs: null, remembered: [denied] });
  const { restore } = await import('../play/assets/byo-source.mjs?denied');
  const r = await restore(true);
  assert.equal(r.count, 1);
  assert.deepEqual(r.handles, []);
});
