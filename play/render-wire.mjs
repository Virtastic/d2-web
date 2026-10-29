// Phase 2 render-worker wiring. `?render=w` (the worker draws the world) is opt-in:
// the Stage 6 default-on was reverted 2026-10-27 because the worker's draw list differs from main's (see
// test/defaults-on.test.mjs). `t` is the selftest; absent, `0` or any other value is the old path and
// yields null, which costs the shell nothing. The host is created lazily, once the engine is ready.
import { packFrame } from './frame-pack.mjs';

export const renderMode = (search) => {
  const v = new URLSearchParams(search).get('render');
  if (v === null) return '0';
  return v === 't' || v === 'w' ? v : '0';
};
const WORLD_CTX = { alpha: false, willReadFrequently: true };   // the page canvas's 2d options, minus nothing the worker needs

const defaultLoad = async () => {
  const { createRenderHost } = await import('./render-host.mjs');
  let covers = () => false;
  try { ({ covers } = await import('./render-worker-draws.mjs')); } catch { /* not built yet: the worker keeps nothing */ }
  return { createRenderHost, covers };
};

/** @returns {null | {begin, end, host}} null unless mode is 't'. */
export function createRenderWire({ mode, load = defaultLoad, Worker = globalThis.Worker, url, readFile, expose = true,
  canvas = null, document: doc = globalThis.document, canvasProto = globalThis.HTMLCanvasElement?.prototype, ctxOptions = WORLD_CTX } = {}) {
  if (mode !== 't' && mode !== 'w') return null;
  const world = mode === 'w';
  let host = null, starting = false, seq = 0, owns = false, evidence = null, worldEl = null, dead = false, prevPos = null;
  let mine = null;   // the list begin() armed last
  let ext = false;   // a recorder outside the wire armed __d2DrawList this frame: main draws everything so its list stays whole
  const setMode = (v) => { const d = (globalThis.__d2 = globalThis.__d2 || {}); d.renderMode = v; };
  const fallback = (reason) => {
    dead = true; owns = false;
    setMode('0:' + reason);
    if (worldEl) { worldEl.remove?.(); worldEl = null; }   // a transferred canvas cannot be drawn by main again
    if (prevPos !== null && canvas?.style) { canvas.style.position = prevPos; prevPos = null; }
    try { host?.close(); host?.terminate?.(); } catch { /* already gone */ }
    host = null;
  };
  const syncBox = () => {
    if (!worldEl || !canvas) return;
    const s = canvas.style ?? {}, w = worldEl.style;
    w.position = 'absolute'; w.pointerEvents = 'none';
    for (const k of ['width', 'height', 'left', 'top']) if (s[k]) w[k] = s[k];
  };
  const start = (width, height) => {
    starting = true;
    if (world) {
      if (!(canvasProto && 'transferControlToOffscreen' in canvasProto) || !doc || !canvas?.parentNode) { fallback('unsupported'); return; }
    }
    load().then(({ createRenderHost, covers }) => {
      let initArg = { width, height };
      if (world) {
        worldEl = doc.createElement('canvas');
        worldEl.width = width; worldEl.height = height; worldEl.className = canvas.className ?? '';
        syncBox();
        canvas.parentNode.insertBefore(worldEl, canvas);
        // An absolute layer paints above a static sibling; a positioned #game keeps DOM order (world under main).
        if (canvas.style) { prevPos = canvas.style.position ?? ""; if (!prevPos) canvas.style.position = "relative"; }
        initArg = { width, height, canvas: worldEl.transferControlToOffscreen(), ctxOptions };
      }
      const h = createRenderHost({ Worker, url: url ?? new URL('./render-worker.mjs', import.meta.url), mode,
        keep: typeof covers === 'function' ? covers : () => false, readFile, onDrawlist: world ? () => { if (!dead && !owns) { owns = true; setMode('w'); } } : null, onEvidence: world ? (ev) => { evidence = ev; } : null, onError: world ? (m) => fallback('worker: ' + (m?.message ?? 'error')) : null });
      host = h;
      if (world) h.init(initArg); else h.init(width, height);
      if (expose) globalThis.__d2RenderHost = h;
    }).catch((error) => { globalThis.__d2RenderHostError = String(error); if (world) fallback('start: ' + error); });
  };
  return {
    get host() { return host; },
    /** True while the worker's canvas paints the world and main must skip the covered draws. */
    get ownsWorld() { return owns && !ext; },
    /** Latest {terrainStats, controlPanel, belt} the worker computed while drawing (world mode), else null. */
    get evidence() { return owns && !ext ? evidence : null; },
    /** Main canvas resized (world mode): resize the worker canvas and keep the CSS box in step. */
    resize(width, height) { if (!host) return; syncBox(); host.resize(width, height); },
    /** Before the draw: make sure a draw list is recording. Returns the token for end(). */
    begin() {
      // A frame that threw between begin and end left our list armed: it is ours, not an outside recorder's. Re-take it.
      const own = !Array.isArray(globalThis.__d2DrawList) || globalThis.__d2DrawList === mine;
      ext = !own;
      if (own) globalThis.__d2DrawList = mine = [];
      return { own, list: globalThis.__d2DrawList };
    },
    /** After the draw: record the main list, post the packed frame, restore the list. `ready` gates it. */
    end(token, { ready, frameSnap, meta, entities, objects }) {
      try {
        if (!ready) return;
        if (dead) return;
        if (!host) { if (!starting) start(meta.width, meta.height); return; }
        const tiles = frameSnap?.tiles;
        if (!tiles?.length) return;
        const n = ++seq, ents = Array.isArray(entities) ? entities : [];   // units + the facing-joined subset
        const packed = packFrame({ tiles, files: frameSnap.files ?? [], units: ents, facings: ents.filter((e) => e.direction !== undefined), lightmap: frameSnap.lightmap ?? null, objects: Array.isArray(objects) ? objects : [] });
        packed.meta = meta;
        host.post(n, packed);   // the host transfers the buffers (frameTransferList)
        globalThis.__d2 = globalThis.__d2 || {}; globalThis.__d2.renderCounters = host.stats?.();   // plan 5.3: frames, dropped, incomplete, errors
        if (!world) host.recordMain(n, token.list);
      } finally {
        if (token.own) delete globalThis.__d2DrawList;
      }
    },
  };
}
