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

// Retail's measured line pitch (talk-*-r22: consecutive lines 18 px apart), not FontFormal11's 16-px cell height.
export const SPEECH_PITCH = 18;
export const SPEECH_BOX = Object.freeze({ x1: 237, y1: 7, x2: 562, y2: 129 });

// The scroll (F21's second half; the salvage branch's `npc-speech.mjs` model, its constants
// re-measured against retail's own vectors -- a retail measurement beats a reading).
//   * The block enters with its first line one pitch under the box (set, `box.y2 + SPEECH_PITCH`
//     -- the salvage's `y + height + lineHeight`) and rises at SPEECH_PX_PER_S.
//   * It settles when its LAST line reaches SPEECH_REST_LAST_Y (123 = box.y2 - 6): retail's own
//     at-rest frame is the retail captures, six lines at exactly 33,51,...,123 --
//     the first one on the box.y1+26 inset this box has always drawn statically. A block that
//     already fits therefore settles on that inset (33) and nothing about a short speech moves;
//     a longer one keeps rising until its last line reaches 123, its head leaving through the top
//     of the drawn window (box.y1+26 -- see the draw, where that bound is the measured one).
//   * The salvage's other two numbers are the reason for that re-measurement: it read `retail/
//     frames/s1.png` as a still and got a 15 px pitch and 30 px/s. Retail's own draw lists say 18
//     (Warriv 33..123; Kashya 88,106,124), and 30 px/s finishes Kashya's measured ~20 s speech in
//     2.6 s. So: pitch 18, and SPEECH_PX_PER_S, the rate the client already scrolls at.
export const SPEECH_REST_LAST_Y = 123;
/** The only constant here no vector measures -- a draw list is one instant. From the two measured
 *  speech totals (Kashya ~20 s, Akara ~60 s; npc-topics.mjs history). 9 px/s fits both. */
export const SPEECH_PX_PER_S = 9;
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
  // Retail draws the spoken line in FontFormal11, not Font16 (the retail captures:
  // `text ... font=8` immediately followed by `celcolor DATA\LOCAL\Font\Latin\FontFormal11.dc6`),
  // left-aligned at x=253 (box.x1+16), first line anchored at y=33 (box.y1+26), 18px line pitch --
  // not the centred Font16 draw this used to do (that was the NPC MENU's font, a different box).
  const font = createFont({ dc6: decodeDc6(read('data\\local\\font\\latin\\fontformal11.dc6')),
    tbl: decodeFontTbl(read('data\\local\\font\\latin\\fontformal11.tbl')), palette, documentObject,
    path: 'data\\local\\font\\latin\\fontformal11.dc6', drawAs: { op: 'celcolor', mode: 5 } });

  // The speech's own clock: the first draw of a given text starts its crawl.
  let key = null, began = 0;

  return {
    /** `text` is the NPC's spoken line, already stripped of its leading numeric token.
     *  `opts.t0` is the speech's start time (retail restarts a replayed speech from the top);
     *  `opts.now` overrides the clock, `opts.top` pins the first line's y (retail's own captured
     *  scroll position), both for a scenario to measure without a timer. */
    draw(ctx, text, box = SPEECH_BOX, opts = {}) {
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
      // Retail's dialogue values carry a leading numeric token and a newline before the spoken text
      // (npc-panel.mjs's spokenText -- Charsi's greeting is "47\nHi there. I'm Charsi...").
      const spoken = String(text || '').replace(/^\s*\d+\s*\n/, '');
      const left = box.x1 + 16;
      // The string's own line breaks are retail's: its lines keep the trailing space and a ' ' line is a blank one (measured:
      // Kashya's "...our blacksmith, " is 1 line although wider than the old 293 px wrap). No re-wrapping.
      const lines = spoken ? spoken.split('\n') : []; if (lines.length > 1 && lines[lines.length - 1] === '') lines.pop();
      lines.forEach((l, i) => { if (!l) lines[i] = ' '; });
      // The scroll (see the constants above): left-aligned at box.x1+16, every line SPEECH_PITCH
      // below the one before it, the whole block rising from under the box to its rest.
      const now = opts.now ?? performance.now();
      if (spoken !== key) { key = spoken; began = now; }          // a new speech starts its own crawl
      const elapsed = Math.max(0, now - (opts.t0 ?? began));
      const enterFirst = box.y2 + SPEECH_PITCH;
      const restFirst = Math.min(box.y1 + 26, SPEECH_REST_LAST_Y - (lines.length - 1) * SPEECH_PITCH);
      // One line has nothing to scroll and rests where it is shown (the salvage's rule, and the one
      // arm of it no capture can falsify: no speech box in any vector holds a single line).
      const travel = lines.length > 1 ? Math.max(0, enterFirst - restFirst) : 0;
      const scroll = Math.min(travel, elapsed / 1000 * SPEECH_PX_PER_S);
      const firstY = opts.top !== undefined ? opts.top : restFirst + travel - scroll;
      const drawn = [];
      // DRAWN WINDOW, measured rather than assumed: a line is painted while
      // `box.y1 + 26 <= y <= box.y2` -- the box's own inset line at the top and its bottom edge. The top
      // of the window is the fact the captures pin, not the box's fill edge: talk-kashya-firstclick-r22
      // aligns its block one line below retail's first captured line and retail paints NOTHING at y=29,
      // while akara-reward-2d paints a line at 35 -- so the window starts at 33 (the at-rest first
      // line's own inset), and a longer speech's head leaves by NOT being painted rather than by rising
      // over the box's top. (The engine-side s2174 diffs every glyph against these vectors and fails if
      // the window is widened to the fill edge.)
      lines.forEach((line, i) => {
        const y = firstY + i * SPEECH_PITCH;
        if (y >= box.y1 + 26 && y <= box.y2) { font.drawAnchored(ctx, line, left, y); drawn.push({ index: i, text: line, y }); }
      });
      // Settled and held: the caller's own "this speech is over" (retail's topic box returns by itself).
      const done = opts.top === undefined && elapsed >= (travel + SPEECH_PITCH) / SPEECH_PX_PER_S * 1000;
      return { box: { ...box }, lines: lines.length, shown: drawn.length, done, wrapped: lines, drawn,
        firstY, restFirst, enterFirst, travel, scroll: opts.top === undefined ? scroll : null,
        pitch: SPEECH_PITCH, pixelsPerSecond: SPEECH_PX_PER_S };
    },
  };
}
