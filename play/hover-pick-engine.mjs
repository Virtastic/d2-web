// Platform seam for D2Client 0x51E80: marshals the browser's hover candidates into the engine's d2_client_hover_pick
// (src/core/d2client/hover_pick_51e80.cpp) and reads the winner back. No selection rule lives here.
import { objectDoorFields } from './assets/object-hover-label.mjs';

const ROW = 9;
export function createHoverPickBackend(Module) {
  if (!Module?._d2_client_hover_pick || !Module._malloc) return null;
  return function pick(rows, reference, storedIndex) {
    const n = Math.min(rows.length, 256);
    if (!n) return -1;
    const p = Module._malloc(n * ROW * 4);
    try {
      const w = new Int32Array(Module.HEAP32.buffer, p, n * ROW);
      for (let i = 0; i < n; i++) {
        const r = rows[i], o = i * ROW;
        const door = r.type === 2 ? objectDoorFields(Module, r.classId) : { isDoor: 0, orient: 0 };
        w[o] = r.type; w[o + 1] = r.mode | 0; w[o + 2] = r.flags | 0; w[o + 3] = Math.round(r.x) | 0; w[o + 4] = Math.round(r.y) | 0;
        w[o + 5] = 1; w[o + 6] = r.attached ? 1 : 0; w[o + 7] = door.isDoor; w[o + 8] = door.orient;
      }
      return Module.ccall('d2_client_hover_pick', 'number', ['bigint', 'number', 'number', 'number', 'number', 'number'],
        [BigInt(p), n, Math.round(Number(reference?.x) || 0) | 0, Math.round(Number(reference?.y) || 0) | 0, storedIndex, 0]);
    } finally { Module._free(p); }
  };
}
