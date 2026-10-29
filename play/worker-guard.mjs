// Worker-mode guard (plan 6.5): every engine entry point parks first when the worker owns the engine
// or a tail/render is pending. W supplies park(), owned(), pending(), parking().
/** ?worker= : absent is 0 (OFF by default, reverted 2026-10-27: see test/defaults-on.test.mjs); t, 1, 2 as asked; anything else 0. */
export const workerMode = (search) => { const v = new URLSearchParams(search).get('worker'); if (v === null) return '0'; return v === 't' || v === '1' || v === '2' ? v : '0'; };
const EXCLUDED = /^(_d2_web_worker_|_free$|_emscripten_|_pthread_|__)/;
const MARK = Symbol.for('d2.workerGuard');

const INPUT_EVENTS = ['keydown', 'keyup', 'pointerdown', 'pointerup', 'pointermove', 'pointercancel', 'wheel', 'contextmenu', 'blur', 'visibilitychange', 'resize'];

// Capture-phase park-first listeners: the engine is parked before any page handler sees the event.
export function installParkListeners(W, target = globalThis) {
  if (!target || typeof target.addEventListener !== 'function') return () => {};
  const on = () => { if ((W.owned() || W.pending()) && !W.parking()) W.park(); };
  const opts = { capture: true, passive: true };
  for (const t of INPUT_EVENTS) target.addEventListener(t, on, opts);
  return () => { for (const t of INPUT_EVENTS) target.removeEventListener(t, on, opts); };
}

// Re-point the heap views at the current wasm memory buffer when it grew.
function freshenHeap(Module) {
  const buf = Module.wasmMemory && Module.wasmMemory.buffer;
  if (!buf || (Module.HEAPU8 && Module.HEAPU8.buffer === buf)) return;
  if (typeof Module.updateMemoryViews === 'function') { Module.updateMemoryViews(); return; }
  Module.HEAP8 = new Int8Array(buf); Module.HEAPU8 = new Uint8Array(buf);
  Module.HEAP16 = new Int16Array(buf); Module.HEAPU16 = new Uint16Array(buf);
  Module.HEAP32 = new Int32Array(buf); Module.HEAPU32 = new Uint32Array(buf);
  Module.HEAPF32 = new Float32Array(buf); Module.HEAPF64 = new Float64Array(buf);
}

export function installWorkerGuard(Module, W, counters, mode = 1, target = globalThis) {
  if (!mode || !Module || !W || Module[MARK]) return false;
  const wrap = (fn) => function guarded(...args) {
    if (!W.parking()) {
      const owned = !!W.owned();
      if (owned || W.pending()) {
        if (owned && counters) counters.callWhileOwned = (counters.callWhileOwned | 0) + 1;
        W.park();
      }
    }
    return fn.apply(this, args);
  };
  for (const name of Object.keys(Module)) {
    if (name.startsWith('_d2_') && !EXCLUDED.test(name) && typeof Module[name] === 'function') Module[name] = wrap(Module[name]);
  }
  if (typeof Module._malloc === 'function') {
    const m = wrap(Module._malloc);
    Module._malloc = function malloc(...a) { const r = m.apply(this, a); freshenHeap(Module); return r; };
  }
  Object.defineProperty(Module, Symbol.for('d2.workerGuard.listeners'), { value: installParkListeners(W, target), configurable: true });
  if (typeof Module.ccall === 'function') Module.ccall = wrap(Module.ccall);
  Object.defineProperty(Module, MARK, { value: true });
  return true;
}
