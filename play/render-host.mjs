// Main-side host for the Stage 4 selftest (mode 't'): post frames to the render worker, compare its draw list
// with the main thread's own list per seq. Main keeps drawing as today; nothing here touches the page canvas.
import { diffDrawLists } from './assets/drawlist.mjs';
import { transferList, frameTransferList } from './frame-pack.mjs';

export const MAX_IN_FLIGHT = 3;

// Exact per-record equality; diffDrawLists (an LCS table, n*m) runs only on a mismatch.
const same = (a, b) => a.length === b.length && a.every((r, i) => { const o = b[i];
  return r.op === o.op && r.path === o.path && r.frame === o.frame && (r.hash ?? '') === (o.hash ?? '') && r.x === o.x && r.y === o.y && r.mode === o.mode; });

export function createRenderHost({ Worker, url, mode = 't', maxInFlight = MAX_IN_FLIGHT, keep = null, readFile = null, onError = null, onEvidence = null, onDrawlist = null }) {
  const worker = new Worker(url, { type: 'module' });
  const st = { frames: 0, matched: 0, mismatched: 0, dropped: 0, errors: 0, incomplete: 0, firstMismatchSeq: null, firstDiff: null, firstError: null };
  const inFlight = new Set(), mainLists = new Map(), workerLists = new Map();
  function compare(seq) {
    if (!mainLists.has(seq) || !workerLists.has(seq)) return;
    const a = mainLists.get(seq), b = workerLists.get(seq);
    mainLists.delete(seq); workerLists.delete(seq);
    st.frames++;
    if (same(a, b)) { st.matched++; return; }
    st.mismatched++;
    if (st.firstMismatchSeq === null) { st.firstMismatchSeq = seq; st.firstDiff = diffDrawLists(a, b); }
  }
  // Sync channel (cross-origin isolated pages): the worker blocks on a read until main writes the bytes here.
  const SYNC_BYTES = 32 << 20;
  const sab = typeof SharedArrayBuffer !== 'undefined' && globalThis.crossOriginIsolated && readFile ? new SharedArrayBuffer(16 + SYNC_BYTES) : null;
  async function serveSync(path) {
    const ctl = new Int32Array(sab, 0, 4);
    let b = null; try { b = (await readFile(path)) ?? null; } catch { b = null; }
    if (b && b.length > SYNC_BYTES) ctl[1] = -2;
    else if (b) { new Uint8Array(sab, 16).set(b); ctl[1] = b.length; } else ctl[1] = -1;
    Atomics.store(ctl, 0, 1); Atomics.notify(ctl, 0);
  }
  const requested = new Set();   // paths asked for and not yet answered, or answered null: never asked twice
  async function serve(paths) {
    const todo = paths.filter((p) => !requested.has(p));
    if (!readFile || !todo.length) return;
    todo.forEach((p) => requested.add(p));
    const files = [];
    for (const p of todo) { let b = null; try { b = (await readFile(p)) ?? null; } catch { b = null; } files.push([p, b]); }
    worker.postMessage({ type: 'files', files });
    for (const [p, b] of files) if (b) requested.delete(p);   // delivered: a later need means the worker lost it
  }
  worker.onerror = (e) => { st.errors++; st.firstError ??= String(e?.message ?? 'worker error'); onError?.({ message: st.firstError }); };
  worker.onmessage = (e) => {
    const m = e.data;
    if (!m) return;
    if (m.type === 'need') { if (m.sync && sab) serveSync(m.paths[0]); else serve(m.paths ?? []); return; }
    inFlight.delete(m.seq);
    if (m.type === 'drawlist' && m.incomplete) { st.incomplete++; mainLists.delete(m.seq); workerLists.delete(m.seq); }
    else if (m.type === 'drawlist') { onDrawlist?.(m); if (m.evidence) onEvidence?.(m.evidence); workerLists.set(m.seq, keep ? (m.list ?? []).filter(keep) : (m.list ?? [])); compare(m.seq); }
    else if (m.type === 'error') { onError?.(m); st.errors++; st.firstError ??= m.message; mainLists.delete(m.seq); }
  };
  return {
    mode,
    init(width, height, assets) {
      let canvas, ctxOptions;
      if (width && typeof width === 'object') ({ width, height, assets, canvas, ctxOptions } = width);
      worker.postMessage({ type: 'init', width, height, assets, ...(sab ? { sync: sab } : {}), ...(canvas ? { canvas, ctxOptions } : {}) }, canvas ? [canvas] : []); },
    resize(width, height) { worker.postMessage({ type: 'resize', width, height }); },
    terminate() { worker.terminate?.(); },
    post(seq, packed) {
      if (inFlight.size >= maxInFlight) { st.dropped++; mainLists.delete(seq); return false; }
      inFlight.add(seq);
      packed.wantList = mode !== 'w';
      worker.postMessage({ type: 'frame', seq, packed }, packed.terrain ? frameTransferList(packed) : transferList(packed));
      return true;
    },
    // Call post(seq) before recordMain(seq): a seq that was dropped, errored or never posted is ignored, so nothing leaks.
    recordMain(seq, list) { if (!inFlight.has(seq) && !workerLists.has(seq)) return; mainLists.set(seq, keep ? list.filter(keep) : list.slice()); compare(seq); },
    stats() { return { ...st, inFlight: inFlight.size, pendingMain: mainLists.size }; },
    close() { worker.postMessage({ type: 'close' }); },
  };
}
