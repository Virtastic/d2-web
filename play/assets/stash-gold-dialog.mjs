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
import { recordDc6 } from './drawlist.mjs';
import { INVENTORY_GOLD_ANCHOR } from './inventory-panel.mjs';
import { STASH_GOLD_COIN_ANCHOR } from './stash-panel.mjs';

export const STASH_GOLD = Object.freeze({ carried: 14, bank: 15, withdraw: 19, deposit: 20 });
export const STASH_GOLD_ART = Object.freeze({
  coin: 'data\\global\\ui\\PANEL\\goldcoinbtn.dc6',
  dialog: 'data\\global\\ui\\MENU\\dialogbackground.dc6',
  buttons: 'data\\global\\ui\\PANEL\\buysellbtn.dc6',
  spinner: 'data\\global\\ui\\menu\\spinner.dc6',
  palette: 'data\\global\\palette\\SKY\\pal.dat',
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTable: 'data\\local\\font\\latin\\font16.tbl',
});

// Retail 1.13c's own draw list for both dialogs (the retail captures, the stash coin;
// the retail captures, the inventory coin; captured 2026-09-29). Every cel is anchored by
// its BOTTOM row (a y of 298 for a 158-high cel is a top of 141), in retail's call order: background, close
// button (buysellbtn 10), OK button (buysellbtn 16), the typed amount and its caret, the two spinner arrows
// (menu\spinner.dc6 frames 0 and 1), then the two-line prompt, centred, its first line keeping the trailing
// space retail wraps after. This replaces OpenDiablo2's MoveGoldPanel offsets, which were 1.10f and 90 px off.
export const MOVE_GOLD_LAYOUT = Object.freeze({
  bgX: 215, bgBottom: 298,
  closeX: 355, okX: 250, buttonBottom: 287, okFrame: 16, closeFrame: 10,
  amountX: 259, amountBottom: 228,
  upX: 224, upBottom: 219, downX: 224, downBottom: 232,
  promptCentreX: 321.5, promptBottoms: [164, 188], promptWidth: 190,
});
export const CARET_BLINK_MS = 250;
const FONT16_TOP_TO_BOTTOM = 15;   // font.draw takes a glyph's TOP row; retail records its bottom row

function canvas(frame, palette, documentObject) {
  const c = documentObject.createElement('canvas'); c.width = Math.max(1, frame.width); c.height = Math.max(1, frame.height);
  if (frame.width && frame.height) c.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return c;
}

function origin(width, height) {
  const scale = Math.min(1, width / 800, height / 600);
  return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 };
}

function bottomRect(x, bottom, frame) { return { x, y: bottom - (frame?.height || 0) + 1, w: frame?.width || 0, h: frame?.height || 0 }; }
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
  const spinner = decodeDc6(read(STASH_GOLD_ART.spinner));
  const font = createFont({ dc6: decodeDc6(read(STASH_GOLD_ART.font)), tbl: decodeFontTbl(read(STASH_GOLD_ART.fontTable)), palette, documentObject,
    path: STASH_GOLD_ART.font, drawAs: { op: 'celcolor', mode: 5 } });
  const dialogFrames = dialog.frames.map((f) => canvas(f, palette, documentObject));
  const buttonFrames = buttons.frames.map((f) => canvas(f, palette, documentObject));
  const spinnerFrames = spinner.frames.map((f) => canvas(f, palette, documentObject));
  const dialogRgba = dialog.frames.map((f) => dc6FrameToRgba(f, palette));
  const L = MOVE_GOLD_LAYOUT;
  const now = () => (globalThis.performance?.now?.() ?? Date.now());
  // Greedy word wrap by pixel width; a line keeps the space it wraps after (retail's own first record).
  const wrap = (text) => {
    const words = text.split(' '), lines = []; let line = '';
    for (const w of words) {
      const next = line ? `${line}${w}` : w;
      if (line && font.measure(next) > L.promptWidth) { lines.push(line); line = `${w} `; } else line = `${next} `;
    }
    if (line) lines.push(line.replace(/ $/, lines.length ? '' : ''));
    return lines.map((l, i) => (i < lines.length - 1 ? l : l.trimEnd()));
  };
  const controls = () => ({
    // The stash panel's own coin, at the SAME anchor stash-panel.mjs draws it (F162) -- read from
    // STASH_GOLD_COIN_ANCHOR rather than derived from the panel geometry, which put the hit box 1 px
    // left of the cel.
    stash: () => bottomRect(STASH_GOLD_COIN_ANCHOR.x, STASH_GOLD_COIN_ANCHOR.bottom, coin.frames[0]),
    // The carried-gold coin the INVENTORY panel draws beside the purse (F162). It is the same
    // goldcoinbtn.dc6 frame 0 the stash panel draws, at retail's own anchor -- read from
    // inventory-panel.mjs's INVENTORY_GOLD_ANCHOR, the one place that rect lives, so the hit box and
    // the blitted cel cannot drift apart.
    inventory: () => bottomRect(INVENTORY_GOLD_ANCHOR.x, INVENTORY_GOLD_ANCHOR.bottom, coin.frames[0]),
    // Bottom-anchored cels: the rectangle's top is the recorded y minus the frame's height plus one.
    ok: () => bottomRect(L.okX, L.buttonBottom, buttons.frames[L.okFrame]),
    close: () => bottomRect(L.closeX, L.buttonBottom, buttons.frames[L.closeFrame]),
    up: () => bottomRect(L.upX, L.upBottom, spinner.frames[0]),
    down: () => bottomRect(L.downX, L.downBottom, spinner.frames[1]),
  });
  return {
    evidence: { art: Object.fromEntries(Object.entries({ coin, dialog, buttons, spinner }).map(([name, d]) => [name, { frames: d.frames.length, sizes: d.frames.map((f) => [f.width, f.height]) }])), layout: MOVE_GOLD_LAYOUT },
    hitAt(px, py, width, height, geometry, open) {
      const o = origin(width, height), point = { x: (px - o.x) / o.scale, y: (py - o.y) / o.scale }, c = controls();
      if (!open) {
        // F162: the two coins open the two dialogs retail's own captures show them opening. The
        // INVENTORY panel's coin (beside the carried gold) opens DEPOSIT; the stash panel's coin opens
        // WITHDRAW on a left click. Which button opens which was measured (the retail captures
        // inv-gold-deposit.txt, stash-gold-withdraw.txt); a right click on either coin is unmeasured,
        // so it does what the left click does rather than a second, invented behaviour.
        if (inside(point, c.inventory())) return { action: 'deposit' };
        return geometry && inside(point, c.stash()) ? { action: 'withdraw' } : null;
      }
      if (inside(point, c.ok())) return { action: 'submit' };
      if (inside(point, c.close())) return { action: 'close' };
      if (inside(point, c.up())) return { action: 'increase' };
      if (inside(point, c.down())) return { action: 'decrease' };
      return { action: 'modal' };
    },
    draw(ctx, width, height, geometry, stashOpen, state) {
      if (!stashOpen || !geometry) return null;
      const o = origin(width, height), c = controls(), stash = c.stash();
      const nativeCanvas = o.scale === 1 && o.x === 0 && o.y === 0;
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      // The stash renderer owns the single coin and bank-total blit.  This
      // overlay only hit-tests that retail chrome and paints a modal above it;
      // drawing the coin again would create a second gold display.
      let modal = null;
      let dialogIdentity = null;
      if (state?.open) {
        const bg = dialogFrames[0];
        const top = L.bgBottom - (bg?.height || 0) + 1;
        if (bg) { ctx.drawImage(bg, L.bgX, top); recordDc6(STASH_GOLD_ART.dialog, 0, dialog.frames[0], L.bgX, top); }
        dialogIdentity = blitIdentity(ctx, dialogRgba[0], { x: L.bgX, y: top, w: bg?.width || 0, h: bg?.height || 0 }, nativeCanvas);
        const ok = c.ok(), close = c.close();
        // Retail's call order (the draw lists above): close, OK, amount + caret, spinners, prompt.
        if (buttonFrames[L.closeFrame]) { ctx.drawImage(buttonFrames[L.closeFrame], close.x, close.y); recordDc6(STASH_GOLD_ART.buttons, L.closeFrame, buttons.frames[L.closeFrame], close.x, close.y); }
        if (buttonFrames[L.okFrame]) { ctx.drawImage(buttonFrames[L.okFrame], ok.x, ok.y); recordDc6(STASH_GOLD_ART.buttons, L.okFrame, buttons.frames[L.okFrame], ok.x, ok.y); }
        const actionKey = state.mode === 'withdraw' ? 'strBankGoldWithdraw' : 'strBankGoldDeposit';
        const action = string(actionKey) || actionKey;
        const amount = String(Math.max(0, state.amount | 0));
        const value = Math.max(0, state.available | 0);
        // A zero amount is an empty field: retail's withdraw dialog opens on nothing but the caret.
        const shown = state.amount > 0 ? amount : '';
        const T = FONT16_TOP_TO_BOTTOM;
        font.draw(ctx, shown, L.amountX, L.amountBottom - T);
        const caretLit = Math.floor(now() / CARET_BLINK_MS) % 2 === 0;
        if (caretLit) font.draw(ctx, '_', L.amountX + font.measure(shown), L.amountBottom - T);
        const up = c.up(), down = c.down();
        if (spinnerFrames[0]) { ctx.drawImage(spinnerFrames[0], up.x, up.y); recordDc6(STASH_GOLD_ART.spinner, 0, spinner.frames[0], up.x, up.y); }
        if (spinnerFrames[1]) { ctx.drawImage(spinnerFrames[1], down.x, down.y); recordDc6(STASH_GOLD_ART.spinner, 1, spinner.frames[1], down.x, down.y); }
        const lines = wrap(action);
        lines.forEach((line, i) => font.draw(ctx, line, Math.trunc(L.promptCentreX - font.measure(line) / 2), L.promptBottoms[i] - T));
        modal = { mode: state.mode, amount: state.amount, available: value, actionKey, action, controls: { ok, close, up: c.up(), down: c.down() } };
      }
      ctx.restore();
      const identity = { dialog: dialogIdentity };
      return { bank: state?.bank | 0, stashButton: stash, inventoryButton: c.inventory(), coin: STASH_GOLD_ART.coin, modal,
        drawn: { coinFrame: 0, coinFrames: coin.frames.length, dialogFrame: state?.open ? 0 : null, dialogFrames: dialog.frames.length,
          amount: state?.open ? state.amount : null, identity } };
    },
  };
}
