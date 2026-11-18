// Installs a stand-in for the d2_client_automap_layout engine export (D2Client+0x5F9F0 / 0x608D0, src/core/d2client/automap_layout.cpp)
// for tests that run without the wasm engine. The arithmetic is the same one automap-layout.test.mjs checks the real export against.
import { bindAutomapLayout } from '../play/assets/automap-overlay.mjs';
const heap = new Uint8Array(4096);
bindAutomapLayout({
  HEAPU8: heap,
  _malloc: () => 64,
  _d2_client_automap_layout() {},
  ccall(_name, _ret, _types, [w, h, mode, left, sel, center, at]) {
    const mini = mode === 1, ax = left ? 0 : Math.floor(2 * w / 3), ay = left ? (sel === 2 ? 0 : 96) : 78;
    const vp = mini ? [ax - 8, ay, ax + Math.floor(w / 3), ay + Math.floor(h / 3)] : [-16, -16, w, h];
    const sc = mini && center ? [Math.floor(w / 3) - ax - 16, Math.floor(h / 3) - ay - 16] : [0, 0];
    new Int32Array(heap.buffer, Number(at), 8).set([ax, ay, ...vp, ...sc]);
  },
});
