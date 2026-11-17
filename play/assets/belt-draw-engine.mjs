// The belt's draw list, from the engine: D2Client+0x28DC0 (d2_client_belt_draw) decides which popup frames, item cels and
// hotkey labels the belt draws and where; this file only packs the shell's belt state in and unpacks the ops out.
// ops: { op: 'bg', x, y } | { op: 'item', item, x, y } | { op: 'text', text, x, y, color } in the belt's own (800x600) coordinates.
import { decodeFontTbl } from './frontend.mjs';

export const BELT_FONT = 'data\\local\\font\\latin\\font16';
const MAX_ITEMS = 16, MAX_BOXES = 16, OPS_CAP = 1024, KEY_UNITS = 15;

// Makes the packer for one wasm module. `read(path)` is the shell's asset reader (the font's advance widths).
export function createBeltDraw(Module, read) {
  if (!Module?._d2_client_belt_draw) return null;
  const alloc = (n) => Number(Module.ccall('malloc', 'bigint', ['number'], [n]));
  const inLen = 6 + 4 * MAX_BOXES + MAX_ITEMS + 4 * (KEY_UNITS + 1);
  let inPtr = 0, opsPtr = 0, advPtr = 0;
  const advances = () => {
    if (advPtr) return advPtr;
    try {
      const adv = new Uint8Array(65536);
      for (const [code, g] of decodeFontTbl(read(BELT_FONT + '.tbl')).chars) adv[code] = g.width;
      advPtr = alloc(65536);
      Module.HEAPU8.set(adv, advPtr);
    } catch { advPtr = 0; }
    return advPtr;
  };
  return {
    // boxes: the worn record's every Belts.txt box (bottom row first); items: [{ gridX, invFile }]; keys: the four belt keys
    // (string or null). Returns the ops, or null when the engine has nothing to draw (no boxes, or a callee it could not answer).
    draw({ w, h, record, open, boxes, items, keys }) {
      if (!boxes?.length || boxes.length > MAX_BOXES) return null;
      const used = items.slice(0, MAX_ITEMS);
      inPtr ||= alloc(inLen * 4); opsPtr ||= alloc(OPS_CAP * 4);
      const adv = advances();
      if (!adv) return null;
      const v = new Int32Array(inLen);
      let k = 0;
      v[k++] = w; v[k++] = h; v[k++] = record; v[k++] = open ? 1 : 0; v[k++] = boxes.length; v[k++] = used.length;
      for (const b of boxes) { v[k++] = b.left; v[k++] = b.right; v[k++] = b.top; v[k++] = b.bottom; }
      for (const it of used) v[k++] = it.gridX;
      for (let i = 0; i < 4; i++) {
        const key = keys?.[i];
        const at = k; k += KEY_UNITS + 1;
        if (!key) { v[at] = -1; continue; }
        const n = Math.min(key.length, KEY_UNITS);
        v[at] = n;
        for (let j = 0; j < n; j++) v[at + 1 + j] = key.charCodeAt(j);
      }
      Module.HEAP32.set(v.subarray(0, k), inPtr >> 2);
      const n = Module._d2_client_belt_draw(BigInt(inPtr), BigInt(adv), BigInt(opsPtr), OPS_CAP);
      if (n < 0) return null;
      const o = Module.HEAP32.subarray(opsPtr >> 2, (opsPtr >> 2) + n);
      return parseBeltOps(o);
    },
  };
}

export function parseBeltOps(o) {
  const out = [];
  for (let i = 0; i < o.length;) {
    const kind = o[i];
    if (kind === 1) { out.push({ op: 'bg', x: o[i + 1], y: o[i + 2] }); i += 3; }
    else if (kind === 2) { out.push({ op: 'item', item: o[i + 1], x: o[i + 2], y: o[i + 3] }); i += 4; }
    else if (kind === 3) {
      const len = o[i + 4];
      out.push({ op: 'text', text: String.fromCharCode(...o.subarray(i + 5, i + 5 + len)), x: o[i + 1], y: o[i + 2], color: o[i + 3] });
      i += 5 + len;
    } else throw new Error(`belt op ${kind}`);
  }
  return out;
}
