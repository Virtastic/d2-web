// The NPC window: what an NPC says, and what they will sell you.
//
// Everything shown here comes from the engine, which has had all of it proved for several cycles
// (s79-s85, s98, s105) while the client had no window to put it in -- the exact "the engine
// computes it and nothing draws it" defect calls out.
//
//   * the greeting text is the retail string for the id the dialogue packet carries
//     (d2_session_npc_dialog -> stringIds -> d2_lang_string_by_index);
//   * the stock is the NPC's real inventory (d2_session_vendor_items), each row named by its item
//     code through the retail string tables and priced by ITEMS_GetTransactionCost.
//
// Nothing here invents an item, a name or a price.

import { createFont, decodeFontTbl, FRONTEND } from './frontend.mjs';
import { inventoryItemAsset } from './inventory-panel.mjs';

export const NPC_PANEL_ASSETS = {
  // In-game panels are drawn on the act palette, like the control panel and inventory.
  palette: 'data\\global\\palette\\ACT1\\pal.dat',
  background: 'data\\global\\ui\\panel\\trade.dc6',
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTbl: 'data\\local\\font\\latin\\font16.tbl',
};

// trade.dc6 is one image cut into 256px-wide columns, like the front-end screens.
const BACKGROUND_COLUMNS = 2;

// The trade window's real geometry, agreed by two independent readings:
//
//   * Inventory.txt rows 6 and 7 through retail's own accessors (s159): panel 80,401,60,502, with
//     10x4 grids at screen 100,385,101,208 and 100,385,315,428, cells 29x29.
//   * The trade.dc6 art itself: composing its four frames gives 320x432, and the dark cell bands
//     land at columns from x=20 on a 29px pitch and rows at y 39..152 and y 255..368.
//
// Panel-relative those agree to a pixel or two, which is how both are known to be right. The upper
// grid is the SHOP's stock and the lower one the player's -- this draws the upper.
//
// The panel sits at screen (80, 60). It had been centred, and the stock had been a text list whose
// 17px rows scaled item sprites down to 8x16 -- painted, counted by the row that "passed", and
// invisible on screen. A list cannot show item art; retail draws a grid, so this draws a grid.
export const TRADE_LAYOUT = Object.freeze({
  screenX: 80, screenY: 60, width: 320, height: 432,
  gridX: 20, gridY: 41, columns: 10, rows: 4, cell: 29,
  // The lower grid is the PLAYER's items -- the other half of a trade screen, and what you click to
  // sell. Same derivation: Inventory.txt row 7 puts it at screen y 315 (panel-relative 255), and
  // trade.dc6's lower cell bands run y 255..368.
  playerGridY: 255,
});
// There is no name slot and no greeting line in retail's trade window, and no gold line of our own:
// this panel used to draw an NPC name at (18,26), a wrapped greeting at (18,50) and "GOLD n" against
// its right edge, all hand-picked. Retail's own draw list of Akara's open
// shop carries exactly two pieces of text on this panel -- the
// STASH label at screen (101,434) and the player's gold at (266,434), both Font16 -- plus the tab
// labels, and retail/frames/ + drawlists/vendor-akara.png show the same window: no name, no greeting.
// The greeting is the SPEECH BOX's (speech-box.mjs), which is the panel retail shows it in. So the
// only text this fallback draws is the gold, at the anchor retail draws it at.
// (npc-panel.mjs is the superseded trade window -- vendor-panel.mjs draws the live one and s250 pins
// it against retail's frame; this path is reached only when the vendor panel cannot be built.)
const GOLD_ANCHOR = Object.freeze({ x: 186, y: 374 });   // screen (266,434) - the panel's (80,60)
function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width);
  canvas.height = Math.max(1, frame.height);
  if (!frame.width || !frame.height) return canvas;
  const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
  for (let i = 0; i < frame.indices.length; i++) {
    if (!frame.opaque[i]) continue;
    const s = frame.indices[i] * 4;
    rgba.set(palette.subarray(s, s + 4), i * 4);
  }
  canvas.getContext('2d').putImageData(new ImageData(rgba, frame.width, frame.height), 0, 0);
  return canvas;
}

function composeTiled(dc6, palette, columns, documentObject) {
  const rows = Math.ceil(dc6.frames.length / columns);
  let width = 0, height = 0;
  for (let c = 0; c < columns; c++) width += dc6.frames[c]?.width ?? 0;
  for (let r = 0; r < rows; r++) height += dc6.frames[r * columns]?.height ?? 0;
  const canvas = documentObject.createElement('canvas');
  canvas.width = width; canvas.height = height;
  const ctx = canvas.getContext('2d');
  let y = 0;
  for (let r = 0; r < rows; r++) {
    let x = 0;
    for (let c = 0; c < columns; c++) {
      const frame = dc6.frames[r * columns + c];
      if (!frame) continue;
      ctx.drawImage(frameCanvas(frame, palette, documentObject), x, y);
      x += frame.width;
    }
    y += dc6.frames[r * columns]?.height ?? 0;
  }
  return { canvas, width, height, rows, columns };
}

/**
 * `read` is the MPQ reader. The panel holds no state of its own beyond scroll position: what it
 * draws is passed in each frame, so it cannot show a stale character's shop.
 */
export function createNpcPanel({ read, decodeDc6, decodeDatPalette, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(NPC_PANEL_ASSETS.palette));
  // Each stock row draws the item's own retail inventory sprite, from the invFile the engine
  // reports for that item (d2_session_vendor_presentations). A shop of names and prices is not a
  // shop; retail shows the goods.
  //
  // NOT yet retail's layout: retail's trade window is an icon GRID, and which Inventory.txt row
  // describes a shop tab is still open -- Charsi's stock spans three tabs with extents up to 10x8
  // and no single row is that shape. Drawing a grid now
  // would mean inventing the geometry, so the icons land in the existing rows and the grid stays
  // declared as open.
  const icons = new Map();
  const icon = (invFile) => {
    if (!invFile) return null;
    if (icons.has(invFile)) return icons.get(invFile);
    let image = null;
    try {
      const decoded = decodeDc6(read(inventoryItemAsset(invFile)));
      if (decoded.frames.length) image = frameCanvas(decoded.frames[0], palette, documentObject);
    } catch { image = null; }   // a missing sprite must not take the whole shop down
    icons.set(invFile, image);
    return image;
  };
  const background = composeTiled(decodeDc6(read(NPC_PANEL_ASSETS.background)), palette, BACKGROUND_COLUMNS, documentObject);
  const font = createFont({
    dc6: decodeDc6(read(NPC_PANEL_ASSETS.font)), tbl: decodeFontTbl(read(NPC_PANEL_ASSETS.fontTbl)),
    palette: decodeDatPalette(read(FRONTEND.palette)), documentObject,
  });

  let scroll = 0;

  // Retail anchors the trade window at screen (80, 60) on an 800x600 screen, scaled with the view.
  const panelBox = (width, height) => {
    const scale = Math.min(1, width / 800, height / 600);
    return {
      x: Math.round((width - 800 * scale) / 2 + TRADE_LAYOUT.screenX * scale),
      y: Math.round((height - 600 * scale) / 2 + TRADE_LAYOUT.screenY * scale),
      width: background.width, height: background.height, scale,
    };
  };
  // One definition of where a row sits, used by both the painter and the hit test.


  return {
    get evidence() {
      return { background: { width: background.width, height: background.height,
        rows: background.rows, columns: background.columns }, lineHeight: font.lineHeight, scroll };
    },
    scrollBy(rows) { scroll = Math.max(0, scroll + rows); return scroll; },
    resetScroll() { scroll = 0; },
    /**
     * The stock row under a canvas point, or null. Shares its geometry with draw() through
     * rowRect(), so a click cannot land on a different row than the one the player sees.
     */
    itemAt(px, py, width, height, state) {
      const items = Array.isArray(state?.items) ? state.items : [];
      const box = panelBox(width, height), L = TRADE_LAYOUT;
      const x = px - box.x, y = py - box.y;
      const gx = Math.floor((x - L.gridX) / L.cell);
      if (gx < 0 || gx >= L.columns) return null;
      // Shares the grid with draw(), so a click cannot land on a different item than the one shown.
      const gy = Math.floor((y - L.gridY) / L.cell);
      if (gy >= 0 && gy < L.rows)
        return items.find((it) => it.gridX === gx && (it.gridY ?? -1) - scroll === gy) || null;
      // The lower grid is the player's: a click there is a SALE, flagged so the caller can tell
      // which side of the trade was clicked.
      const playerRow = Math.floor((y - L.playerGridY) / L.cell);
      if (playerRow >= 0 && playerRow < L.rows) {
        const own = (state?.playerItems || []).find((it) =>
          it.invPage === 0 && it.gridX === gx && it.gridY === playerRow);
        return own ? { ...own, mine: true } : null;
      }
      return null;
    },

    /** `state` is { name, greeting, items: [{ name, cost, code }], gold }. */
    draw(ctx, width, height, state) {
      const box = panelBox(width, height);
      const w = box.width, h = box.height, x = box.x, y = box.y;
      ctx.drawImage(background.canvas, x, y);


      const items = Array.isArray(state.items) ? state.items : [];
      // A stock page is taller than the four visible rows (Charsi's tabs reach 10x8), so `scroll`
      // is a row offset into the page, not an index into a list.
      const L = TRADE_LAYOUT;
      let drawnIcons = 0, placed = 0;
      for (const item of items) {
        const gx = item.gridX ?? -1, gy = (item.gridY ?? -1) - scroll;
        if (gx < 0 || gx >= L.columns || gy < 0 || gy >= L.rows) continue;
        placed++;
        // Absolute canvas coordinates: the grid offsets are panel-relative, and the panel is not at
        // the origin. Leaving them relative drew the whole stock up in the top-left of the screen.
        const cx = x + L.gridX + gx * L.cell, cy = y + L.gridY + gy * L.cell;
        const affordable = !(state.gold >= 0) || item.cost <= state.gold;
        ctx.globalAlpha = affordable ? 1 : 0.45;
        const sprite = icon(item.invFile);
        if (sprite) {
          // An item's art spans the cells it OCCUPIES and anchors at its top-left cell -- a spear is
          // 116px of sprite over four cells. Centring it on one 29px cell offsets it 43px upward,
          // which floated the whole stock above the panel.
          ctx.drawImage(sprite, cx, cy);
          drawnIcons++;
        }
        ctx.globalAlpha = 1;
      }
      if (state.gold >= 0) font.draw(ctx, String(state.gold), x + GOLD_ANCHOR.x, y + GOLD_ANCHOR.y);
      // The player's own items, in the lower grid. Selling is a click on one of these, so drawing
      // them is what makes the trade screen a trade screen rather than a shop window.
      let playerDrawn = 0;
      for (const item of state.playerItems || []) {
        if (item.invPage !== 0) continue;          // carried items only, not equipped or belt
        const gx = item.gridX ?? -1, gy = item.gridY ?? -1;
        if (gx < 0 || gx >= L.columns || gy < 0 || gy >= L.rows) continue;
        const sprite = icon(item.invFile);
        if (!sprite) continue;
        ctx.drawImage(sprite, x + L.gridX + gx * L.cell, y + L.playerGridY + gy * L.cell);
        playerDrawn++;
      }

      // How much of the page is on screen, since a stock page is taller than the grid.
      if (items.length > placed) {
        const more = `${placed}/${items.length}`;
        font.draw(ctx, more, x + GOLD_ANCHOR.x, y + h - 52);
      }
      return { box: { x, y, width: w, height: h }, shown: placed, total: items.length,
        gold: state.gold ?? -1, drawnIcons, scroll, playerDrawn };
    },
  };
}
