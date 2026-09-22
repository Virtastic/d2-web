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

export const NPC_PANEL_ASSETS = {
  // In-game panels are drawn on the act palette, like the control panel and inventory.
  palette: 'data\\global\\palette\\ACT1\\pal.dat',
  background: 'data\\global\\ui\\panel\\trade.dc6',
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTbl: 'data\\local\\font\\latin\\font16.tbl',
};

// trade.dc6 is one image cut into 256px-wide columns, like the front-end screens.
const BACKGROUND_COLUMNS = 2;
const ROW_HEIGHT = 17;
// trade.dc6 composes to 320x432, so the list has to live inside that -- an earlier width of 420
// printed every price off the right edge of the panel and onto the world behind it.
const LIST = { x: 18, y: 104, rows: 16, width: 284 };
const GREETING_LINES = 3;

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
  const background = composeTiled(decodeDc6(read(NPC_PANEL_ASSETS.background)), palette, BACKGROUND_COLUMNS, documentObject);
  const font = createFont({
    dc6: decodeDc6(read(NPC_PANEL_ASSETS.font)), tbl: decodeFontTbl(read(NPC_PANEL_ASSETS.fontTbl)),
    palette: decodeDatPalette(read(FRONTEND.palette)), documentObject,
  });

  let scroll = 0;

  // Retail's dialogue values carry a leading numeric token and a newline before the spoken text --
  // Charsi's greeting is stored as "47\nHi there. I'm Charsi...". Retail does not show that token,
  // so it is stripped for display rather than printed as part of the line.
  const spokenText = (raw) => String(raw || '').replace(/^\s*\d+\s*\n/, '');

  const panelBox = (width, height) => ({
    x: Math.round((width - background.width) / 2),
    y: Math.round((height - background.height) / 2),
    width: background.width, height: background.height,
  });
  // One definition of where a row sits, used by both the painter and the hit test.
  const rowRect = (box, i) => ({ x: box.x + LIST.x, y: box.y + LIST.y + i * ROW_HEIGHT - 2,
    w: LIST.width, h: ROW_HEIGHT });

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
      const box = panelBox(width, height);
      for (let i = 0; i < Math.min(LIST.rows, items.length - scroll); i++) {
        const r = rowRect(box, i);
        if (px >= r.x && px < r.x + r.w && py >= r.y && py < r.y + r.h) return items[scroll + i];
      }
      return null;
    },
    /** `state` is { name, greeting, items: [{ name, cost, code }], gold }. */
    draw(ctx, width, height, state) {
      const box = panelBox(width, height);
      const w = box.width, h = box.height, x = box.x, y = box.y;
      ctx.drawImage(background.canvas, x, y);

      const line = (text, lx, ly, limit) => {
        let s = String(text ?? '');
        if (limit) while (s.length && font.measure(s) > limit) s = s.slice(0, -1);
        font.draw(ctx, s, lx, ly);
      };

      if (state.name) line(state.name, x + LIST.x, y + 26, w - 40);
      // The greeting, wrapped to the panel rather than running off it.
      const greeting = spokenText(state.greeting);
      if (greeting) {
        const words = greeting.replace(/\n+/g, ' ').split(/\s+/).filter(Boolean);
        const lines = [];
        let row = '';
        for (const word of words) {
          const next = row ? `${row} ${word}` : word;
          if (font.measure(next) > LIST.width && row) {
            lines.push(row); row = word;
            if (lines.length >= GREETING_LINES) break;
          } else row = next;
        }
        if (row && lines.length < GREETING_LINES) lines.push(row);
        for (let i = 0; i < lines.length; i++) line(lines[i], x + LIST.x, y + 50 + i * ROW_HEIGHT);
      }

      const items = Array.isArray(state.items) ? state.items : [];
      const visible = items.slice(scroll, scroll + LIST.rows);
      for (let i = 0; i < visible.length; i++) {
        const item = visible[i], ry = y + LIST.y + i * ROW_HEIGHT;
        // Anything the player cannot afford is dimmed rather than hidden, the way retail greys out
        // unaffordable stock: it is still theirs to see, just not to buy.
        const affordable = !(state.gold >= 0) || item.cost <= state.gold;
        ctx.globalAlpha = affordable ? 1 : 0.45;
        line(item.name || item.code, x + LIST.x, ry, LIST.width - 90);
        const cost = String(item.cost ?? '');
        font.draw(ctx, cost, x + LIST.x + LIST.width - font.measure(cost), ry);
        ctx.globalAlpha = 1;
      }
      if (state.gold >= 0) {
        const gold = `GOLD ${state.gold}`;
        font.draw(ctx, gold, x + LIST.x + LIST.width - font.measure(gold), y + h - 30);
      }
      if (items.length > LIST.rows) {
        const more = `${scroll + visible.length}/${items.length}`;
        font.draw(ctx, more, x + LIST.x, y + LIST.y + LIST.rows * ROW_HEIGHT + 4);
      }
      return { box: { x, y, width: w, height: h }, shown: visible.length, total: items.length, gold: state.gold ?? -1 };
    },
  };
}
