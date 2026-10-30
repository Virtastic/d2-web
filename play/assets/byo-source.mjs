// Where the player's own Diablo II files come from: picker, folder, drop, and (on Chromium) a remembered
// set of file handles. Nothing here is uploaded; every file stays a Blob/File in this browser and is
// handed to the engine through StreamFS.mountBlob. Detection is byo-detect.mjs.
import { inspect, classify, extractInnerPatch, ROLES, ORDER } from './byo-detect.mjs';

const DB = 'd2-byo', STORE = 'handles', KEY = 'set';
const idb = (mode, fn) => new Promise((res, rej) => {
  try {
    const open = indexedDB.open(DB, 1);
    open.onupgradeneeded = () => open.result.createObjectStore(STORE);
    open.onerror = () => rej(open.error);
    open.onsuccess = () => {
      const tx = open.result.transaction(STORE, mode), r = fn(tx.objectStore(STORE));
      tx.oncomplete = () => res(r && r.result);
      tx.onerror = () => rej(tx.error);
    };
  } catch (e) { rej(e); }
});
export const remembered = () => idb('readonly', (s) => s.get(KEY)).then((v) => v || []).catch(() => []);
export const remember = (handles) => idb('readwrite', (s) => s.put(handles, KEY)).catch(() => {});
export const forget = () => idb('readwrite', (s) => s.delete(KEY)).catch(() => {});

let known = null;
export const knownMpqs = () => known || (known = fetch('assets/known-mpqs.json').then((r) => r.json()).catch(() => ({ patch_d2: [] })));

// The 1.13c Patch_D2.mpq the host serves base64-encoded (tools/patch-shim.mjs). It mounts over whatever
// patch the player gave, so the engine always reads 1.13c data; null when the host serves none.
let shim = null;
export const patchShim = () => shim || (shim = fetch('assets/patch-113c.patch')
  .then((r) => (r.ok ? r.text() : null))
  .then((t) => {
    if (!t) return null;
    const b = Uint8Array.fromBase64 ? Uint8Array.fromBase64(t) : Uint8Array.from(atob(t), (c) => c.charCodeAt(0));
    return new File([b], 'Patch_D2.mpq', { type: 'application/octet-stream' });
  })
  .catch(() => null));

const WANT = /\.(mpq|exe)$/i;

/** Walk a directory handle (two levels is enough for an install folder with D2SE/CORES inside). */
async function walk(dir, depth = 3, out = []) {
  for await (const [, h] of dir.entries()) {
    if (h.kind === 'file') { if (WANT.test(h.name)) out.push(h); }
    else if (depth > 0) await walk(h, depth - 1, out);
  }
  return out;
}

/** Files (and handles, where the browser gives them) from a drop. */
export async function fromDrop(dt) {
  const handles = [], files = [];
  const items = [...(dt.items || [])];
  for (const it of items) {
    if (it.kind !== 'file') continue;
    const h = it.getAsFileSystemHandle ? await it.getAsFileSystemHandle().catch(() => null) : null;
    if (h) { if (h.kind === 'directory') handles.push(...await walk(h)); else if (WANT.test(h.name)) handles.push(h); continue; }
    const e = it.webkitGetAsEntry && it.webkitGetAsEntry();
    if (e) files.push(...await entryFiles(e)); else { const f = it.getAsFile(); if (f && WANT.test(f.name)) files.push(f); }
  }
  if (!items.length) files.push(...[...dt.files].filter((f) => WANT.test(f.name)));
  return { handles, files };
}
async function entryFiles(e, depth = 3) {
  if (e.isFile) return WANT.test(e.name) ? [await new Promise((r, j) => e.file(r, j))] : [];
  if (!depth) return [];
  const rd = e.createReader(), all = [];
  for (;;) { const b = await new Promise((r, j) => rd.readEntries(r, j)); if (!b.length) break; all.push(...b); }
  return (await Promise.all(all.map((x) => entryFiles(x, depth - 1)))).flat();
}

/** Chromium pickers return handles (which can be remembered); elsewhere an <input> returns Files. */
export async function pickFiles() {
  if (window.showOpenFilePicker) {
    try { return { handles: await window.showOpenFilePicker({ multiple: true, types: [{ description: 'Diablo II archives', accept: { 'application/octet-stream': ['.mpq', '.exe'] } }] }), files: [] }; }
    catch (e) { if (e.name === 'AbortError') return { handles: [], files: [] }; }
  }
  return { handles: [], files: await inputFiles(false) };
}
export async function pickFolder() {
  if (window.showDirectoryPicker) {
    try { return { handles: await walk(await window.showDirectoryPicker()), files: [] }; }
    catch (e) { if (e.name === 'AbortError') return { handles: [], files: [] }; }
  }
  return { handles: [], files: await inputFiles(true) };
}
const inputFiles = (folder) => new Promise((res) => {
  const i = document.createElement('input');
  i.type = 'file'; i.multiple = true; if (folder) i.webkitdirectory = true; else i.accept = '.mpq,.exe';
  i.onchange = () => res([...i.files].filter((f) => WANT.test(f.name)));
  i.oncancel = () => res([]);
  i.click();
});

/**
 * The set of files the player has given so far. add() takes {handles, files}, inspects each, unpacks
 * any official patch installer, and keeps `state` (classify's answer) current.
 */
export function createCollection() {
  const entries = [];   // { file, handle?, info }
  const state = { slots: {}, issues: [], missing: [], ready: false, patch: { level: 'missing' }, busy: 0 };
  const seen = new Set();
  async function refold() {
    const k = await knownMpqs();
    const verified = (i) => (k.patch_d2 || []).some((p) => p.compat === 'verified' && p.sha256 === i.sha256);
    const infos = entries.map((e) => e.info).sort((a, b) => verified(b) - verified(a));
    Object.assign(state, classify(infos, k));
    state.files = Object.fromEntries(Object.entries(state.slots).filter(([, s]) => s.have).map(([r, s]) => [r, entries.find((e) => e.info === s.file).file]));
    // The player must still give their own Patch_D2.mpq; once they have, the 1.13c shim mounts in its place.
    const p = state.slots.patch_d2?.have ? await patchShim() : null;
    if (p) state.files.patch_d2 = p;
  }
  return {
    state,
    entries,
    async add({ handles = [], files = [] }) {
      state.busy++;
      try {
        const incoming = [];
        for (const h of handles) { try { incoming.push({ file: await h.getFile(), handle: h }); } catch { /* moved or denied */ } }
        for (const f of files) incoming.push({ file: f });
        for (const e of incoming) {
          const key = e.file.name + ':' + e.file.size + ':' + e.file.lastModified;
          if (seen.has(key)) continue;
          seen.add(key);
          e.info = await inspect(e.file);
          entries.push(e);
          if (e.info.installer && e.info.inner) {
            const inner = await extractInnerPatch(e.file, e.info).catch(() => null);
            if (inner) entries.push({ file: inner, info: inner.info, from: e.file.name });
          }
        }
        await refold();
        // Only archives the game uses (or an installer that held one) are worth remembering.
        const keep = entries.filter((e) => e.handle && (e.info.role || e.info.installer)).map((e) => e.handle);
        if (keep.length) remember(keep);
      } finally { state.busy--; }
      return state;
    },
    /** What the engine mounts: [{ f, size, blob }] in the engine's own priority order. */
    mounts() {
      return ORDER.filter((r) => state.files && state.files[r]).map((r) => ({ f: ROLES[r].file, size: state.files[r].size, blob: state.files[r] }));
    },
  };
}

/** Re-open the remembered handles; `ask` is true inside a click, where the browser may prompt. */
export async function restore(ask) {
  const hs = await remembered(), ok = [];
  for (const h of hs) {
    try {
      let p = await h.queryPermission({ mode: 'read' });
      if (p !== 'granted' && ask) p = await h.requestPermission({ mode: 'read' });
      if (p === 'granted') ok.push(h);
    } catch { /* stale handle */ }
  }
  return { count: hs.length, handles: ok };
}
