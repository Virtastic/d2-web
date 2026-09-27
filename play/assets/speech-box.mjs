// The NPC speech box: the fixed top-centre panel retail scrolls dialogue text through, distinct from
// the NPC menu (play/assets/npc-menu.mjs) and the trade window (play/assets/npc-panel.mjs). Ours had
// been showing plain talk in the trade.dc6 panel, which is not what retail draws for it
//.
//
// Geometry from retail's own draw list:
// a single fill box `box 237 7 562 129 color=0 mode=1` (D2GFX DrawBox, colour 0, DrawMode 1 = 50%
// darken -- the same primitive and mode the NPC menu's box uses), then ui\menu\boxpieces.dc6 cels
// around it. Those cels are the SAME cycle play/assets/npc-menu.mjs's menuFramePieces() already
// derived (top 2..7, bottom 16..21, left 12/10/11, right 13/14/15, corners 0/1/8/9) -- retail draws
// every box with this one border, menu or speech alike -- just over a piece-box inset one pixel
// outside the fill box on the left/top and one/two outside on the right/bottom: (x1-1, y1-1, x2+2,
// y2+1). Checked against every one of the 66 boxpieces cels in the vector: exact.
import { menuFramePieces } from './npc-menu.mjs';
import { recordPrim, recordDc6 } from './drawlist.mjs';

export const SPEECH_BOX = Object.freeze({ x1: 237, y1: 7, x2: 562, y2: 129 });
export const SPEECH_BOX_ASSETS = { pieces: 'data\\global\\ui\\menu\\boxpieces.dc6' };

/** The boxpieces cels retail draws around the fixed speech box, in call order. */
export function speechBoxPieces(box = SPEECH_BOX) {
  return menuFramePieces(box.x1 - 1, box.y1 - 1, box.x2 + 2, box.y2 + 1);
}

export function createSpeechBox({ read, decodeDc6, decodeDatPalette, createFont, decodeFontTbl, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read('data\\global\\palette\\act1\\pal.dat'));
  const pieces = decodeDc6(read(SPEECH_BOX_ASSETS.pieces));
  const pieceCanvas = new Map();
  const piece = (i) => {
    if (!pieceCanvas.has(i)) {
      const f = pieces.frames[i], c = documentObject.createElement('canvas');
      c.width = f.width; c.height = f.height;
      const rgba = new Uint8ClampedArray(f.width * f.height * 4);
      for (let k = 0; k < f.indices.length; k++) if (f.opaque[k]) rgba.set(palette.subarray(f.indices[k] * 4, f.indices[k] * 4 + 4), k * 4);
      c.getContext('2d').putImageData(new ImageData(rgba, f.width, f.height), 0, 0);
      pieceCanvas.set(i, c);
    }
    return pieceCanvas.get(i);
  };
  const font = createFont({ dc6: decodeDc6(read('data\\local\\font\\latin\\font16.dc6')),
    tbl: decodeFontTbl(read('data\\local\\font\\latin\\font16.tbl')), palette, documentObject,
    path: 'data\\local\\font\\latin\\font16.dc6', drawAs: { op: 'celcolor', mode: 5 } });

  return {
    /** `text` is the NPC's spoken line, already stripped of its leading numeric token. */
    draw(ctx, text, box = SPEECH_BOX) {
      ctx.save();
      ctx.fillStyle = 'rgba(0,0,0,0.5)';
      ctx.fillRect(box.x1, box.y1, box.x2 - box.x1, box.y2 - box.y1);
      recordPrim('box', box.x1, box.y1, box.x2, box.y2, 0, 1);
      for (const [i, ax, ay] of speechBoxPieces(box)) {
        const f = pieces.frames[i], left = ax + f.offsetX, top = ay + f.offsetY - f.height + 1;
        ctx.drawImage(piece(i), left, top);
        recordDc6(SPEECH_BOX_ASSETS.pieces, i, f, left, top);
      }
      ctx.restore();
      // Wrapped inside the box, one line per lineHeight, top-down from a small inset -- the box is
      // 122px tall so several lines fit; retail scrolls longer text, which is not yet implemented
      // here (declared below).
      // Retail's dialogue values carry a leading numeric token and a newline before the spoken text
      // (npc-panel.mjs's spokenText -- Charsi's greeting is "47\nHi there. I'm Charsi...").
      const spoken = String(text || '').replace(/^\s*\d+\s*\n/, '');
      const words = spoken.replace(/\n+/g, ' ').split(/\s+/).filter(Boolean);
      const maxWidth = (box.x2 - box.x1) - 16, lines = [];
      let row = '';
      for (const word of words) {
        const next = row ? `${row} ${word}` : word;
        if (font.measure(next) > maxWidth && row) { lines.push(row); row = word; } else row = next;
      }
      if (row) lines.push(row);
      const cx = Math.trunc((box.x1 + box.x2) / 2);
      let y = box.y1 + 10;
      for (const line of lines) {
        font.drawAnchored(ctx, line, cx - Math.trunc(font.measure(line) / 2), y + font.lineHeight - 4);
        y += font.lineHeight;
        if (y > box.y2 - 10) break;   // TODO(client-parity F21): retail scrolls the rest; we clip it
      }
      return { box: { ...box }, lines: lines.length };
    },
  };
}
