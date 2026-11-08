// Where the player's own Diablo II files come from: picker, folder, drop, and (on Chromium) a remembered
// set of file handles. Nothing here is uploaded; every file stays a Blob/File in this browser and is
// handed to the engine through StreamFS.mountBlob. Detection is byo-detect.mjs.
import { inspect, classify, extractInnerPatch, ROLES, ORDER } from './byo-detect.mjs';
import { isPlugyFile } from './plugy-files.mjs';
import { unpackArchive, isArchiveName } from './byo-archive.mjs';

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

// The archives, plus PlugY's stash art and PlugY.ini from a D2SE folder (plugy-files.mjs): they are not archives and are kept apart.
const WANT = { test: (n) => /\.(mpq|exe)$/i.test(n) || isPlugyFile(n) };

/** Walk a directory handle (two levels is enough for an install folder with D2SE/CORES inside). */
async function walk(dir, depth = 3, out = []) {
  for await (const [, h] of dir.entries()) {
    if (h.kind === 'file') { if (WANT.test(h.name)) out.push(h); }
    else if (depth > 0) await walk(h, depth - 1, out);
  }
  return out;
}

// `step` (every reader below takes one) reports progress to the door's upload popup; upload-modal.mjs lists the steps.
const quiet = () => {};

/** Files (and handles, where the browser gives them) from a drop. */
export async function fromDrop(dt, step = quiet) {
  const handles = [], files = [];
  const items = [...(dt.items || [])];
  for (const it of items) {
    if (it.kind !== 'file') continue;
    const h = it.getAsFileSystemHandle ? await it.getAsFileSystemHandle().catch(() => null) : null;
    if (h) { if (h.kind === 'directory') { step({ kind: 'reading' }); handles.push(...await walk(h)); } else if (WANT.test(h.name)) handles.push(h); continue; }
    const e = it.webkitGetAsEntry && it.webkitGetAsEntry();
    if (e) { if (e.isDirectory) step({ kind: 'reading' }); files.push(...await entryFiles(e)); } else { const f = it.getAsFile(); if (f && WANT.test(f.name)) files.push(f); }
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

// ---- pickerless browsers (Android Chrome): the picked files are copied into OPFS, so a reload keeps them ----
// An <input> File is a snapshot the browser will not remember. Where neither picker exists, each picked file is
// copied into the origin's private file system (navigator.storage.getDirectory()) and its handle joins the same
// remembered set the Chromium pickers fill. A later boot reopens it through restore(), and the door mounts it as
// it mounts a remembered desktop handle. Without OPFS, or where a picker exists, nothing here runs.
const OPFS_DIR = 'd2-byo', CHUNK = 16 << 20;
const opfsAvailable = () => !!navigator.storage?.getDirectory && typeof FileSystemFileHandle !== 'undefined';
const storeAvailable = () => !window.showOpenFilePicker && !window.showDirectoryPicker && opfsAvailable();

/**
 * Copy each File into OPFS, asking the browser to keep the storage first. Resolves { handles, files }: a file
 * whose copy fails (quota, a locked file) stays a File for this visit only, so the page still works without OPFS.
 */
export async function stashFiles(files, progress = () => {}, step = quiet) {
  try { await navigator.storage.persist?.(); } catch { /* refused: the copy still works, the browser may evict it */ }
  const dir = await (await navigator.storage.getDirectory()).getDirectoryHandle(OPFS_DIR, { create: true });
  const total = files.reduce((n, f) => n + f.size, 0) || 1, names = new Set(), handles = [], kept = [];
  let done = 0;
  for (const f of files) {
    if (names.has(f.name)) { kept.push(f); continue; }   // two picks with one name: the second stays a File
    names.add(f.name);
    try {
      const h = await dir.getFileHandle(f.name, { create: true }), w = await h.createWritable();
      try {
        for (let p = 0; p < f.size; p += CHUNK) {
          const part = f.slice(p, p + CHUNK);
          await w.write(part); done += part.size;
          step({ kind: 'store', name: f.name, pct: Math.floor(100 * done / total) });
          progress(`Copying your files to this browser: ${Math.floor(100 * done / total)}%`);
        }
        await w.close();
      } catch (e) { await w.abort?.().catch(() => {}); throw e; }
      handles.push(h);
    } catch (e) {
      done += f.size; kept.push(f);
      await dir.removeEntry(f.name).catch(() => {});
      progress(`${f.name} could not be copied (${e.name || 'error'}); it lasts this visit only`);
    }
  }
  return { handles, files: kept };
}

/** Pickerless browsers copy into OPFS; everyone else, or a host where OPFS will not open, keeps the Files. */
async function keepFiles(files, progress, step) {
  if (!files.length || !storeAvailable()) return { handles: [], files };
  try { return await stashFiles(files, progress, step); } catch { return { handles: [], files }; }
}

// An archive entry has no handle of its own, so it is copied to OPFS on every browser that has it, pickers or not.
async function archiveDir() {
  if (!opfsAvailable()) return null;
  try { await navigator.storage.persist?.(); } catch { /* refused: the copies still work, the browser may evict them */ }
  try { return await (await navigator.storage.getDirectory()).getDirectoryHandle(OPFS_DIR, { create: true }); }
  catch { return null; }
}
/**
 * The .mpq/.exe/PlugY entries of each picked .zip or .7z (byo-archive.mjs). Where OPFS exists each entry is
 * written there, in a folder of its own, so it is remembered like a copied pick; elsewhere it is kept as a File.
 */
async function unpackPicked(archives, step = quiet) {
  const handles = [], files = [];
  if (!archives.length) return { handles, files };
  const dir = await archiveDir();
  let seq = 0;
  const copy = async (name, open) => {
    const sub = await dir.getDirectoryHandle(`archive-${seq++}`, { create: true });
    const h = await sub.getFileHandle(name, { create: true }), w = await h.createWritable();
    const r = (await open()).getReader();
    try {
      for (;;) { const { done, value } = await r.read(); if (done) break; await w.write(value); }
      await w.close();
    } catch (e) { await w.abort?.().catch(() => {}); await sub.removeEntry(name).catch(() => {}); throw e; }
    return h;
  };
  const collect = async (name, open) => {
    const r = (await open()).getReader(), parts = [];
    for (;;) { const { done, value } = await r.read(); if (done) break; parts.push(value); }
    return new File(parts, name, { type: 'application/octet-stream' });
  };
  for (const [i, a] of archives.entries()) {
    const at = { kind: 'unzip', name: a.name, n: i + 1, m: archives.length };
    step(at);
    await unpackArchive(a, {
      wanted: (n) => WANT.test(n),
      place: async (name, size, open) => {
        step({ ...at, entry: name });
        if (dir) {
          try { handles.push(await copy(name, open)); return; }
          catch (e) { if (e.corrupt) throw e; }   // a bad entry is skipped; a copy the disk refused is kept in memory
        }
        files.push(await collect(name, open));
      },
    });
  }
  return { handles, files };
}

/** Chromium pickers return handles (which can be remembered); elsewhere an <input> returns Files, copied to OPFS where it exists. */
export async function pickFiles({ progress, step } = {}) {
  if (window.showOpenFilePicker) {
    try {
      const hs = await window.showOpenFilePicker({ multiple: true, types: [{ description: 'Diablo II archives', accept: { 'application/octet-stream': ['.mpq', '.exe', '.zip', '.7z'] } }] });
      const arc = [], plain = [];
      for (const h of hs) (isArchiveName(h.name) ? arc : plain).push(h);
      const unpacked = await unpackPicked(await Promise.all(arc.map((h) => h.getFile())), step);
      return { handles: [...plain, ...unpacked.handles], files: unpacked.files };
    } catch (e) { if (e.name === 'AbortError') return { handles: [], files: [] }; }
  }
  const picked = await inputFiles(false);
  const unpacked = await unpackPicked(picked.filter((f) => isArchiveName(f.name)), step);
  const kept = await keepFiles(picked.filter((f) => !isArchiveName(f.name)), progress, step);
  return { handles: [...unpacked.handles, ...kept.handles], files: [...unpacked.files, ...kept.files] };
}
export async function pickFolder({ progress, step = quiet } = {}) {
  if (window.showDirectoryPicker) {
    let dir = null;
    try { dir = await window.showDirectoryPicker(); } catch (e) { if (e.name === 'AbortError') return { handles: [], files: [] }; }
    if (dir) { step({ kind: 'reading' }); return { handles: await walk(dir), files: [] }; }   // a folder that cannot be read is an error, not a second picker
  }
  return keepFiles(await inputFiles(true), progress, step);
}
const inputFiles = (folder) => new Promise((res) => {
  const i = document.createElement('input');
  i.type = 'file'; i.multiple = true; if (folder) i.webkitdirectory = true; else if (!/Android/i.test(navigator.userAgent)) i.accept = '.mpq,.exe,.zip,.7z';   // Android's chooser greys out any file whose type it cannot map from an unknown extension; the names are filtered below instead
  i.onchange = () => res([...i.files].filter((f) => WANT.test(f.name) || (!folder && isArchiveName(f.name))));
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
  const plugy = {}, plugyHandles = [];   // PlugY's files by lower-case name (plugy-files.mjs plugySource reads them)
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
    plugy,
    async add({ handles = [], files = [] }, step = quiet) {
      state.busy++;
      try {
        const incoming = [];
        for (const h of handles) { try { incoming.push({ file: await h.getFile(), handle: h }); } catch { /* moved or denied */ } }
        for (const f of files) incoming.push({ file: f });
        for (const [i, e] of incoming.entries()) {
          step({ kind: 'process', name: e.file.name, n: i + 1, m: incoming.length });
          const key = e.file.name + ':' + e.file.size + ':' + e.file.lastModified;
          if (seen.has(key)) continue;
          seen.add(key);
          if (isPlugyFile(e.file.name)) { plugy[e.file.name.toLowerCase()] = e.file; if (e.handle) plugyHandles.push(e.handle); continue; }
          e.info = await inspect(e.file);
          entries.push(e);
          if (e.info.installer && e.info.inner) {
            const inner = await extractInnerPatch(e.file, e.info).catch(() => null);
            if (inner) entries.push({ file: inner, info: inner.info, from: e.file.name });
          }
        }
        await refold();
        // Only archives the game uses (or an installer that held one) are worth remembering.
        const keep = entries.filter((e) => e.handle && (e.info.role || e.info.installer)).map((e) => e.handle).concat(plugyHandles);
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
      // An OPFS handle (a phone's copy) has no permission to ask about: this origin always reads its own files.
      if (typeof h.queryPermission !== 'function') { ok.push(h); continue; }
      let p = await h.queryPermission({ mode: 'read' });
      if (p !== 'granted' && ask) p = await h.requestPermission({ mode: 'read' });
      if (p === 'granted') ok.push(h);
    } catch { /* stale handle */ }
  }
  return { count: hs.length, handles: ok };
}
