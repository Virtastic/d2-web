// Retail's stash-gold affordance and its amount dialog.
//
// Prior art used: riiablo's StashPanel puts goldcoinbtn at (74,392) and the
// stash amount at (98,393), in the 320x432 TradeStash panel's bottom-origin
// coordinates. OpenDiablo2's MoveGoldPanel supplies dialogbackground.dc6,
// the SKY palette, and the dialog controls at 300,350 / 335,308 / 440,308.
// The engine is still the authority: this module only reports a hit and draws
// values supplied by gameplay-shell after it rereads the player stats.

import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { createFont, decodeFontTbl } from './frontend.mjs';

export const STASH_GOLD = Object.freeze({ carried: 14, bank: 15, withdraw: 19, deposit: 20 });
export const STASH_GOLD_ART = Object.freeze({
  coin: 'data\\global\\ui\\PANEL\\goldcoinbtn.dc6',
  dialog: 'data\\global\\ui\\MENU\\dialogbackground.dc6',
  buttons: 'data\\global\\ui\\PANEL\\buysellbtn.dc6',
  palette: 'data\\global\\palette\\SKY\\pal.dat',
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTable: 'data\\local\\font\\latin\\font16.tbl',
});

// Screen coordinates from OpenDiablo2 MoveGoldPanel. The background's exact
// top-left follows its DC6 dimensions; its control coordinates are retained
// as source-3 layout until the retail dialog draw list is captured.
export const MOVE_GOLD_LAYOUT = Object.freeze({
  panelX: 300, panelY: 350, valueX: 329, valueY: 260,
  actionX: 405, actionY: 200, actionPitch: 25,
  upX: 314, upY: 259, downX: 314, downY: 274,
  okX: 335, okY: 308, closeX: 440, closeY: 308,
});

function canvas(frame, palette, documentObject) {
  const c = documentObject.createElement('canvas'); c.width = Math.max(1, frame.width); c.height = Math.max(1, frame.height);
  if (frame.width && frame.height) c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return c;
}

function origin(width, height) {
  const scale = Math.min(1, width / 800, height / 600);
  return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 };
}

function rectAt(x, y, frame) { return { x, y, w: frame?.width || 0, h: frame?.height || 0 }; }
function inside(point, rect) { return point.x >= rect.x && point.x < rect.x + rect.w && point.y >= rect.y && point.y < rect.y + rect.h; }

// This is deliberately a per-pixel identity check, rather than a canvas hash.
// It checks every opaque source pixel at the exact rectangle just blitted.  It
// runs before gameplay-shell applies the display ramp, because the DC6 decoder
// and the 2D canvas agree on the un-ramped palette values at this point.
function blitIdentity(ctx, rgba, rect, nativeCanvas) {
  if (!rgba || !nativeCanvas || rect.x < 0 || rect.y < 0 || rect.x + rect.w > ctx.canvas.width || rect.y + rect.h > ctx.canvas.height) {
    return { tested: 0, mismatches: 0, skipped: true, rect };
  }
  const actual = ctx.getImageData(rect.x, rect.y, rect.w, rect.h).data;
  let tested = 0, mismatches = 0;
  for (let p = 0; p < rgba.length; p += 4) {
    if (!rgba[p + 3]) continue;
    tested++;
    if (actual[p] !== rgba[p] || actual[p + 1] !== rgba[p + 1] || actual[p + 2] !== rgba[p + 2] || actual[p + 3] !== rgba[p + 3]) mismatches++;
  }
  return { tested, mismatches, skipped: false, rect };
}

export function createStashGoldRenderer(read, documentObject = globalThis.document, { string = () => '' } = {}) {
  const palette = decodeDatPalette(read(STASH_GOLD_ART.palette));
  const coin = decodeDc6(read(STASH_GOLD_ART.coin));
  const dialog = decodeDc6(read(STASH_GOLD_ART.dialog));
  const buttons = decodeDc6(read(STASH_GOLD_ART.buttons));
  const font = createFont({ dc6: decodeDc6(read(STASH_GOLD_ART.font)), tbl: decodeFontTbl(read(STASH_GOLD_ART.fontTable)), palette, documentObject,
    path: STASH_GOLD_ART.font, drawAs: { op: 'celcolor', mode: 5 } });
  const dialogFrames = dialog.frames.map((f) => canvas(f, palette, documentObject));
  const buttonFrames = buttons.frames.map((f) => canvas(f, palette, documentObject));
  const dialogRgba = dialog.frames.map((f) => dc6FrameToRgba(f, palette));
  const L = MOVE_GOLD_LAYOUT;
  const controls = () => ({
    // Riiablo positions controls in bottom-origin TradeStash local space.
    stash: (geometry) => {
      const f = coin.frames[0];
      return rectAt(geometry.left + 74, geometry.top + 432 - 392 - (f?.height || 0), f);
    },
    // buysellbtn frame 0 is the check and frame 10 is close (OpenDiablo2's
    // ButtonTypeSquareOk/Close base frames).
    ok: () => rectAt(L.okX, L.okY, buttons.frames[0]),
    close: () => rectAt(L.closeX, L.closeY, buttons.frames[10]),
    up: () => ({ x: L.upX, y: L.upY, w: 20, h: 15 }),
    down: () => ({ x: L.downX, y: L.downY, w: 20, h: 15 }),
  });
  return {
    evidence: { art: Object.fromEntries(Object.entries({ coin, dialog, buttons }).map(([name, d]) => [name, { frames: d.frames.length, sizes: d.frames.map((f) => [f.width, f.height]) }])), layout: MOVE_GOLD_LAYOUT },
    hitAt(px, py, width, height, geometry, open) {
      const o = origin(width, height), point = { x: (px - o.x) / o.scale, y: (py - o.y) / o.scale }, c = controls();
      if (!open) return geometry && inside(point, c.stash(geometry)) ? { action: 'open' } : null;
      if (inside(point, c.ok())) return { action: 'submit' };
      if (inside(point, c.close())) return { action: 'close' };
      if (inside(point, c.up())) return { action: 'increase' };
      if (inside(point, c.down())) return { action: 'decrease' };
      return { action: 'modal' };
    },
    draw(ctx, width, height, geometry, stashOpen, state) {
      if (!stashOpen || !geometry) return null;
      const o = origin(width, height), c = controls(), stash = c.stash(geometry);
      const nativeCanvas = o.scale === 1 && o.x === 0 && o.y === 0;
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      // The stash renderer owns the single coin and bank-total blit.  This
      // overlay only hit-tests that retail chrome and paints a modal above it;
      // drawing the coin again would create a second gold display.
      let modal = null;
      let dialogIdentity = null;
      if (state?.open) {
        const bg = dialogFrames[0];
        // OpenDiablo2 positions this sprite at 300,350; D2 UI cels use their
        // bottom row as the anchor, so convert to a canvas top edge once.
        const top = L.panelY - (bg?.height || 0) + 1;
        if (bg) ctx.drawImage(bg, L.panelX, top);
        dialogIdentity = blitIdentity(ctx, dialogRgba[0], { x: L.panelX, y: top, w: bg?.width || 0, h: bg?.height || 0 }, nativeCanvas);
        const ok = c.ok(), close = c.close();
        if (buttonFrames[0]) ctx.drawImage(buttonFrames[0], ok.x, ok.y);
        if (buttonFrames[10]) ctx.drawImage(buttonFrames[10], close.x, close.y);
        const actionKey = state.mode === 'withdraw' ? 'strBankGoldWithdraw' : 'strBankGoldDeposit';
        const action = string(actionKey) || actionKey;
        const amount = String(Math.max(0, state.amount | 0));
        const value = Math.max(0, state.available | 0);
        font.draw(ctx, action, L.actionX - Math.floor(font.measure(action) / 2), L.actionY);
        font.draw(ctx, amount, L.valueX, L.valueY);
        // Arrow geometry is sourced from OpenDiablo2; retail arrow art is not
        // yet identified, so the controls remain visibly declared in docs.
        ctx.fillStyle = '#c8b479'; ctx.fillText('▲', L.upX, L.upY + 12); ctx.fillText('▼', L.downX, L.downY + 12);
        modal = { mode: state.mode, amount: state.amount, available: value, actionKey, action, controls: { ok, close, up: c.up(), down: c.down() } };
      }
      ctx.restore();
      const identity = { dialog: dialogIdentity };
      return { bank: state?.bank | 0, stashButton: stash, coin: STASH_GOLD_ART.coin, modal,
        drawn: { coinFrame: 0, coinFrames: coin.frames.length, dialogFrame: state?.open ? 0 : null, dialogFrames: dialog.frames.length,
          amount: state?.open ? state.amount : null, identity } };
    },
  };
}
