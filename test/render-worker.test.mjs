import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createRenderWorkerCore } from '../play/render-worker.mjs';
import { createRenderHost } from '../play/render-host.mjs';
import { packTerrain } from '../play/frame-pack.mjs';
import { recordPrim } from '../play/assets/drawlist.mjs';

const tile = (i) => ({ room: i, layer: 0, ordinal: i, orientation: 0, mainIndex: 0, subIndex: 0, frameIndex: 0, worldX: i, worldY: i,
  screenX: 0, screenY: 0, width: 160, height: 80, flags: 0, librarySlot: 0, libraryIndex: 0, entryHint: -1, pop: 0 });
const fakeCanvas = (w, h) => ({ width: w, height: h, getContext: () => ({ tag: 'ctx' }) });
const draw = (ctx, frame) => { recordPrim('rect', frame.terrain.length, 2, 3, 4, 5, 6); };
const pk = (n = 2) => packTerrain(Array.from({ length: n }, (_, i) => tile(i)));

test('init, frame, close', () => {
  let seen;
  const core = createRenderWorkerCore({ createCanvas: fakeCanvas, draws: [(c, f, a) => { seen = [c, f, a]; draw(c, f); }] });
  assert.equal(core.handle({ type: 'init', width: 640, height: 480, assets: { k: 1 } }), null);
  const p = pk(3); p.wantList = true;
  const r = core.handle({ type: 'frame', seq: 7, packed: p });
  assert.equal(r.type, 'drawlist'); assert.equal(r.seq, 7); assert.equal(r.list.length, 1);
  assert.equal(seen[0].tag, 'ctx'); assert.equal(seen[1].terrain.length, 3); assert.deepEqual(seen[2], { k: 1 });
  assert.equal(globalThis.__d2DrawList, undefined);
  assert.equal(core.handle({ type: 'close' }), null);
  assert.equal(core.handle({ type: 'frame', seq: 8, packed: pk() }).type, 'error');
});
test('no list unless asked', () => {
  const core = createRenderWorkerCore({ createCanvas: fakeCanvas, draws: [draw] });
  core.handle({ type: 'init', width: 1, height: 1 });
  assert.equal(core.handle({ type: 'frame', seq: 1, packed: pk() }).list, null);
});
test('bad messages reply error, never throw', () => {
  const core = createRenderWorkerCore({ createCanvas: fakeCanvas, draws: [() => { throw new Error('boom'); }] });
  assert.equal(core.handle(null).type, 'error');
  assert.equal(core.handle({ type: 'nope', seq: 2 }).seq, 2);
  assert.equal(core.handle({ type: 'frame', seq: 3, packed: pk() }).type, 'error');   // before init
  core.handle({ type: 'init', width: 1, height: 1 });
  assert.equal(core.handle({ type: 'frame', seq: 4 }).type, 'error');
  const r = core.handle({ type: 'frame', seq: 5, packed: pk() });
  assert.deepEqual([r.type, r.seq, r.message], ['error', 5, 'boom']);
});

function fakeWorkerClass(core, { auto = true } = {}) {
  return class FakeWorker {
    constructor() { FakeWorker.last = this; this.posted = []; this.pending = []; }
    postMessage(msg, transfer) {
      const m = structuredClone(msg, { transfer });
      this.posted.push(m);
      if (!auto) { this.pending.push(m); return; }
      this.deliver(m);
    }
    deliver(m) { const r = core.handle(m); if (r) this.onmessage({ data: structuredClone(r) }); }
  };
}
const mainList = (n) => { const l = []; globalThis.__d2DrawList = l; recordPrim('rect', n, 2, 3, 4, 5, 6); globalThis.__d2DrawList = undefined; return l; };
function setup(opts, draws = [draw]) {
  const core = createRenderWorkerCore({ createCanvas: fakeCanvas, draws });
  const Worker = fakeWorkerClass(core, opts);
  const host = createRenderHost({ Worker, url: 'x', mode: 't' });
  host.init(10, 10, {});
  return { host, w: Worker.last };
}

test('seq matching either order, and transfer detaches', () => {
  const { host, w } = setup({ auto: false });
  w.deliver(w.pending[0]);
  const p = pk(2);
  host.post(1, p);
  assert.equal(p.buf.byteLength, 0);
  host.recordMain(1, mainList(2));          // main before the worker reply
  w.deliver(w.pending[1]);
  host.post(2, pk(3));
  w.deliver(w.pending[2]);                  // worker reply before main
  host.recordMain(2, mainList(3));
  assert.deepEqual([host.stats().frames, host.stats().matched, host.stats().mismatched], [2, 2, 0]);
});
test('a seq never posted, dropped or errored keeps no main list', () => {
  const { host } = setup({ auto: false }, []);
  host.recordMain(9, mainList(1));          // never posted
  [1, 2, 3, 4].forEach((s) => host.post(s, pk()));
  host.recordMain(4, mainList(1));          // dropped by the cap
  assert.equal(host.stats().pendingMain, 0);
  const e = setup({}, [() => { throw new Error('bad'); }]).host;
  e.post(1, pk());
  e.recordMain(1, mainList(1));             // errored
  assert.equal(e.stats().pendingMain, 0);
});
test('mismatch detected', () => {
  const { host } = setup();
  host.post(1, pk(2));
  host.recordMain(1, mainList(5));
  const s = host.stats();
  assert.equal(s.mismatched, 1); assert.equal(s.firstMismatchSeq, 1); assert.ok(s.firstDiff.diffs > 0);
});
test('in-flight cap drops and counts', () => {
  const { host, w } = setup({ auto: false });
  const sent = [1, 2, 3, 4, 5].map((s) => host.post(s, pk()));
  assert.deepEqual(sent, [true, true, true, false, false]);
  assert.equal(host.stats().dropped, 2);
  w.deliver(w.pending[1]);   // init is [0]; reply to frame 1
  assert.equal(host.stats().inFlight, 2);
  assert.equal(host.post(6, pk()), true);
});
test('worker error is counted', () => {
  const { host } = setup({}, [() => { throw new Error('bad'); }]);
  host.post(1, pk());
  assert.equal(host.stats().errors, 1); assert.equal(host.stats().firstError, 'bad');
});
