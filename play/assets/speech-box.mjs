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

// D2Client 1.13c speech object: the first ASCII line is parsed with atoi and narrowed to WORD.
// FUN_6FB2BB40 advances fixed-point offset by (timeGetTime() - startTick) >> 2, times that WORD.
// Its renderer uses an 18 px (0x4800) line pitch and origin 12 + 112 = 124 px.
export const SPEECH_RATE_FALLBACK = 8;
export const SPEECH_FIXED_ONE = 1024;
export const SPEECH_TICK_DIVISOR = 4;
export const SPEECH_LINE_UNITS = 100;

/** D2Client's line parser fills a 100-WCHAR scratch line before linking it. If a source line is
 *  longer, the next node starts at the unconsumed code unit; JavaScript string slices use the same
 *  UTF-16 code-unit indexing as the native buffer. */
export function splitSpeechLines(spoken) {
  const source = spoken ? String(spoken) : '';
  const lines = [];
  let offset = 0;
  while (offset < source.length) {
    let line = '';
    let units = 0;
    while (offset < source.length && source.charAt(offset) !== '\n' && units < SPEECH_LINE_UNITS) {
      line += source.charAt(offset++);
      units++;
    }
    // Native consumes LF only when the buffer stopped short of its 100-unit limit. If
    // exactly full, the next parser iteration sees LF as an empty line node.
    if (units < SPEECH_LINE_UNITS && source.charAt(offset) === '\n') offset++;
    lines.push(line || ' ');
  }
  return lines;
}

export function parseSpeechHeader(text) {
  const value = String(text || '');
  const newline = value.indexOf('\n');
  if (newline < 0) return { rate: SPEECH_RATE_FALLBACK, spoken: value, hasHeader: false };
  const header = value.slice(0, newline);
  // The native UTF-16-to-ASCII conversion rejects a non-ASCII header and uses 8.
  if ([...header].some((ch) => ch.codePointAt(0) >= 0x80))
    return { rate: SPEECH_RATE_FALLBACK, spoken: value.slice(newline + 1), hasHeader: true };
  const parsed = /^[\t\n\v\f\r ]*([+-]?\d+)/.exec(header);
  const rate = parsed ? (Number(parsed[1]) & 0xffff) : 0;
  return { rate, spoken: value.slice(newline + 1), hasHeader: true };
}
export function speechOffsetFixed(rate, elapsedMs) {
  const elapsed = Math.max(0, Math.floor(elapsedMs));
  return ((Math.floor(elapsed / SPEECH_TICK_DIVISOR) * (rate & 0xffff)) >>> 0);
}
/** Model the renderer's repeated-timeGetTime branch, including its averaged sub-tick step. */
export function createSpeechClock(rate) {
  let startTick = null, lastTick = null, repeatStep = 0, repeatCount = 0, repeatAccum = 0;
  return (tick) => {
    tick >>>= 0;
    if (startTick === null) { startTick = tick; lastTick = tick; return 0; }
    let local = 0;
    if (tick === lastTick) {
      if (repeatStep === 0) repeatCount = (repeatCount + 1) & 0xffff;
      else { repeatAccum = (repeatAccum + repeatStep) >>> 0; local = repeatAccum; }
    } else {
      repeatAccum = 0;
      if (repeatStep === 0 && repeatCount !== 0)
        repeatStep = Math.floor(((tick - lastTick) >>> 0) / repeatCount);
      lastTick = tick;
    }
    const elapsedQuanta = Math.floor(((tick - startTick) >>> 0) / SPEECH_TICK_DIVISOR);
    return (((local + elapsedQuanta) * (rate & 0xffff)) >>> 0);
  };
}

export const SPEECH_BOX_ASSETS = { pieces: 'data\\global\\ui\\menu\\boxpieces.dc6' };

/**
 * D2Client centers the fixed 325 px speech object with integer division. The panel and text
 * coordinates below are already expressed in the native 800x600 screen space (box starts at
 * x=237, text starts at y=124), so only translate that coordinate space. In particular, speech
 * stays pinned to the top of taller views instead of following the general HUD's vertical center.
 */
export function speechOrigin(width) {
  return { scale: 1, x: Math.trunc((width - (SPEECH_BOX.x2 - SPEECH_BOX.x1)) / 2) - SPEECH_BOX.x1, y: 0 };
}

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

  let clockKey = null, clockStart = null, clock = null;

  return {
    /** `text` is the raw D2Lang value: native speech strings start with an atoi rate line. */
    draw(ctx, text, box = SPEECH_BOX, opts = {}) {
      // FUN_6FB2D200 uses its own fixed-width dialog initializer, not the shared HUD viewport
      // origin: x=(screen width-325)/2 and y=12. These coordinates already include that native
      // y origin (box y=7, initial text anchor y=124), so translate only the 325 px box in x.
      const width = ctx.canvas?.width || 800;
      const origin = speechOrigin(width);
      ctx.save();
      try {
        ctx.translate(origin.x, origin.y);
        ctx.scale(origin.scale, origin.scale);
        ctx.fillStyle = 'rgba(0,0,0,0.5)';
        ctx.fillRect(box.x1, box.y1, box.x2 - box.x1, box.y2 - box.y1);
        recordPrim('box', box.x1, box.y1, box.x2, box.y2, 0, 1);
        for (const [i, ax, ay] of speechBoxPieces(box)) {
          const f = pieces.frames[i], left = ax + f.offsetX, top = ay + f.offsetY - f.height + 1;
          ctx.drawImage(piece(i), left, top);
          recordDc6(SPEECH_BOX_ASSETS.pieces, i, f, left, top);
        }
        const parsed = parseSpeechHeader(text);
        const spoken = parsed.spoken;
        const left = box.x1 + 16;
        const lines = splitSpeechLines(spoken);
        const now = opts.now ?? performance.now();
        const messageKey = `${parsed.rate}\u0000${spoken}${opts.messageInstance === undefined ? '' : `\u0000${opts.messageInstance}`}`;
        const startTick = opts.t0 === undefined ? null : Math.floor(opts.t0) >>> 0;
        if (messageKey !== clockKey || (startTick !== null && startTick !== clockStart)) {
          clockKey = messageKey; clockStart = startTick; clock = createSpeechClock(parsed.rate);
        }
        const offsetFixed = opts.top === undefined ? clock(Math.floor(now) >>> 0) : null;
        const offsetPx = offsetFixed === null ? null : Math.floor(offsetFixed / SPEECH_FIXED_ONE);
        // FUN_6FB2BB40 anchors the bottom of the linked text block at 124-offset, then walks up
        // by 18 px. The native residual gate allows baselines from 31 through 124 inclusive.
        const firstY = opts.top !== undefined ? opts.top : 124 - offsetPx;
        const drawn = [];
        lines.forEach((line, i) => {
          const y = firstY + i * SPEECH_PITCH;
          if (y >= 31 && y <= 124) { font.drawAnchored(ctx, line, left, y); drawn.push({ index: i, text: line, y }); }
        });
        // Native completion: ((max(1, lineCount-1)*18 + 112) < offsetPx).
        const threshold = Math.max(1, lines.length - 1) * SPEECH_PITCH + 112;
        const done = offsetPx !== null && threshold < offsetPx;
        const screenBox = { x: origin.x + box.x1 * origin.scale, y: origin.y + box.y1 * origin.scale,
          width: (box.x2 - box.x1) * origin.scale, height: (box.y2 - box.y1) * origin.scale };
        return { box: { ...box }, origin, screenBox,
          screenDrawn: drawn.map((line) => ({ ...line, x: origin.x + left * origin.scale,
            screenX: origin.x + left * origin.scale, screenY: origin.y + line.y * origin.scale })),
          lines: lines.length, shown: drawn.length, done, wrapped: lines, drawn,
          firstY, restFirst: 124, enterFirst: 124, travel: 0, scroll: offsetPx,
          pitch: SPEECH_PITCH, rate: parsed.rate, offsetFixed, offsetPx, threshold };
      } finally {
        ctx.restore();
      }

    },
  };
}
