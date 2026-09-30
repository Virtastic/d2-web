// The stash: retail's personal storage in town, opened by clicking the stash object.
//
// Nothing here decides anything. The window is open because the ENGINE sent retail's 0x77
// UPDATEUI_OPENSTASH (D2MOO's OBJECTS_OperateFunction32_Bank, vendored), it closes because the
// engine answered TRADEBTN_CLOSESTASH, and the grid is the Inventory.txt record retail itself uses
// for a Lord of Destruction character -- INVENTORYRECORD_BIG_BANK_PAGE_1 (Units.cpp: bLoD ? BIG_BANK
// : BANK), read through the engine's own accessor (d2_session_inventory_layout). One position is
// NOT from retail data -- the art's top edge, LEFT_PANEL_TOP below -- and it is declared as such.
//
// The art is data\global\ui\PANEL\TradeStash.dc6. OpenDiablo2 stalled before stashes, so there is
// no layout prior art; its frames are composed the way the other retail panels' are (a 2x2 set
// when it carries four), and the composition is REPORTED so a scenario can check it rather than
// trust it.

import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { inventoryItemAsset } from './inventory-panel.mjs';
import { BORDER_LEFT, BORDER_RIGHT, CHROME_ART, createPanelChrome } from './panel-chrome.mjs';

export const STASH_ART = 'data\\global\\ui\\PANEL\\TradeStash.dc6';
export const INVPAGE_STASH = 4;
// F162: the stash panel's own gold coin -- goldcoinbtn.dc6 frame 0, x left / y = the cel's BOTTOM row, at
// retail's own draw-list anchor (the retail captures `goldcoinbtn.dc6 frame=0 x=155
// y=100`). Exported so the stash-gold hit test uses this exact rect instead of re-deriving it from the
// panel geometry: geometry.left+74 was 154 while the cel is drawn at 155, so a click on the coin's right
// column missed the drawn coin.
export const STASH_GOLD_COIN_ANCHOR = Object.freeze({ x: 155, bottom: 100 });
// The Horadric Cube's grid is page 3 and Inventory.txt row TRANSMOGRIFY_BOX_PAGE_1 (D2Inventory.h); its window
// reuses this renderer -- the same TradeStash art (the only 141012-byte trade panel the MPQs carry: no cube-specific
// DC6 exists under PANEL) with the cube's own grid.
export const INVPAGE_CUBE = 3;
export const TRANSMOGRIFY_BOX_PAGE_1 = 9;
// Inventory.txt row for a Lord of Destruction stash, and the 800x600 flag (row + 16 * higherRes).
export const BIG_BANK_PAGE_1 = 12;

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (frame.width && frame.height)
    canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

// Where the ART is anchored. The record's grid is exact, but its panel rectangle is not the art's:
// BIG_BANK_PAGE_1 reports a panel top of 280 -- the same rectangle as the trade screen's lower
// half -- while its own grid starts at y 142, so 280 cannot be where the stash art begins. The art
// is placed at the left-panel origin the trade screen uses (x = the record's left, y = 60), and the
// frame is LOOKED AT to confirm the grid falls on the art's cells (s000 captures it). Declared in
// until a retail draw list (OBJECTIVE v5 E) gives the number directly.
// Retail's draw list: TradeStash frames anchored (80,316), (336,316),
// (80,492), (336,492) -- the art's top is y 61.
export const LEFT_PANEL_TOP = 61;

/**
 * `layout` is d2_session_inventory_layout's twelve numbers for BIG_BANK_PAGE_1 at 800x600:
 * [left, right, top, bottom, columns, rows, gridLeft, gridRight, gridTop, gridBottom, boxW, boxH].
 */
export function stashGeometry(layout) {
  if (!Array.isArray(layout) || layout.length < 12) return null;
  const [left, right, top, bottom, columns, rows, gridLeft, , gridTop, , boxW, boxH] = layout;
  if (!(right > left && bottom > top && columns > 0 && rows > 0 && boxW > 0 && boxH > 0)) return null;
  return { left, top: LEFT_PANEL_TOP, recordTop: top, columns, rows, gridLeft, gridTop, boxW, boxH };
}

// In-game panels are drawn with the ACT's palette.
const actPalette = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;

export function createStashPanelRenderer(read, documentObject = globalThis.document, { string = null, stashGold = null, page = INVPAGE_STASH, transmute = null } = {}) {
  const chrome = createPanelChrome(read, documentObject);
  let dc6 = decodeDc6(read(STASH_ART));
  const perAct = new Map();
  const plugy = { on: false, pageName: null };   // PY3: ?plugy=1 swaps the art for PlugY's TradeStash.dc6 and names the page
  const forAct = (act) => {
    let a = perAct.get(act);
    if (!a) {
      const palette = decodeDatPalette(read(actPalette(act)));
      a = { palette, frames: dc6.frames.map((f) => frameCanvas(f, palette, documentObject)), icons: new Map() };
      perAct.set(act, a);
    }
    return a;
  };
  const icon = (a, invFile) => {
    let c = a.icons.get(invFile);
    if (!c) { c = frameCanvas(decodeDc6(read(inventoryItemAsset(invFile))).frames[0], a.palette, documentObject); a.icons.set(invFile, c); }
    return c;
  };
  const origin = (width, height) => {
    const scale = Math.min(1, width / 800, height / 600);
    return { scale, x: (width - 800 * scale) / 2, y: (height - 600 * scale) / 2 };
  };

  // Level 1 (declared, not retail-measured): the Transmute button sits under the cube's grid, its width.
  const buttonRect = (g) => ({ x: g.gridLeft, y: g.gridTop + g.rows * g.boxH + 10, w: g.columns * g.boxW, h: 24 });
  return {
    /** Whether a canvas point is on the Transmute button (cube window only). */
    transmuteAt(px, py, width, height, geometry) {
      if (!transmute || !geometry) return false;
      const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale, b = buttonRect(geometry);
      return x >= b.x && x < b.x + b.w && y >= b.y && y < b.y + b.h;
    },
    plugy,
    /** Font16 helpers for PlugY's hover popups (plugy-stash-ui.mjs). */
    text: { measure: (act, str) => chrome.measure(act, str), draw: (ctx, act, str, x, y) => chrome.text(ctx, act, str, x, y) },
    /** PY3: replace the panel art (PlugY's own TradeStash.dc6 bytes). */
    setArt(bytes) { dc6 = decodeDc6(bytes); perAct.clear(); },
    get evidence() { return { frames: dc6.frames.length, sizes: dc6.frames.map((f) => [f.width, f.height]) }; },
    /**
     * The panel's own rectangle on the 800x600 screen -- the art's composed size at its anchor
     * (LEFT_PANEL_TOP). A click inside it belongs to the stash and never falls through to the world.
     */
    panelRect(geometry) {
      if (!geometry) return null;
      const w = dc6.frames.length === 4 ? dc6.frames[0].width + dc6.frames[1].width : (dc6.frames[0]?.width || 0);
      const h = dc6.frames.length === 4 ? dc6.frames[0].height + dc6.frames[2].height : (dc6.frames[0]?.height || 0);
      return { left: geometry.left, top: geometry.top, width: w, height: h };
    },
    /** Whether the pointer is over the stash panel at all. */
    contains(px, py, width, height, geometry) {
      const r = this.panelRect(geometry);
      if (!r) return false;
      const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
      return x >= r.left && x < r.left + r.width && y >= r.top && y < r.top + r.height;
    },
    /**
     * Which stashed item is under a canvas point, or null. An item is covered by its WHOLE footprint
     * (ItemsTxt nInvWidth/nInvHeight), the same rule the vendor grid got in an earlier change: matching only
     * the top-left cell makes a large item's body dead to the pointer.
     */
    itemAt(px, py, width, height, geometry, items) {
      if (!geometry) return null;
      const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
      const g = geometry;
      return [...(items || [])].reverse().find((i) => i.invPage === page && i.invFile &&
        x >= g.gridLeft + i.gridX * g.boxW && x < g.gridLeft + (i.gridX + (i.invWidth || 1)) * g.boxW &&
        y >= g.gridTop + i.gridY * g.boxH && y < g.gridTop + (i.gridY + (i.invHeight || 1)) * g.boxH) || null;
    },
    /**
     * The stash cell a cursor item would be put down at, the item centred on the pointer as retail
     * holds it; null off the grid. The inverse of itemAt, so a click cannot land on a cell other than
     * the one the player sees the item over.
     */
    cellAt(px, py, width, height, geometry, item) {
      if (!geometry) return null;
      const o = origin(width, height), x = (px - o.x) / o.scale, y = (py - o.y) / o.scale;
      const g = geometry, w = item?.invWidth || 1, h = item?.invHeight || 1;
      if (x < g.gridLeft || y < g.gridTop || x >= g.gridLeft + g.columns * g.boxW || y >= g.gridTop + g.rows * g.boxH) return null;
      const cx = Math.round((x - g.gridLeft) / g.boxW - w / 2), cy = Math.round((y - g.gridTop) / g.boxH - h / 2);
      return { x: Math.max(0, Math.min(g.columns - w, cx)), y: Math.max(0, Math.min(g.rows - h, cy)) };
    },
    draw(ctx, width, height, geometry, items, open, act = 1) {
      if (!open || !geometry) return null;
      const a = forAct(act), frames = a.frames, o = origin(width, height);
      ctx.save(); ctx.translate(o.x, o.y); ctx.scale(o.scale, o.scale);
      // Panel art at the left-panel origin (see LEFT_PANEL_TOP).
      let composed = { width: 0, height: 0 };
      if (frames.length === 4) {
        const [tl, tr, bl, br] = frames;
        ctx.drawImage(tl, geometry.left, geometry.top);
        ctx.drawImage(tr, geometry.left + tl.width, geometry.top);
        ctx.drawImage(bl, geometry.left, geometry.top + tl.height);
        ctx.drawImage(br, geometry.left + tl.width, geometry.top + tl.height);
        composed = { width: tl.width + tr.width, height: tl.height + bl.height };
      } else if (frames.length) {
        ctx.drawImage(frames[0], geometry.left, geometry.top);
        composed = { width: frames[0].width, height: frames[0].height };
      }
      // Items the engine says are on the stash page, at the grid cell the engine reports.
      const drawn = [];
      for (const item of items || []) {
        if (item.invPage !== page || !item.invFile) continue;
        const c = icon(a, item.invFile);
        const x = geometry.gridLeft + item.gridX * geometry.boxW, y = geometry.gridTop + item.gridY * geometry.boxH;
        ctx.drawImage(c, x, y);
        drawn.push({ id: item.id, invFile: item.invFile, cell: [item.gridX, item.gridY], at: [x, y] });
      }
      // Gold in the stash beside the coin button, the limit below it (string 4051 "Gold Max: %d"), the close
      // button, and the left border -- retail's anchors.
      if (stashGold) {
        const inStash = String(stashGold(0)), cx = 245;   // one glyph captured: left or right alignment not yet known
        chrome.cel(ctx, act, CHROME_ART.gold, 0, STASH_GOLD_COIN_ANCHOR.x, STASH_GOLD_COIN_ANCHOR.bottom);
        chrome.text(ctx, act, inStash, cx, 100);
        if (plugy.pageName) { if (plugy.nameColour) chrome.textIn(ctx, act, plugy.pageName, plugy.nameX, plugy.nameY, plugy.nameColour); else chrome.text(ctx, act, plugy.pageName, plugy.nameX, plugy.nameY); }
        else if (string) chrome.text(ctx, act, string(4051).replace('%d', String(stashGold(1))), 158, 125);
      }
      if (transmute) {
        const b = buttonRect(geometry);
        ctx.fillStyle = 'rgba(40,30,20,0.85)'; ctx.fillRect(b.x, b.y, b.w, b.h);
        ctx.strokeStyle = '#8a7a52'; ctx.strokeRect(b.x + 0.5, b.y + 0.5, b.w - 1, b.h - 1);
        chrome.text(ctx, act, transmute, b.x + 8, b.y + 18);
      }
      chrome.cel(ctx, act, CHROME_ART.close, 10, 352, 476);
      // The stash opens with the inventory; retail frames both after both panels, left then right.
      chrome.border(ctx, act, BORDER_LEFT); chrome.border(ctx, act, BORDER_RIGHT);
      ctx.restore();
      return { page, anchor: [geometry.left, geometry.top], recordTop: geometry.recordTop, palette: actPalette(act), open: true, composed, frames: frames.length, grid: { columns: geometry.columns, rows: geometry.rows,
        left: geometry.gridLeft, top: geometry.gridTop, box: [geometry.boxW, geometry.boxH] }, drawn };
    },
  };
}
