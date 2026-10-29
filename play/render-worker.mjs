// Render worker.
// The logic is a pure factory so node can test it; the bottom wires self.onmessage only inside a worker.
// Protocol: init {width,height,assets} | frame {seq,packed} | files {files} | close  ->  drawlist {seq,list,incomplete?} | need {seq,paths} | error {seq,message}.
import { unpackFrame, unpackTerrain } from './frame-pack.mjs';

// Frame messages carry packed = packFrame(...) plus optional meta {player,width,height,viewCX,palettePath} and wantList.
// A bare terrain pack (packTerrain) is still accepted. Files: {type:'files', files:[[path, Uint8Array|null]...]} feeds read().
// A read of a file not yet received is recorded; the frame's reply is then [ {type:'need', paths}, {type:'drawlist', incomplete:true, ...} ].
// Otherwise the reply is the single drawlist message.
// Worker side of the sync channel: ctl = Int32Array(sab, 0, 4) [state, length], data after 16 bytes. Returns bytes, null
// (main has no such file) or undefined (too big / timed out: fall back to the need protocol).
export function makeFetchSync(sab, post = (m) => self.postMessage(m)) {
  const ctl = new Int32Array(sab, 0, 4), data = new Uint8Array(sab, 16);
  return (path) => {
    Atomics.store(ctl, 0, 0);
    post({ type: 'need', sync: true, paths: [path] });
    if (Atomics.wait(ctl, 0, 0, 20000) === 'timed-out') return undefined;
    const n = ctl[1];
    return n === -1 ? null : n < 0 ? undefined : data.slice(0, n);
  };
}

export function createRenderWorkerCore({ createCanvas, draws = [], decodeDt1, decodeDatPalette, documentObject, fetchSync = null }) {
  let canvas = null, ctx = null, assets = null, closed = false, owned = false;
  const store = new Map(), missed = new Set();
  const doc = documentObject ?? { createElement: () => createCanvas(1, 1) };
  const read = (path) => {
    if (store.has(path)) {
      const b = store.get(path);
      if (b === null) throw new Error('missing file ' + path);
      return b;
    }
    // A blocking fetch (SharedArrayBuffer round trip to main) lets one frame discover every file; without it a frame ends 'incomplete'.
    const got = fetchSync ? fetchSync(path) : undefined;
    if (got !== undefined) { store.set(path, got); if (got === null) throw new Error('missing file ' + path); return got; }
    missed.add(path); throw new Error('file not loaded ' + path);
  };
  const env = { read, decodeDt1, decodeDatPalette, documentObject: doc, missed };
  const fail = (seq, e) => ({ type: 'error', seq, message: String(e && e.message || e) });
  function handle(msg) {
    try {
      if (!msg || typeof msg.type !== 'string') throw new Error('bad message');
      if (msg.type === 'init') {
        owned = !!msg.canvas;
        if (msg.sync && !fetchSync) fetchSync = makeFetchSync(msg.sync);
        canvas = msg.canvas ?? createCanvas(msg.width, msg.height);
        ctx = canvas.getContext('2d', msg.ctxOptions ?? { alpha: false, willReadFrequently: true });   // the page canvas's options (render-wire WORLD_CTX)
        assets = msg.assets ?? {};
        return null;
      }
      if (msg.type === 'resize') { if (canvas) { canvas.width = msg.width; canvas.height = msg.height; } return null; }
      if (msg.type === 'files') { for (const [path, bytes] of msg.files ?? []) store.set(path, bytes ?? null); return null; }
      if (msg.type === 'close') { closed = true; ctx = canvas = assets = null; store.clear(); return null; }
      if (msg.type === 'frame') {
        if (closed || !ctx) throw new Error('frame before init');
        const p = msg.packed;
        if (!p || !(p.buf || p.terrain)) throw new Error('frame without packed buffer');
        const frame = p.terrain ? unpackFrame(p) : { tiles: unpackTerrain(p), files: (p.files ?? []).slice(), lightmap: null };
        frame.seq = msg.seq; frame.terrain = frame.tiles; frame.meta = p.meta ?? {}; frame.wantList = !!p.wantList;
        const prev = globalThis.__d2DrawList;
        const list = [];
        globalThis.__d2DrawList = list;
        missed.clear();
        env.evidence = {};
        // A transferred canvas is the page's world layer: the black field main paints first (and clears on its own canvas) is ours.
        if (owned) { ctx.fillStyle = '#000'; ctx.fillRect(0, 0, canvas.width, canvas.height); }
        try { for (const d of draws) d(ctx, frame, assets, env); }
        finally { globalThis.__d2DrawList = prev; }
        const reply = { type: 'drawlist', seq: msg.seq, list: frame.wantList ? list : null };
        try { reply.evidence = structuredClone(env.evidence); } catch { reply.evidence = null; }
        if (!missed.size) return reply;
        reply.incomplete = true;
        return [{ type: 'need', seq: msg.seq, paths: [...missed] }, reply];
      }
      throw new Error('unknown message type ' + msg.type);
    } catch (e) { return fail(msg && msg.seq, e); }
  }
  return { handle };
}

if (typeof WorkerGlobalScope !== 'undefined' && typeof self !== 'undefined' && self instanceof WorkerGlobalScope) {
  // session-terrain's shadow silhouette falls back to globalThis.document; a worker has none.
  globalThis.document ??= { createElement: () => new OffscreenCanvas(1, 1) };
  // onmessage is set before any await: a module worker's top-level await does not hold the message queue, so messages
  // posted during the imports (init, the first frames) were dispatched to nobody and lost, leaving the host's in-flight slots full for good.
  let ready = (async () => {
    const { draws } = await import('./render-worker-draws.mjs').catch(() => ({ draws: [] }));
    const { decodeDt1, decodeDatPalette } = await import('./assets/d2-formats.mjs');
    return createRenderWorkerCore({ createCanvas: (w, h) => new OffscreenCanvas(w, h), draws, decodeDt1, decodeDatPalette });
  })();
  self.onmessage = (e) => {
    ready = ready.then((core) => {
      try { const r = core.handle(e.data); if (r) for (const m of [].concat(r)) self.postMessage(m); } catch (err) { self.postMessage({ type: 'error', seq: e.data?.seq, message: String(err?.message ?? err) }); }
      if (e.data && e.data.type === 'close') self.close();
      return core;
    });
  };
}
