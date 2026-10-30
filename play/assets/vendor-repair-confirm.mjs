// The single-item Repair confirmation. Retail's D2Client 0x47500 (state 3) opens a Yes/No dialog titled
// with string 3351 ("Repair"); its Yes callback (0x6FAF7480) is what reaches the 0x35 send (0x46EE0).
// Nothing is sent from the item click itself: this module holds the pending item until Yes.
// Layout reuses the stash-gold dialog's background and OK/close buttons (the same buysellbtn cels).
import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl } from './frontend.mjs';
import { MOVE_GOLD_LAYOUT as L, STASH_GOLD_ART as ART } from './stash-gold-dialog.mjs';

export const REPAIR_TITLE_STRING = 3351;   // 0xd17 "Repair"
export const YES_STRING = 3344;            // 0xd10 "Yes"

/** Pending-item state machine (an item that does not need repair, per 0x45B40, never opens it): request() opens, yes() hands the item to send(), no() drops it. */
export function createRepairConfirm(send, needsRepair = () => true) {
  let pending = -1;
  return {
    get open() { return pending >= 0; },
    get itemId() { return pending; },
    request(itemId) { if (!(itemId >= 0) || pending >= 0 || !needsRepair(itemId)) return false; pending = itemId; return true; },
    yes() { if (pending < 0) return false; const id = pending; pending = -1; send(id); return true; },
    no() { const was = pending >= 0; pending = -1; return was; },
  };
}

function frameCanvas(frame, palette, doc) {
  const c = doc.createElement('canvas'); c.width = Math.max(1, frame.width); c.height = Math.max(1, frame.height);
  if (frame.width && frame.height) c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return c;
}
const rectOf = (x, bottom, f) => ({ x, y: bottom - f.height + 1, w: f.width, h: f.height });
const inside = (p, r) => p.x >= r.x && p.x < r.x + r.w && p.y >= r.y && p.y < r.y + r.h;
const origin = (w, h) => { const s = Math.min(1, w / 800, h / 600); return { s, x: (w - 800 * s) / 2, y: (h - 600 * s) / 2 }; };

export function createRepairConfirmRenderer(read, doc = globalThis.document, { string = () => '' } = {}) {
  const palette = decodeDatPalette(read(ART.palette));
  const dialog = decodeDc6(read(ART.dialog)), buttons = decodeDc6(read(ART.buttons));
  const font = createFont({ dc6: decodeDc6(read(ART.font)), tbl: decodeFontTbl(read(ART.fontTable)), palette, documentObject: doc,
    path: ART.font, drawAs: { op: 'celcolor', mode: 5 } });
  const bg = frameCanvas(dialog.frames[0], palette, doc);
  const btn = buttons.frames.map((f) => frameCanvas(f, palette, doc));
  const yes = () => rectOf(L.okX, L.buttonBottom, buttons.frames[L.okFrame]);
  const no = () => rectOf(L.closeX, L.buttonBottom, buttons.frames[L.closeFrame]);
  return {
    hitAt(px, py, width, height) {
      const o = origin(width, height), p = { x: (px - o.x) / o.s, y: (py - o.y) / o.s };
      return inside(p, yes()) ? 'yes' : inside(p, no()) ? 'no' : 'modal';
    },
    draw(ctx, width, height) {
      const o = origin(width, height);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.s, o.s);
      ctx.drawImage(bg, L.bgX, L.bgBottom - dialog.frames[0].height + 1);
      const y = yes(), n = no();
      ctx.drawImage(btn[L.closeFrame], n.x, n.y); ctx.drawImage(btn[L.okFrame], y.x, y.y);
      const title = string(REPAIR_TITLE_STRING) || 'Repair';
      font.draw(ctx, title, Math.trunc(L.promptCentreX - font.measure(title) / 2), L.promptBottoms[0] - 15);
      ctx.restore();
    },
  };
}
