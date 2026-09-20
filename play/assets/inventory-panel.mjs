import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';

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
  const icons = new Map();
  function icon(invFile) {
    let image = icons.get(invFile);
    if (!image) {
      const decoded = decodeDc6(read(inventoryItemAsset(invFile)));
      if (!decoded.frames.length) throw new Error(`empty inventory item ${invFile}`);
      image = frameCanvas(decoded.frames[0], palette, documentObject); icons.set(invFile, image);
    }
    return image;
  }
  return { itemAt(screenX, screenY, width, height, state) {
    const scale = Math.min(1, width / 800, height / 600);
    const baseX = (width - 800 * scale) / 2 + AMAZON2_INVENTORY_LAYOUT.screenX * scale;
    const baseY = (height - 600 * scale) / 2 + AMAZON2_INVENTORY_LAYOUT.screenY * scale;
    const x = (screenX - baseX) / scale, y = (screenY - baseY) / scale;
    return [...(state?.items || [])].reverse().find((item) => { if (item.invPage !== 0) return false;
      const at = inventoryGridPosition(item); return x >= at.x && x < at.x + 29 && y >= at.y && y < at.y + 29; }) || null;
  }, draw(ctx, width, height, state, visible = true) {
    if (!visible) return null;
    const scale = Math.min(1, width / 800, height / 600);
    const baseX = (width - 800 * scale) / 2 + AMAZON2_INVENTORY_LAYOUT.screenX * scale;
    const baseY = (height - 600 * scale) / 2 + AMAZON2_INVENTORY_LAYOUT.screenY * scale;
    ctx.save(); ctx.translate(baseX, baseY); ctx.scale(scale, scale);
    // Retail invchar6 direction 1: TL, TR, BL, BR at flattened frames 4..7.
    let x = 1, y = 4;
    ctx.drawImage(panel[4], x, y); x += panel[4].width;
    ctx.drawImage(panel[5], x, y); y += panel[5].height;
    ctx.drawImage(panel[7], x, y); x = 1;
    ctx.drawImage(panel[6], x, y);
    let itemPixels = 0, equippedItemPixels = 0, equippedItemCount = 0;
    for (const item of state?.items || []) {
      if (!item.invFile) continue;
      const image = icon(item.invFile);
      if (item.invPage === 0) { const at = inventoryGridPosition(item);
        ctx.drawImage(image, at.x, at.y); itemPixels += image.width * image.height; }
      else if (item.mode === 1 && AMAZON2_EQUIPMENT_LAYOUT[item.bodyLoc]) { const slot = AMAZON2_EQUIPMENT_LAYOUT[item.bodyLoc];
        ctx.drawImage(image, slot.x + Math.floor((slot.width - image.width) / 2), slot.y + Math.floor((slot.height - image.height) / 2));
        equippedItemPixels += image.width * image.height; equippedItemCount++; }
    }
    ctx.restore();
    return { x: baseX, y: baseY, scale, panelFrames: 4,
      panelPixels: [4, 5, 6, 7].reduce((sum, i) => sum + dc6.frames[i].width * dc6.frames[i].height, 0),
      itemCount: (state?.items || []).filter((item) => item.invPage === 0).length, itemPixels,
      equippedItemCount, equippedItemPixels };
  } };
}
