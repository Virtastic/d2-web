// The in-game menu Esc opens: OPTIONS / SAVE AND EXIT GAME / RETURN TO GAME in font42, the selected
// line flanked by the spinning pentagram cursor (data\global\ui\CURSOR\pentspin.DC6).
//
// Measured from retail's own frame (retail/frames/esc-menu.png, ~/d2-retail 1.13c): the lines are
// centred on x=400 with their ink starting at y=198, 249 and 299, and their widths are font42's
// UPPER-case advances (OPTIONS 160px, as retail). TODO(client-parity G1): the TEXT is copied from
// that frame -- it is in none of the string tables, and where D2Client takes it from is not
// recovered. OPTIONS has no submenu yet.
import { createFont, decodeFontTbl, FRONTEND } from './frontend.mjs';

export const GAME_MENU = {
  font: 'data\\local\\font\\latin\\font42.dc6',
  fontTbl: 'data\\local\\font\\latin\\font42.tbl',
  pentagram: 'data\\global\\ui\\CURSOR\\pentspin.DC6',
  items: ['OPTIONS', 'SAVE AND EXIT GAME', 'RETURN TO GAME'],
  inkTops: [198, 249, 299],
  pentagramOffsetX: 272,   // pentagram centres sit this far either side of x=400 (retail frame)
};

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (!frame.width || !frame.height) return canvas;
  const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
  for (let i = 0; i < frame.indices.length; i++) {
    if (!frame.opaque[i]) continue;
    rgba.set(palette.subarray(frame.indices[i] * 4, frame.indices[i] * 4 + 4), i * 4);
  }
  canvas.getContext('2d').putImageData(new ImageData(rgba, frame.width, frame.height), 0, 0);
  return canvas;
}

/** `onChoose(item)` receives one of GAME_MENU.items. */
export function createGameMenu({ read, decodeDc6, decodeDatPalette, onChoose, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(FRONTEND.palette));
  const fontDc6 = decodeDc6(read(GAME_MENU.font));
  const fontTbl = decodeFontTbl(read(GAME_MENU.fontTbl));
  const font = createFont({ dc6: fontDc6, tbl: fontTbl, palette, documentObject });
  // Where a capital's ink starts inside font42's fixed glyph cell, so a line can be placed by its ink.
  const oFrame = fontDc6.frames[fontTbl.chars.get(79)?.imageIndex ?? 79];
  let inkTop = 0;
  for (let y = 0; oFrame && y < oFrame.height; y++) { if (oFrame.opaque.subarray(y * oFrame.width, (y + 1) * oFrame.width).some(Boolean)) { inkTop = y; break; } }
  const pent = decodeDc6(read(GAME_MENU.pentagram));
  const pentCanvases = pent.frames.map((f) => frameCanvas(f, palette, documentObject));
  const rows = GAME_MENU.items.map((label, i) => {
    const w = font.measure(label);
    return { label, x: 400 - w / 2, y: GAME_MENU.inkTops[i] - inkTop, w, h: font.cellHeight - inkTop };
  });
  let open = false, selected = rows.length - 1, tick = 0;
  const view = (width, height) => {
    const scale = Math.min(width / 800, height / 600);
    return { scale, ox: (width - 800 * scale) / 2, oy: (height - 600 * scale) / 2 };
  };
  const rowAt = (x, y, width, height) => {
    const v = view(width, height), sy = (y - v.oy) / v.scale;
    return rows.findIndex((r) => sy >= r.y + inkTop - 8 && sy < r.y + inkTop + 34);
  };
  return {
    get open() { return open; },
    get evidence() { return { open, selected: rows[selected]?.label ?? null, rows: rows.map((r) => ({ label: r.label, x: Math.round(r.x), y: r.y, w: r.w })) }; },
    toggle() { open = !open; if (open) selected = rows.length - 1; return open; },
    close() { open = false; },
    key(code) {
      if (!open) return false;
      if (code === 'ArrowUp') selected = (selected + rows.length - 1) % rows.length;
      else if (code === 'ArrowDown') selected = (selected + 1) % rows.length;
      else if (code === 'Enter') onChoose?.(rows[selected].label);
      else return false;
      return true;
    },
    hover(x, y, width, height) { const i = rowAt(x, y, width, height); if (i >= 0) selected = i; return i; },
    click(x, y, width, height) { const i = rowAt(x, y, width, height); if (i >= 0) { selected = i; onChoose?.(rows[i].label); } return i; },
    /** Canvas pixel centre of each line, from the same rows click() tests. */
    targets(width, height) {
      const v = view(width, height);
      return Object.fromEntries(rows.map((r) => [r.label, [Math.round(v.ox + (r.x + r.w / 2) * v.scale), Math.round(v.oy + (r.y + inkTop + 12) * v.scale)]]));
    },
    step() { tick++; },
    draw(ctx, width, height) {
      if (!open) return;
      const v = view(width, height);
      ctx.save(); ctx.translate(v.ox, v.oy); ctx.scale(v.scale, v.scale);
      for (const r of rows) font.draw(ctx, r.label, r.x, r.y);
      const r = rows[selected], frame = Math.floor(tick / 2) % pentCanvases.length, pc = pentCanvases[frame];
      const cy = r.y + inkTop + 11 - pc.height / 2;
      ctx.drawImage(pc, 400 - GAME_MENU.pentagramOffsetX - pc.width / 2, cy);
      ctx.drawImage(pc, 400 + GAME_MENU.pentagramOffsetX - pc.width / 2, cy);
      ctx.restore();
    },
  };
}
