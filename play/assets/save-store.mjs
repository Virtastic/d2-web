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
import { createUnloadSaveJournal } from './unload-save-journal.mjs';

// Fixed header offsets
const OFF_MAGIC = 0, OFF_VERSION = 4, OFF_SIZE = 8, OFF_NAME = 20, OFF_CLASS = 40, OFF_LEVEL = 43, OFF_TOWN = 0xA8;
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
  // nTown[3] (D2SaveHeaderStrc 0xA8): per difficulty, bit 7 = entered, bits 0-2 = the act the
  // character is in -- PLRSAVE2_WriteSaveHeader stores CLIENTS_GetActNo | 0x80 for the game's
  // difficulty (d2s-format "Difficulty"). The highest entered difficulty is the one it plays.
  let difficulty = 0, act0 = 0;
  for (let d = 0; d < 3 && bytes.length > OFF_TOWN + d; d++) if (bytes[OFF_TOWN + d] & 0x80) { difficulty = d; act0 = bytes[OFF_TOWN + d] & 7; }
  return {
    name,
    version: view.getUint32(OFF_VERSION, true),
    size: view.getUint32(OFF_SIZE, true),
    playerClass: bytes[OFF_CLASS],
    className: CLASS_NAMES[bytes[OFF_CLASS]] ?? `class ${bytes[OFF_CLASS]}`,
    level: bytes[OFF_LEVEL],
    // The client save flags word (0x24): 0x20 Expansion, 0x04 Hardcore (PLRSAVE2_WriteSaveHeader).
    flags: view.getUint32(0x24, true) >>> 0,
    expansion: Boolean(view.getUint32(0x24, true) & 0x20),
    hardcore: Boolean(view.getUint32(0x24, true) & 0x04),
    dead: Boolean(view.getUint32(0x24, true) & 0x08),
    difficulty, act0: Math.min(act0, 4),
    bytes: bytes.length,
    // The character-menu appearance (D2SaveHeaderStrc 0x88: 16 component indices, 0x98: 16 colours),
    // written by INVENTORY_GetItemSaveGfxInfo; character select draws the portrait from it.
    appearance: bytes.length >= 0xA8 ? Array.from(bytes.subarray(0x88, 0x98)) : null,
    appearanceColours: bytes.length >= 0xA8 ? Array.from(bytes.subarray(0x98, 0xA8)) : null,
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
 *
 * `navigator.storage.persist()` is requested once, at the first `put()`:
 * Safari evicts a site's storage after 7 days with no interaction ("Support
 * matrix"), and a save is the one thing here that must survive that. Best-effort and never awaited
 * into the save path -- a browser that refuses or lacks the API still gets the save; it just keeps
 * the eviction risk the support matrix already documents.
 */
export function createSaveStore({ indexedDBObject = globalThis.indexedDB, storageManager = globalThis.navigator?.storage,
  localStorageObject = globalThis.localStorage } = {}) {
  let dbPromise = null, persistRequested = false;
  const db = () => (dbPromise ??= open(indexedDBObject));
  const journal = createUnloadSaveJournal(localStorageObject);
  function requestPersistOnce() {
    if (persistRequested) return;
    persistRequested = true;
    try { storageManager?.persist?.().catch(() => {}); } catch {}
  }

  async function restoreUnloadJournal() {
    // An upgrade can briefly have the old un-suffixed v2 entry plus the new name-scoped one.
    // Keep only the newest valid hand-off for a name before issuing IndexedDB gets, otherwise two
    // callbacks for that one object-store key could race each other inside this transaction.
    const candidates = journal.readAll().filter(({ record }) => readSaveHeader(record.data)?.name === record.name);
    const byName = new Map();
    for (const entry of candidates) {
      if (readSaveHeader(entry.record.data)?.name !== entry.record.name) continue;
      const prior = byName.get(entry.record.name);
      if (!prior || prior.record.revision < entry.record.revision) byName.set(entry.record.name, entry);
    }
    const found = [...byName.values()];
    if (!found.length) return false;
    try {
      const database = await db();
      const applied = await new Promise((resolve, reject) => {
        const tx = database.transaction(STORE, 'readwrite'), store = tx.objectStore(STORE);
        const used = [];
        for (const entry of found) {
          const get = store.get(entry.record.name);
          get.onsuccess = () => {
            const stored = get.result;
            if (!stored || (Number(stored.revision) || 0) < entry.record.revision) {
              store.put(entry.record);
              used.push(entry);
            }
          };
          get.onerror = () => reject(get.error || new Error('IndexedDB journal read failed'));
        }
        tx.oncomplete = () => resolve(used);
        tx.onerror = () => reject(tx.error || new Error('IndexedDB journal restore failed'));
        tx.onabort = () => reject(tx.error || new Error('IndexedDB journal restore aborted'));
      });
      for (const entry of candidates) journal.discard(entry);
      return applied.length > 0;
    } catch { return false; }
  }

  return {
    async put(bytes) {
      const header = readSaveHeader(bytes);
      if (!header) throw new Error('refusing to store bytes that are not a .d2s');
      if (!header.name) throw new Error('refusing to store a character with no name');
      requestPersistOnce();
      // Stored as a plain copy: a view onto wasm memory would be invalidated by the next growth.
      const record = { name: header.name, playerClass: header.playerClass, level: header.level,
        saved: Date.now(), revision: journal.nextRevision(), data: Uint8Array.from(bytes) };
      await run(await db(), 'readwrite', (store) => store.put(record));
      return header;
    },
    // This is intentionally synchronous. It is called from beforeunload, where an IndexedDB put
    // may be abandoned when the browser destroys the document. The next list/get imports it.
    putDuringUnload(bytes) {
      const header = readSaveHeader(bytes);
      if (!header?.name) throw new Error('refusing to journal bytes that are not a named .d2s');
      if (!localStorageObject) throw new Error('localStorage unavailable for unload journal');
      const record = { name: header.name, playerClass: header.playerClass, level: header.level,
        saved: Date.now(), revision: journal.nextRevision(), data: Uint8Array.from(bytes) };
      journal.write(record);
      return header;
    },
    // Side data (PlugY .sss blobs) beside the characters, same store, key '_LOD_<name>.sss'. Never a .d2s.
    async putAux(key, bytes) { requestPersistOnce(); await run(await db(), 'readwrite', (store) => store.put({ name: key, aux: true, saved: 0, data: Uint8Array.from(bytes) })); },
    async getAux(key) { const r = await run(await db(), 'readonly', (store) => store.get(key)); return r ? Uint8Array.from(r.data) : null; },
    get persistRequested() { return persistRequested; },
    async get(name) {
      await restoreUnloadJournal();
      const record = await run(await db(), 'readonly', (store) => store.get(name));
      return record ? Uint8Array.from(record.data) : null;
    },
    async listProfile() {
      await restoreUnloadJournal();
      const records = await run(await db(), 'readonly', (store) => store.getAll());
      const rows = records || [];
      const characters = rows
        .map((record) => ({ ...readSaveHeader(Uint8Array.from(record.data)), saved: record.saved }))
        .filter((header) => header && header.name)
        .sort((a, b) => b.saved - a.saved);
      // Retail's SINGLE PLAYER callback checks whether a *.d2s filename exists before it parses
      // the save. Preserve that distinction: an unreadable file still routes to SELECT, where the
      // character enumerator handles it, while auxiliary .sss/.ma0 records do not count as saves.
      const hasSaveFiles = rows.some((record) => record && !record.aux && typeof record.name === 'string');
      return { characters, hasSaveFiles };
    },
    async list() { return (await this.listProfile()).characters; },
    // Deleting a character deletes everything it owns: its .d2s, its PlugY side blob and its saved
    // automap layer (F268, the `<character>.ma0` written beside it at Save and Exit).
    async remove(name) { journal.invalidate(name); await run(await db(), 'readwrite', (store) => { store.delete(`_LOD_${name}.sss`); store.delete(`_LOD_${name}.d2x`); store.delete(`_LOD_${name}.ma0`); return store.delete(name); }); },
    async clear() { journal.invalidate(); await run(await db(), 'readwrite', (store) => store.clear()); },
  };
}
