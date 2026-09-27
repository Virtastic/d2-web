// A vendor's shop window, as retail draws it: the buysell panel on the left, a row of tabs across its
// top (Armor / Weapons / Weapons / Misc), a 10x10 grid holding the tab's stock, and the Buy / Sell /
// Repair buttons along the bottom. The player's own inventory is the ordinary inventory panel on the
// right (the shell opens it with the shop).
//
// Retail data end to end:
//   * art: data\global\ui\PANEL\buysell.dc6 (four frames -> 320x432), buyselltabs.DC6 (frame t is
//     tab t active, t+4 inactive, 79x31), buysellbtn.DC6 (Buy 2/3, Sell 4/5, Repair 6/7, Repair All
//     18/19, Exit 10/11) -- the frame roles and the tab/button offsets are riiablo's
//     (reference/riiablo .../screen/panel/VendorPanel.java: tabs from the panel top, 80px apart;
//     buttons at x 116/168/220/272, 15px above the panel bottom);
//   * geometry: Inventory.txt row 5 "Monster" through the engine's accessor
//     (d2_session_inventory_layout): panel 80..401 x 60..502, grid 10x10 of 29px cells at 96,123;
//   * which tab an item is on, and where: the ENGINE's store (d2_session_vendor_presentations
//     invPage = the tab, gridX/gridY) -- retail's server places the stock, the client only shows it;
//   * tab names: strBSArmor / strBSWeapons / strBSMisc; a tab with no stock is not drawn.
// TODO(client-parity P8c): no retail frame of a shop yet (level 2); the opening tab (Misc) is
// riiablo's; buying is a left click on the item (P8d), which retail may bind differently.
import { createFont, decodeFontTbl, FRONTEND, textColourPalette } from './frontend.mjs';
import { BORDER_LEFT, BORDER_RIGHT, createPanelChrome } from './panel-chrome.mjs';
import { inventoryItemAsset } from './inventory-panel.mjs';

export const VENDOR_ASSETS = {
  palette: 'data\\global\\palette\\ACT1\\pal.dat',
  panel: 'data\\global\\ui\\PANEL\\buysell.dc6',
  tabs: 'data\\global\\ui\\PANEL\\buyselltabs.DC6',
  buttons: 'data\\global\\ui\\PANEL\\buysellbtn.DC6',
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTbl: 'data\\local\\font\\latin\\font16.tbl',
};
// Inventory.txt "Monster" at 800x600 (read through the engine by the shell; these are its values,
// kept here so the panel can draw before the first read and so a mismatch is visible in review).
export const VENDOR_LAYOUT = Object.freeze({ left: 80, top: 60, gridLeft: 96, gridTop: 123, columns: 10, rows: 10, cell: 29,
  tabStep: 80, buttonX: [195, 248, 300, 352], buttonFromBottom: 15 });   // buttonX: retail's draw list (vendor-akara), riiablo's 116..272 were off
const TAB_NAMES = ['strBSArmor', 'strBSWeapons', 'strBSWeapons', 'strBSMisc'];
const BUTTON_FRAMES = { blank: 0, buy: 2, sell: 4, repair: 6, exit: 10, repairAll: 18 };
// The smiths (MonStats: Charsi, Fara, Hratli, Halbu, Larzuk) are the vendors D2GAME_NPC_Repair accepts;
// their row is Buy / Sell / Repair / Repair All, every other vendor's Buy / Sell / (blank) / Exit
// (riiablo's VendorPanel SMITHY and TRADER masks over the same four slots).
export const SMITH_CLASSES = new Set([154, 178, 253, 257, 511]);
export const vendorButtons = (npcClass) => (SMITH_CLASSES.has(npcClass) ? ['buy', 'sell', 'repair', 'repairAll'] : ['buy', 'sell', 'blank', 'exit']);

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (!frame.width || !frame.height) return canvas;
  const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
  for (let i = 0; i < frame.indices.length; i++) if (frame.opaque[i]) rgba.set(palette.subarray(frame.indices[i] * 4, frame.indices[i] * 4 + 4), i * 4);
  canvas.getContext('2d').putImageData(new ImageData(rgba, frame.width, frame.height), 0, 0);
  return canvas;
}

// `drawImage` has no result value.  For evidence, compare the destination rectangle immediately
// before and after the blit; an asset which decodes but paints no pixels is not a drawn item.
function changedOutputPixels(ctx, transform, x, y, width, height, draw) {
  const x0 = Math.max(0, Math.floor(transform.e + x * transform.a));
  const y0 = Math.max(0, Math.floor(transform.f + y * transform.d));
  const x1 = Math.min(ctx.canvas.width, Math.ceil(transform.e + (x + width) * transform.a));
  const y1 = Math.min(ctx.canvas.height, Math.ceil(transform.f + (y + height) * transform.d));
  if (x1 <= x0 || y1 <= y0) { draw(); return 0; }
  const before = ctx.getImageData(x0, y0, x1 - x0, y1 - y0).data;
  draw();
  const after = ctx.getImageData(x0, y0, x1 - x0, y1 - y0).data;
  let changed = 0;
  for (let i = 0; i < after.length; i += 4)
    if (before[i] !== after[i] || before[i + 1] !== after[i + 1] || before[i + 2] !== after[i + 2] || before[i + 3] !== after[i + 3]) changed++;
  return changed;
}

export function createVendorPanel({ read, decodeDc6, decodeDatPalette, string, layout = VENDOR_LAYOUT, documentObject = globalThis.document }) {
  // Everything drawn through a palette is built per act (the session's act, as the skill tree and
  // npc-menu.mjs fontsFor(act)): pal.dat for the art, pal.pl2 for the gold text and the item box blend.
  const actPath = (act, f) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\${f}`;
  const panelDc6 = decodeDc6(read(VENDOR_ASSETS.panel));
  const tabDc6 = decodeDc6(read(VENDOR_ASSETS.tabs)), buttonDc6 = decodeDc6(read(VENDOR_ASSETS.buttons));
  const fontDc6 = decodeDc6(read(VENDOR_ASSETS.font)), fontTbl = decodeFontTbl(read(VENDOR_ASSETS.fontTbl));
  const perAct = new Map();
  const forAct = (act) => {
    let a = perAct.get(act);
    if (a) return a;
    const palette = decodeDatPalette(read(actPath(act, 'pal.dat'))), pl2 = read(actPath(act, 'pal.pl2'));
    // buysell.dc6: 256+64 wide over 256+176 tall, four frames in two rows.
    const panel = documentObject.createElement('canvas');
    panel.width = panelDc6.frames[0].width + panelDc6.frames[1].width; panel.height = panelDc6.frames[0].height + panelDc6.frames[2].height;
    { const g = panel.getContext('2d'); let y = 0;
      for (let r = 0; r < 2; r++) { let x = 0; for (let c = 0; c < 2; c++) { const f = panelDc6.frames[r * 2 + c]; g.drawImage(frameCanvas(f, palette, documentObject), x, y); x += f.width; } y += panelDc6.frames[r * 2].height; } }
    const font = createFont({ dc6: fontDc6, tbl: fontTbl, palette, documentObject });
    const index = new Map();
    for (let k = 255; k >= 0; k--) index.set((palette[k * 4] << 16) | (palette[k * 4 + 1] << 8) | palette[k * 4 + 2], k);
    a = { palette, pl2, index, panel, font, icons: new Map(),
      gold: createFont({ dc6: fontDc6, tbl: fontTbl, palette: textColourPalette(pl2, palette, 4), documentObject }),
      tabFrames: tabDc6.frames.map((f) => frameCanvas(f, palette, documentObject)),
      buttonFrames: buttonDc6.frames.map((f) => frameCanvas(f, palette, documentObject)) };
    perAct.set(act, a);
    return a;
  };
  let cur = forAct(1);
  const chrome = createPanelChrome(read, documentObject);
  // A decoded icon's first successful blit is sufficient evidence for its asset.  Retain that
  // readback result so opening or redrawing a shop does not turn the renderer into a readback loop.
  const visibleBlits = new WeakMap();
  // A stock item sits on retail's box, colour 234 in draw mode 0 (AlphaBlend[2][under][234]), as in the
  // inventory (s197/s250), read back from the canvas as palette indices.
  const itemBackground = (ctx, x, y, w, h) => {
    const { pl2, index, palette } = cur;
    const X = Math.round(x), Y = Math.round(y), W = Math.round(w), H = Math.round(h);
    ctx.save(); ctx.setTransform(1, 0, 0, 1, 0, 0);
    const img = ctx.getImageData(X, Y, W, H), d = img.data, base = 13568 + 2 * 65536;
    for (let i = 0; i < d.length; i += 4) { const u = index.get((d[i] << 16) | (d[i + 1] << 8) | d[i + 2]); if (u === undefined) continue;
      const c = pl2[base + u * 256 + 234]; d[i] = palette[c * 4]; d[i + 1] = palette[c * 4 + 1]; d[i + 2] = palette[c * 4 + 2]; }
    ctx.putImageData(img, X, Y); ctx.restore();
  };
  const icon = (invFile) => {
    if (!invFile) return null;
    const icons = cur.icons;
    if (icons.has(invFile)) return icons.get(invFile);
    let image = null;
    try { const d = decodeDc6(read(inventoryItemAsset(invFile))); if (d.frames.length) image = frameCanvas(d.frames[0], cur.palette, documentObject); } catch { image = null; }
    icons.set(invFile, image); return image;
  };
  const L = layout;
  let tab = 3, lastTabs = [];
  const view = (width, height) => { const scale = Math.min(width / 800, height / 600); return { scale, ox: (width - 800 * scale) / 2, oy: (height - 600 * scale) / 2 }; };
  const toScreen = (width, height, px, py) => { const v = view(width, height); return [(px - v.ox) / v.scale, (py - v.oy) / v.scale]; };
  // Inventory.txt has two weapon store pages (1 and 2), but the retail strip has
  // one Weapons tab: vendor-malah-act5 draws frame 5 at x=160 once, never a second
  // tab at x=240.  Page 2 is therefore not a second label/control.
  const tabsWithStock = (items) => [0, 1, 3].filter((t) => items.some((it) => it.invPage === t || (t === 1 && it.invPage === 2)));
  // This predicate deliberately serves both rendering and pointer selection.  Splitting page 2
  // from page 1 here would make items the Weapons tab says it contains invisible or unclickable.
  const belongsToTab = (it, t) => it.invPage === t || (t === 1 && it.invPage === 2);
  // Retail's anchors: the panel's top row is y 61, a tab's too.
  const tabRect = (t) => ({ x: L.left + t * L.tabStep, y: L.top + 1, w: cur.tabFrames[t].width, h: cur.tabFrames[t].height });
  const buttonTop = () => 446;   // retail: bottom row 477, 32 tall (vendor-akara)
  const buttonRect = (i, name) => { const f = cur.buttonFrames[BUTTON_FRAMES[name]]; return { x: L.buttonX[i], y: buttonTop(), w: f ? f.width : 32, h: f ? f.height : 32 }; };
  return {
    get tab() { return tab; },
    get evidence() { return { background: { width: cur.panel.width, height: cur.panel.height }, art: VENDOR_ASSETS.panel, layout: { ...L } }; },
    /** Show the tab retail opens a shop on, or the first one with stock. */
    reset(items = []) { const t = tabsWithStock(items); tab = t.includes(3) ? 3 : (t[0] ?? 3); },
    setTab(t) { tab = t; },
    tabAt(px, py, width, height) {
      const [x, y] = toScreen(width, height, px, py);
      return lastTabs.find((t) => { const r = tabRect(t); return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h; }) ?? null;
    },
    itemAt(px, py, width, height, state) {
      const [x, y] = toScreen(width, height, px, py);
      const gx = Math.floor((x - L.gridLeft) / L.cell), gy = Math.floor((y - L.gridTop) / L.cell);
      if (gx < 0 || gx >= L.columns || gy < 0 || gy >= L.rows) return null;
      // An item covers its whole footprint -- the engine's own ItemsTxt nInvWidth/nInvHeight when the
      // vendor presentation carries them -- and a click anywhere on it buys it.  Draw order is input
      // order, so the final matching cel is visually on top: a grouped Weapons page can share a cell
      // across pages 1/2, and selecting the top cel makes the hit test name the item the player sees.
      return (state?.items || []).findLast((it) => belongsToTab(it, tab) && gx >= it.gridX && gx < it.gridX + (it.invWidth || it.width || 1) && gy >= it.gridY && gy < it.gridY + (it.invHeight || it.height || 1))
        ?? (state?.items || []).findLast((it) => belongsToTab(it, tab) && it.gridX === gx && it.gridY === gy) ?? null;
    },
    /** The button under a canvas point (not 'blank'), for the vendor of MonStats class npcClass. */
    buttonAt(px, py, width, height, npcClass) {
      const [x, y] = toScreen(width, height, px, py);
      const names = vendorButtons(npcClass);
      const i = names.findIndex((n, k) => { const r = buttonRect(k, n); return n !== 'blank' && x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h; });
      return i < 0 ? null : names[i];
    },
    /** Canvas pixel centre of each of the vendor's buttons, keyed by name. */
    buttonTargets(width, height, npcClass) {
      const v = view(width, height);
      return Object.fromEntries(vendorButtons(npcClass).filter((n) => n !== 'blank').map((n) => { const r = buttonRect(vendorButtons(npcClass).indexOf(n), n);
        return [n, [Math.round(v.ox + (r.x + r.w / 2) * v.scale), Math.round(v.oy + (r.y + r.h / 2) * v.scale)]]; }));
    },
    /** Canvas pixel centre of each visible tab, keyed by tab index. */
    tabTargets(width, height) {
      const v = view(width, height);
      return Object.fromEntries(lastTabs.map((t) => { const r = tabRect(t); return [t, [Math.round(v.ox + (r.x + r.w / 2) * v.scale), Math.round(v.oy + (r.y + r.h / 2) * v.scale)]]; }));
    },
    draw(ctx, width, height, state) {
      const items = Array.isArray(state?.items) ? state.items : [];
      cur = forAct(state?.act || 1);
      const { panel, tabFrames, buttonFrames, font, gold } = cur;
      const v = view(width, height);
      ctx.save(); ctx.translate(v.ox, v.oy); ctx.scale(v.scale, v.scale);
      ctx.drawImage(panel, L.left, L.top + 1);
      lastTabs = tabsWithStock(items);
      for (const t of lastTabs) {
        const r = tabRect(t), f = tabFrames[t === tab ? t : t + 4];
        ctx.drawImage(f, r.x, r.y);
        // The open tab's name in D2Win colour 4 (gold), the others white, anchored at row 80.
        const label = string(TAB_NAMES[t]) || '', face = t === tab ? gold : font;
        // x = tab + (w + 4 - text) / 2, fitted on retail's two labels (Weapons 168, Misc 346).
        face.drawAnchored(ctx, label, r.x + Math.floor((r.w + 4 - face.measure(label)) / 2), 80);
      }
      let shown = 0, drawnIcons = 0;
      // The exact engine presentation identity carried into the visible panel. This makes the
      // top-left cell and its whole ItemsTxt footprint observable to browser rows.
      const drawnItems = [], undrawnItems = [];
      for (const it of items) {
        if (!belongsToTab(it, tab) || it.gridX < 0 || it.gridY < 0) continue;
        shown++;
        const img = icon(it.invFile);
        const x = L.gridLeft + it.gridX * L.cell, y = L.gridTop + it.gridY * L.cell;
        if (img) {
          const t = ctx.getTransform();
          itemBackground(ctx, t.e + x * t.a, t.f + y * t.d, Math.ceil(img.width / L.cell) * L.cell * t.a, Math.ceil(img.height / L.cell) * L.cell * t.d);
          let visiblePixels = visibleBlits.get(img);
          if (visiblePixels === undefined) {
            visiblePixels = changedOutputPixels(ctx, t, x, y + 1, img.width, img.height, () => ctx.drawImage(img, x, y + 1));
            visibleBlits.set(img, visiblePixels);
          } else ctx.drawImage(img, x, y + 1);
          // Identity is evidence of a successful visible blit, never of an intended one.
          if (visiblePixels > 0) {
            drawnIcons++;
            drawnItems.push({ id: it.id, invFile: it.invFile, asset: inventoryItemAsset(it.invFile), visiblePixels,
              invPage: it.invPage, gridX: it.gridX, gridY: it.gridY,
              invWidth: it.invWidth || it.width || 1, invHeight: it.invHeight || it.height || 1 });
          } else undrawnItems.push({ id: it.id, invFile: it.invFile, asset: inventoryItemAsset(it.invFile), reason: 'no-visible-output' });
        } else undrawnItems.push({ id: it.id, invFile: it.invFile, reason: 'unresolved-asset' });
      }
      // Retail's button row (vendor-akara: buysellbtn 2 / 4 / 0 / 10 at x 195/248/300/352, bottom row 477);
      // a smith's row is main's Buy / Sell / Repair / Repair All (not captured yet).
      const buttons = vendorButtons(state?.classId);
      buttons.forEach((b, i) => { const c = buttonFrames[BUTTON_FRAMES[b]]; if (c) ctx.drawImage(c, L.buttonX[i], 477 - c.height + 1); });
      // The stash line (1.13 vendors take stash gold): string "stash" at (101, 434), the stash's gold at (266, 434).
      font.drawAnchored(ctx, string('stash'), 101, 434);
      if (state?.stashGold >= 0) font.drawAnchored(ctx, String(state.stashGold), 266, 434);
      ctx.restore();
      ctx.save(); ctx.translate(v.ox, v.oy); ctx.scale(v.scale, v.scale);
      chrome.border(ctx, state?.act || 1, BORDER_LEFT); chrome.border(ctx, state?.act || 1, BORDER_RIGHT);
      ctx.restore();
      return { box: { x: L.left, y: L.top, width: panel.width, height: panel.height }, tab, tabs: [...lastTabs], shown, total: items.length,
        drawnIcons, drawnItems, undrawnItems, scroll: 0, gold: state?.gold ?? -1, buttons };
    },
  };
}
