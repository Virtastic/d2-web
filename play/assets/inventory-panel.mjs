import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { BORDER_RIGHT, CHROME_ART, createPanelChrome } from './panel-chrome.mjs';
import { paperdollIdentity } from './paperdoll-identity.mjs';
import { PANEL_CLOSE_ART, panelCloseLayout } from './panel-close.mjs';

// Inventory.txt Amazon2, normalized from its 800x600 screen coordinates to the
// invchar6 panel origin. Retail leaves the final 80 pixels for the screen edge.
export const AMAZON2_INVENTORY_LAYOUT = Object.freeze({
  screenX: 400, screenY: 60, width: 320, height: 441,
  gridX: 19, gridY: 255, columns: 10, rows: 4, cellWidth: 29, cellHeight: 29,
});
export const AMAZON2_EQUIPMENT_LAYOUT = Object.freeze({
  1: { x: 135, y: 8, width: 54, height: 51 }, 2: { x: 209, y: 35, width: 23, height: 24 },
  3: { x: 133, y: 77, width: 56, height: 82 }, 4: { x: 20, y: 47, width: 55, height: 112 },
  5: { x: 251, y: 47, width: 55, height: 112 }, 6: { x: 95, y: 180, width: 23, height: 24 },
  7: { x: 209, y: 180, width: 23, height: 24 }, 8: { x: 136, y: 179, width: 52, height: 25 },
  9: { x: 252, y: 182, width: 54, height: 52 }, 10: { x: 21, y: 181, width: 54, height: 53 },
});

// Inventory.txt's component order (DATATBLS_GetInventoryComponentGrid) -> the body location each
// component holds (D2 BODYLOC_*: 1 head, 2 neck, 3 torso, 4 right arm, 5 left arm, 6/7 rings, 8 belt,
// 9 feet, 10 gloves).
export const COMPONENT_BODYLOC = Object.freeze([4, 3, 5, 1, 2, 6, 7, 8, 9, 10]);

/**
 * The player's panel geometry from the engine, in 800x600 screen pixels: `grid` is
 * d2_session_inventory_layout's twelve numbers and `components` d2_session_inventory_components'
 * sixty, for the player's own Inventory.txt row (d2_session_player_inventory_record). Returns
 * { grid: {left, top, columns, rows, cellWidth, cellHeight}, slots: {bodyLoc: {left, right, top, bottom}} }.
 */
export function inventoryLayoutFromEngine(grid, components) {
  if (!grid || grid.length < 12) return null;
  const slots = {};
  for (let i = 0; components && i < COMPONENT_BODYLOC.length && i * 6 + 5 < components.length; i++) {
    const [left, right, top, bottom] = components.slice(i * 6, i * 6 + 4);
    if (right > left && bottom > top) slots[COMPONENT_BODYLOC[i]] = { left, right, top, bottom };
  }
  return { grid: { left: grid[6], top: grid[8], columns: grid[4], rows: grid[5], cellWidth: grid[10], cellHeight: grid[11] }, slots };
}

// Empty equipment slots show retail's silhouettes: bodyLoc ->
// [file, frame, left x, bottom y] on the 800x600 screen.
export const EMPTY_SLOT_ART = Object.freeze({
  3: ['inv_armor', 0, 535, 217], 8: ['inv_belt', 0, 536, 263], 9: ['inv_boots', 0, 651, 290],
  1: ['inv_helm_glove', 1, 535, 118], 10: ['inv_helm_glove', 0, 420, 292],
  2: ['inv_ring_amulet', 0, 609, 118], 6: ['inv_ring_amulet', 1, 495, 263], 7: ['inv_ring_amulet', 1, 609, 263],
});
// Retail's own anchors for InvChar6 frames 4..7 (left x, bottom y).
const INV_PANEL = [[4, 400, 316], [5, 656, 316], [6, 400, 492], [7, 656, 492]];

export const ITEM_PRESENTATION_BYTES = 112;

function fixedAscii(bytes, offset, length) {
  let end = offset;
  while (end < offset + length && bytes[end]) end++;
  for (let i = offset; i < end; i++) if (bytes[i] < 0x20 || bytes[i] > 0x7e) throw new RangeError('invalid item filename byte');
  return String.fromCharCode(...bytes.subarray(offset, end));
}

export function parseInventoryItems(input, count) {
  const bytes = input instanceof Uint8Array ? input : new Uint8Array(input);
  if (!Number.isInteger(count) || count < 0 || bytes.length !== count * ITEM_PRESENTATION_BYTES)
    throw new RangeError('invalid item presentation extent');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength), items = [];
  for (let i = 0; i < count; i++) {
    const o = i * ITEM_PRESENTATION_BYTES, invFile = fixedAscii(bytes, o + 72, 32);
    const invPage = view.getUint8(o + 108);
    if (!invFile || invPage !== 0) continue;
    items.push({ id: view.getUint32(o, true), classId: view.getUint32(o + 4, true), code: view.getUint32(o + 8, true),
      quality: view.getUint32(o + 24, true), itemFlags: view.getUint32(o + 28, true), invFile,
      x: view.getInt16(o + 104, true), y: view.getInt16(o + 106, true), invPage, bodyLoc: view.getUint8(o + 109) });
  }
  return items;
}

export function inventoryItemAsset(invFile) {
  if (typeof invFile !== 'string' || !/^[A-Za-z0-9_]+$/.test(invFile)) throw new RangeError('invalid inventory item file');
  return `data\\global\\items\\${invFile}.dc6`;
}

export function inventoryGridPosition(item, layout = AMAZON2_INVENTORY_LAYOUT) {
  const x = Number(item?.gridX ?? item?.x), y = Number(item?.gridY ?? item?.y);
  if (!Number.isInteger(x) || !Number.isInteger(y) || x < 0 || y < 0 || x >= layout.columns || y >= layout.rows)
    throw new RangeError(`invalid inventory grid position ${x},${y}`);
  return { x: layout.gridX + x * layout.cellWidth, y: layout.gridY + y * layout.cellHeight };
}

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas'); canvas.width = frame.width; canvas.height = frame.height;
  canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
}

export function createInventoryPanelRenderer(read, documentObject = globalThis.document) {
  const palette = decodeDatPalette(read('data\\global\\palette\\SKY\\pal.dat'));
  const dc6 = decodeDc6(read('data\\global\\ui\\PANEL\\invchar6.DC6'));
  if (dc6.frames.length < 8) throw new Error('unexpected retail invchar6 frame set');
  const panel = dc6.frames.map((frame) => frameCanvas(frame, palette, documentObject));
  const chrome = createPanelChrome(read, documentObject);
  const actPal = (act) => `data\\global\\palette\\ACT${Math.min(5, Math.max(1, act | 0))}\\pal.dat`;
  const icons = new Map();
  function icon(invFile, act = 1) {
    const key = act + ':' + invFile;
    let image = icons.get(key);
    if (!image) {
      const decoded = decodeDc6(read(inventoryItemAsset(invFile)));
      if (!decoded.frames.length) throw new Error(`empty inventory item ${invFile}`);
      image = frameCanvas(decoded.frames[0], decodeDatPalette(read(actPal(act))), documentObject); icons.set(key, image);
    }
    return image;
  }
  // A grid item sits on retail's box, colour 234 in draw mode 0: the act PL2's AlphaBlend[2][under][234]
  // (s197's frame buffer), read back from the canvas as palette indices (exact at scale 1).
  const blendByAct = new Map();
  function itemBackground(ctx, act, x, y, w, h) {
    let t = blendByAct.get(act);
    if (!t) { const pal = decodeDatPalette(read(actPal(act))), pl2 = read(actPal(act).replace(/pal\.dat$/i, 'pal.pl2')), index = new Map();
      for (let k = 255; k >= 0; k--) index.set((pal[k * 4] << 16) | (pal[k * 4 + 1] << 8) | pal[k * 4 + 2], k);
      t = { pal, pl2, index }; blendByAct.set(act, t); }
    ctx.save(); ctx.setTransform(1, 0, 0, 1, 0, 0);
    const X = Math.round(x), Y = Math.round(y), W = Math.round(w), H = Math.round(h);
    const img = ctx.getImageData(X, Y, W, H), d = img.data, base = 13568 + 2 * 65536;
    for (let i = 0; i < d.length; i += 4) { const u = t.index.get((d[i] << 16) | (d[i + 1] << 8) | d[i + 2]); if (u === undefined) continue;
      const c = t.pl2[base + u * 256 + 234]; d[i] = t.pal[c * 4]; d[i + 1] = t.pal[c * 4 + 1]; d[i + 2] = t.pal[c * 4 + 2]; }
    ctx.putImageData(img, X, Y); ctx.restore();
  }
  // Engine geometry (inventoryLayoutFromEngine) once the shell has it; until then the Amazon2
  // constants above, which draw exactly as before.
  let layout = null;
  const view = (width, height) => { const scale = Math.min(1, width / 800, height / 600);
    return { scale, ox: (width - 800 * scale) / 2, oy: (height - 600 * scale) / 2 }; };
  // An item's rectangle on the 800x600 screen: its grid cells, or the body slot it is worn in.
  const gridRect = (item) => {
    if (layout) { const g = layout.grid; return { left: g.left + item.gridX * g.cellWidth, top: g.top + item.gridY * g.cellHeight,
      width: (item.invWidth || 1) * g.cellWidth, height: (item.invHeight || 1) * g.cellHeight }; }
    const at = inventoryGridPosition(item);
    return { left: AMAZON2_INVENTORY_LAYOUT.screenX + at.x, top: AMAZON2_INVENTORY_LAYOUT.screenY + at.y,
      width: (item.invWidth || 1) * 29, height: (item.invHeight || 1) * 29 };
  };
  const slotRect = (bodyLoc) => {
    if (layout) { const r = layout.slots[bodyLoc]; return r ? { left: r.left, top: r.top, width: r.right - r.left, height: r.bottom - r.top } : null; }
    const r = AMAZON2_EQUIPMENT_LAYOUT[bodyLoc];
    return r ? { left: AMAZON2_INVENTORY_LAYOUT.screenX + r.x, top: AMAZON2_INVENTORY_LAYOUT.screenY + r.y, width: r.width, height: r.height } : null;
  };
  const toScreen = (width, height, x, y) => { const v = view(width, height); return [(x - v.ox) / v.scale, (y - v.oy) / v.scale]; };
  const inside = (x, y, r) => r && x >= r.left && x < r.left + r.width && y >= r.top && y < r.top + r.height;
  return {
  setLayout(value) { layout = value || null; },
  get layout() { return layout; },
  gridRect, slotRect,
  itemAt(screenX, screenY, width, height, state) {
    const [x, y] = toScreen(width, height, screenX, screenY);
    return [...(state?.items || [])].reverse().find((item) => item.invPage === 0 && item.mode === 0 && inside(x, y, gridRect(item))) || null;
  },
  // The worn item under the pointer, or the empty slot: { bodyLoc, item }.
  slotAt(screenX, screenY, width, height, state) {
    const [x, y] = toScreen(width, height, screenX, screenY);
    for (let bodyLoc = 1; bodyLoc <= 10; bodyLoc++) if (inside(x, y, slotRect(bodyLoc)))
      return { bodyLoc, item: (state?.items || []).find((i) => i.mode === 1 && i.bodyLoc === bodyLoc) || null };
    return null;
  },
  // The grid cell a cursor item would be put down at, the item centred on the pointer as retail
  // holds it; null off the grid.
  cellAt(screenX, screenY, width, height, item) {
    const [x, y] = toScreen(width, height, screenX, screenY);
    const g = layout ? layout.grid : { left: AMAZON2_INVENTORY_LAYOUT.screenX + AMAZON2_INVENTORY_LAYOUT.gridX, top: AMAZON2_INVENTORY_LAYOUT.screenY + AMAZON2_INVENTORY_LAYOUT.gridY,
      columns: AMAZON2_INVENTORY_LAYOUT.columns, rows: AMAZON2_INVENTORY_LAYOUT.rows, cellWidth: 29, cellHeight: 29 };
    if (x < g.left || y < g.top || x >= g.left + g.columns * g.cellWidth || y >= g.top + g.rows * g.cellHeight) return null;
    const w = item?.invWidth || 1, h = item?.invHeight || 1;
    const cx = Math.round((x - g.left) / g.cellWidth - w / 2), cy = Math.round((y - g.top) / g.cellHeight - h / 2);
    return { x: Math.max(0, Math.min(g.columns - w, cx)), y: Math.max(0, Math.min(g.rows - h, cy)) };
  },
  // The panel's own rectangle on the 800x600 screen: what `contains` tests and what the shell reports
  // to an input layer that has to bound a finger's own aim by the panel's chrome (M15, s377) -- so the
  // band a finger's tap is allowed to land in is read from ONE place rather than re-derived.
  panelRect() { return { left: AMAZON2_INVENTORY_LAYOUT.screenX, top: AMAZON2_INVENTORY_LAYOUT.screenY,
    width: AMAZON2_INVENTORY_LAYOUT.width, height: AMAZON2_INVENTORY_LAYOUT.height }; },
  // Whether the pointer is over the panel at all (a click there is never a click on the world).
  contains(screenX, screenY, width, height) {
    const [x, y] = toScreen(width, height, screenX, screenY);
    const r = this.panelRect();
    return x >= r.left && x < r.left + r.width && y >= r.top && y < r.top + r.height;
  },
  // The item on the cursor, drawn centred on the pointer (retail draws the held item's inventory
  // art there). Returns what was drawn, for identity checks.
  drawCursorItem(ctx, width, height, item, pointerX, pointerY) {
    if (!item?.invFile) return null;
    const image = icon(item.invFile), v = view(width, height);
    const x = pointerX - image.width * v.scale / 2, y = pointerY - image.height * v.scale / 2;
    ctx.save(); ctx.translate(x, y); ctx.scale(v.scale, v.scale); ctx.drawImage(image, 0, 0); ctx.restore();
    return { id: item.id, invFile: item.invFile, x: Math.round(x), y: Math.round(y), width: image.width, height: image.height };
  },
  draw(ctx, width, height, state, visible = true) {
    if (!visible) return null;
    const scale = Math.min(1, width / 800, height / 600);
    const baseX = (width - 800 * scale) / 2 + AMAZON2_INVENTORY_LAYOUT.screenX * scale;
    const baseY = (height - 600 * scale) / 2 + AMAZON2_INVENTORY_LAYOUT.screenY * scale;
    const act = state?.act || 1, ox = (width - 800 * scale) / 2, oy = (height - 600 * scale) / 2;
    // The panel and the empty-slot silhouettes at retail's screen anchors, act palette (s197).
    ctx.save(); ctx.translate(ox, oy); ctx.scale(scale, scale);
    for (const [i, x, y] of INV_PANEL) chrome.cel(ctx, act, 'data\\global\\ui\\PANEL\\invchar6.DC6', i, x, y);
    const worn = new Set((state?.items || []).filter((item) => item.mode === 1).map((item) => item.bodyLoc));
    for (const [loc, [file, frame, x, y]] of Object.entries(EMPTY_SLOT_ART))
      if (!worn.has(Number(loc))) chrome.cel(ctx, act, `data\\global\\ui\\PANEL\\${file}.dc6`, frame, x, y);
    ctx.restore();
    ctx.save(); ctx.translate(baseX, baseY); ctx.scale(scale, scale);
    let itemPixels = 0, equippedItemPixels = 0, equippedItemCount = 0;
    // Which item went where, in 800x600 screen pixels: the identity a row checks against the engine.
    const drawn = [];
    const sx = AMAZON2_INVENTORY_LAYOUT.screenX, sy = AMAZON2_INVENTORY_LAYOUT.screenY;
    for (const item of state?.items || []) {
      if (!item.invFile) continue;
      const image = icon(item.invFile, act);
      // Retail's placement (s197's frame buffer): the item's box is its cells; in the grid the box is
      // backed by colour 234 in mode 0, and the art is drawn at the box's left, one row below its top.
      if (item.invPage === 0 && item.mode === 0) { const r = gridRect(item);
        itemBackground(ctx, act, ox + r.left * scale, oy + r.top * scale, r.width * scale, r.height * scale);
        ctx.drawImage(image, r.left - sx, r.top - sy + 1); itemPixels += image.width * image.height;
        drawn.push({ id: item.id, where: 'grid', gridX: item.gridX, gridY: item.gridY, left: r.left, top: r.top }); }
      else if (item.mode === 1 && slotRect(item.bodyLoc)) { const slot = slotRect(item.bodyLoc);
        // On the body the box (cells of 29) is centred in the slot rounding down, the art one row lower.
        const bw = Math.ceil(image.width / 29) * 29, bh = Math.ceil(image.height / 29) * 29;
        const left = slot.left + Math.floor((slot.width - bw) / 2), top = slot.top + Math.floor((slot.height - bh) / 2) + 1;
        ctx.drawImage(image, left - sx, top - sy);
        equippedItemPixels += image.width * image.height; equippedItemCount++;
        drawn.push(paperdollIdentity(item, decodeDc6(read(inventoryItemAsset(item.invFile))).frames[0], left, top, slot)); }
    }
    ctx.restore();
    // Gold, the gold button, the close button and the right-hand border (retail's anchors, s197).
    ctx.save(); ctx.translate(ox, oy); ctx.scale(scale, scale);
    chrome.text(ctx, act, String(state?.gold ?? 0), 508, 468);
    chrome.cel(ctx, act, CHROME_ART.gold, 0, 484, 469);
    // panelCloseLayout is the one place this rect lives now (panel-close.mjs) -- the drawn cel and
    // the click hit-box both come from it, so they cannot drift apart the way 419,449 vs 418,476 did.
    const close = panelCloseLayout('inventory');
    const closeBlit = chrome.cel(ctx, act, PANEL_CLOSE_ART.path, PANEL_CLOSE_ART.frame, close.x, close.y + close.height - 1);
    chrome.border(ctx, act, BORDER_RIGHT);
    ctx.restore();
    return { x: baseX, y: baseY, scale, panelFrames: 4,
      panelPixels: [4, 5, 6, 7].reduce((sum, i) => sum + dc6.frames[i].width * dc6.frames[i].height, 0),
      itemCount: (state?.items || []).filter((item) => item.invPage === 0).length, itemPixels,
      equippedItemCount, equippedItemPixels, drawn, layout: layout ? 'engine' : 'amazon2',
      close: { ...close, path: PANEL_CLOSE_ART.path, frame: PANEL_CLOSE_ART.frame, blit: closeBlit } };
  } };
}
