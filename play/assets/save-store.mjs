// Where characters live between sessions.
//
// The engine produces a real retail .d2s; this stores those bytes in the
// browser and reads back enough of the header to list them. Nothing here reinterprets the file --
// the header fields it parses are the ones retail put at fixed offsets, and the bytes are handed
// back to the engine verbatim.
//
// IndexedDB rather than localStorage: a .d2s is binary and grows with the character, and
// localStorage would force a base64 round trip through a string quota measured in a few megabytes.

const DB_NAME = 'd2-web-saves';
const DB_VERSION = 1;
const STORE = 'characters';

// Fixed header offsets
const OFF_MAGIC = 0, OFF_VERSION = 4, OFF_SIZE = 8, OFF_NAME = 20, OFF_CLASS = 40, OFF_LEVEL = 43;
export const D2S_MAGIC = 0xaa55aa55;

// CharStats.txt's seven-row order, which is also the engine's class index.
export const CLASS_NAMES = ['Amazon', 'Sorceress', 'Necromancer', 'Paladin', 'Barbarian', 'Druid', 'Assassin'];

/**
 * Read the listing fields out of a .d2s header. Returns null if the bytes are not a save file --
 * a caller listing a store should skip those rather than trusting a partial parse.
 */
export function readSaveHeader(bytes) {
  if (!(bytes instanceof Uint8Array) || bytes.length < 48) return null;
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  if (view.getUint32(OFF_MAGIC, true) >>> 0 !== D2S_MAGIC) return null;
  let name = '';
  for (let i = 0; i < 16 && bytes[OFF_NAME + i]; i++) name += String.fromCharCode(bytes[OFF_NAME + i]);
  return {
    name,
    version: view.getUint32(OFF_VERSION, true),
    size: view.getUint32(OFF_SIZE, true),
    playerClass: bytes[OFF_CLASS],
    className: CLASS_NAMES[bytes[OFF_CLASS]] ?? `class ${bytes[OFF_CLASS]}`,
    level: bytes[OFF_LEVEL],
    bytes: bytes.length,
  };
}

function open(indexedDBObject = globalThis.indexedDB) {
  return new Promise((resolve, reject) => {
    if (!indexedDBObject) { reject(new Error('IndexedDB unavailable')); return; }
    const request = indexedDBObject.open(DB_NAME, DB_VERSION);
    request.onupgradeneeded = () => {
      const db = request.result;
      if (!db.objectStoreNames.contains(STORE)) db.createObjectStore(STORE, { keyPath: 'name' });
    };
    request.onsuccess = () => resolve(request.result);
    request.onerror = () => reject(request.error || new Error('IndexedDB open failed'));
  });
}

function run(db, mode, fn) {
  return new Promise((resolve, reject) => {
    const tx = db.transaction(STORE, mode);
    const request = fn(tx.objectStore(STORE));
    tx.oncomplete = () => resolve(request?.result);
    tx.onerror = () => reject(tx.error || new Error('IndexedDB transaction failed'));
    tx.onabort = () => reject(tx.error || new Error('IndexedDB transaction aborted'));
  });
}

/**
 * The character store. Every method rejects rather than swallowing: a save that silently did not
 * happen is the worst outcome here, because the player only finds out when the character is gone.
 */
export function createSaveStore({ indexedDBObject = globalThis.indexedDB } = {}) {
  let dbPromise = null;
  const db = () => (dbPromise ??= open(indexedDBObject));

  return {
    async put(bytes) {
      const header = readSaveHeader(bytes);
      if (!header) throw new Error('refusing to store bytes that are not a .d2s');
      if (!header.name) throw new Error('refusing to store a character with no name');
      // Stored as a plain copy: a view onto wasm memory would be invalidated by the next growth.
      const record = { name: header.name, playerClass: header.playerClass, level: header.level,
        saved: Date.now(), data: Uint8Array.from(bytes) };
      await run(await db(), 'readwrite', (store) => store.put(record));
      return header;
    },
    async get(name) {
      const record = await run(await db(), 'readonly', (store) => store.get(name));
      return record ? Uint8Array.from(record.data) : null;
    },
    async list() {
      const records = await run(await db(), 'readonly', (store) => store.getAll());
      return (records || [])
        .map((record) => ({ ...readSaveHeader(Uint8Array.from(record.data)), saved: record.saved }))
        .filter((header) => header && header.name)
        .sort((a, b) => b.saved - a.saved);
    },
    async remove(name) { await run(await db(), 'readwrite', (store) => store.delete(name)); },
    async clear() { await run(await db(), 'readwrite', (store) => store.clear()); },
  };
}
