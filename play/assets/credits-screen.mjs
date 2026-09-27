// Retail credits: background and text paths follow OpenDiablo2's recovered credits screen.
import { createFont, decodeFontTbl, composeTiled, textColourPalette, TITLE_COLUMNS } from './frontend.mjs';
import { recordDc6 } from './drawlist.mjs';

export const CREDITS_ASSETS = {
  background: 'data\\global\\ui\\CharSelect\\creditsbckgexpand.dc6',
  palette: 'data\\global\\palette\\Sky\\pal.dat',
  text: 'data\\local\\ui\\eng\\ExpansionCredits.txt',
  font: 'data\\local\\font\\latin\\fontformal10.dc6',
  fontTbl: 'data\\local\\font\\latin\\fontformal10.tbl',
  pl2: 'data\\global\\palette\\Sky\\pal.pl2',
  exitButton: 'data\\global\\ui\\FrontEnd\\MediumButtonBlank.dc6',
  exitFont: 'data\\local\\font\\latin\\fontexocet10.dc6',
  exitFontTbl: 'data\\local\\font\\latin\\fontexocet10.tbl',
};
const SCREEN = { width: 800, height: 600 }, TICKS_PER_PIXEL = 60 / 50;
// OpenDiablo2 credits.go: ButtonTypeMedium at (33,543), ColorTokenRed heading / ColorTokenGold name.
export const CREDITS_LAYOUT = { exit: { x: 33, y: 543 }, colours: { red: 1, gold: 4 } };
const decodeCredits = (bytes) => new TextDecoder('utf-16le').decode(bytes).replace(/^\uFEFF/, '').split(/\r?\n/).map((line) => line.trim());

// The evidence needs to distinguish the two transformed font palettes, not merely
// report the token the line requested.  FNV-1a gives the scenario a compact identity
// for the exact RGBA table handed to each font.
function fnv1a(bytes) {
  let hash = 0x811c9dc5;
  for (const byte of bytes) { hash ^= byte; hash = Math.imul(hash, 0x01000193) >>> 0; }
  return hash.toString(16).padStart(8, '0');
}

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
  for (let i = 0; i < frame.indices.length; i++) if (frame.opaque[i]) rgba.set(palette.subarray(frame.indices[i] * 4, frame.indices[i] * 4 + 4), i * 4);
  canvas.getContext('2d').putImageData(new ImageData(rgba, canvas.width, canvas.height), 0, 0);
  return canvas;
}

// D2Win does not give every source row its own centred line.  As recovered by
// OpenDiablo2's Credits.setItemLabelPosition, two adjacent ordinary names share
// a scroll line; headings and blank rows change the insertion cadence.
function scheduleCredits(lines, measure) {
  const events = [];
  let source = 0, start = 0;
  const isHeading = (text) => text?.startsWith('*');
  while (source < lines.length) {
    const raw = lines[source];
    if (!raw) {
      start += isHeading(lines[source + 1]) ? 38 : 19;
      source++;
      continue;
    }
    const heading = isHeading(raw), nextHeading = isHeading(lines[source + 1]), nextSpace = lines[source + 1] === '';
    const labels = [];
    let doubled = false;
    if (!heading && source + 1 < lines.length && !nextHeading && !nextSpace) {
      doubled = true;
      const left = { sourceLine: source, text: raw, x: 400 - measure(raw), mode: 'gold' };
      const right = { sourceLine: source + 1, text: lines[source + 1], x: 410, mode: 'gold' };
      labels.push(left, right);
      source += 2;
    } else {
      const text = heading ? raw.slice(1) : raw;
      labels.push({ sourceLine: source, text, x: 405 - Math.trunc(measure(text) / 2), mode: heading ? 'red' : 'gold' });
      source++;
    }
    const nextIsHeading = isHeading(lines[source]);
    events.push({ start, labels });
    if (heading && nextIsHeading) start += 38;
    else if (nextIsHeading) start += doubled ? 38 : 57;
    else if (heading) start += 38;
    else start += 19;
  }
  return events;
}

/** A credits screen made only from the retail assets named by its evidence. */
export function createCreditsScreen({ read, decodeDc6, decodeDatPalette, onExit = () => {}, string = () => '', documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(CREDITS_ASSETS.palette));
  const backgroundDc6 = decodeDc6(read(CREDITS_ASSETS.background));
  const background = composeTiled(backgroundDc6, palette, TITLE_COLUMNS, documentObject);
  const textBytes = read(CREDITS_ASSETS.text), lines = decodeCredits(textBytes);
  const pl2 = read(CREDITS_ASSETS.pl2);
  const textPalettes = Object.fromEntries(Object.entries(CREDITS_LAYOUT.colours).map(([mode, colour]) => [mode, textColourPalette(pl2, palette, colour)]));
  const colourTableHashes = Object.fromEntries(Object.entries(textPalettes).map(([mode, textPalette]) => [mode, fnv1a(textPalette)]));
  const fonts = Object.fromEntries(Object.entries(textPalettes).map(([mode, textPalette]) => [mode, createFont({
    dc6: decodeDc6(read(CREDITS_ASSETS.font)), tbl: decodeFontTbl(read(CREDITS_ASSETS.fontTbl)), palette: textPalette, documentObject,
    path: CREDITS_ASSETS.font,
    // This is deliberately recorded at the image blit, rather than inferred from a text string:
    // level-2 evidence must name the requested and actually drawn font cel at its screen position.
    //
    // The font-level callback is handed the RECORDED cel row (drawlist.mjs recordDc6): `frame` is the
    // cel index, `x`/`y` the record's own anchor, and `path` the draw list's canonical form of the
    // font path (drawPath: backslashes, lower case, no `data\` prefix, no extension). Requested
    // identity is the retail path we asked for, drawn identity is what was recorded for the blit --
    // the row compares the two after normalising the requested side through the same drawPath.
    onGlyph: (blit) => glyphBlits.push({
      line: drawingLine.line, text: drawingLine.text, character: blit.char,
      requested: { font: CREDITS_ASSETS.font, cel: blit.frame, x: blit.x, y: blit.y, mode: drawingLine.mode, tableHash: colourTableHashes[drawingLine.mode] },
      // `mode` and `tableHash` come from this font's closure: swapping fonts or
      // collapsing their palettes therefore changes the drawn side of the evidence.
      drawn: { font: blit.path, cel: blit.frame, x: blit.x, y: blit.y, mode, tableHash: colourTableHashes[mode] },
    }),
  })]));
  const exitDc6 = decodeDc6(read(CREDITS_ASSETS.exitButton)), exitFrame = exitDc6.frames[0];
  const exitButton = { path: CREDITS_ASSETS.exitButton, frame: exitFrame, canvas: frameCanvas(exitFrame, palette, documentObject) };
  const exitFont = createFont({ dc6: decodeDc6(read(CREDITS_ASSETS.exitFont)), tbl: decodeFontTbl(read(CREDITS_ASSETS.exitFontTbl)), palette, documentObject,
    path: CREDITS_ASSETS.exitFont, drawAs: { op: 'celcolor', mode: 5 } });
  const exitLabel = string('strExit') || 'EXIT';
  let drawingLine = null, glyphBlits = [], labelBlits = [], lastTick = 0;
  const scene = documentObject.createElement('canvas'); scene.width = SCREEN.width; scene.height = SCREEN.height;
  const sceneCtx = scene.getContext('2d');
  const view = (width, height) => { const scale = Math.min(width / SCREEN.width, height / SCREEN.height); return { scale, ox: Math.round((width - SCREEN.width * scale) / 2), oy: Math.round((height - SCREEN.height * scale) / 2) }; };
  const schedule = scheduleCredits(lines, (text) => fonts.gold.measure(text));
  const exitRect = { ...CREDITS_LAYOUT.exit, width: exitFrame.width, height: exitFrame.height };
  const hitExit = (x, y, width, height) => {
    const v = view(width, height), sx = (x - v.ox) / v.scale, sy = (y - v.oy) / v.scale;
    return sx >= exitRect.x && sx < exitRect.x + exitRect.width && sy >= exitRect.y && sy < exitRect.y + exitRect.height;
  };
  return {
    get evidence() { return { background: { path: CREDITS_ASSETS.background, width: background.width, height: background.height, frames: backgroundDc6.frames.length, rows: background.rows, columns: background.columns }, text: { path: CREDITS_ASSETS.text, encoding: 'utf-16le', bytes: textBytes.length, lines: lines.length }, font: { path: CREDITS_ASSETS.font, glyphs: fonts.gold.measure('CREDITS'), lineHeight: fonts.gold.lineHeight, colourTableHashes }, exit: { path: CREDITS_ASSETS.exitButton, font: CREDITS_ASSETS.exitFont, label: exitLabel, rect: exitRect }, tick: lastTick, scrollPixels: Math.floor(lastTick / TICKS_PER_PIXEL), glyphBlits, labelBlits }; },
    targets(width, height) { const v = view(width, height); return { EXIT: [Math.round(v.ox + (exitRect.x + exitRect.width / 2) * v.scale), Math.round(v.oy + (exitRect.y + exitRect.height / 2) * v.scale)] }; },
    click(x, y, width, height) { if (!hitExit(x, y, width, height)) return false; onExit(); return true; },
    press(x, y, width, height) { return hitExit(x, y, width, height) ? 'exit' : null; },
    release(token, x, y, width, height) {
      return token === 'exit' && hitExit(x, y, width, height) ? this.click(x, y, width, height) : false;
    },
    cancel() {},
    draw(ctx, width, height, tick) {
      sceneCtx.fillStyle = '#000'; sceneCtx.fillRect(0, 0, SCREEN.width, SCREEN.height); sceneCtx.drawImage(background.canvas, 0, 0);
      lastTick = tick; glyphBlits = []; labelBlits = [];
      const scroll = Math.floor(tick / TICKS_PER_PIXEL);
      for (const event of schedule) {
        const anchorY = 605 + event.start - scroll;
        // Credits scroll upward. Lines already above the viewport must not prevent later lines
        // from drawing; only the first line below it can end this ordered pass.
        if (anchorY < -fonts.gold.cellHeight) continue;
        if (anchorY > SCREEN.height + fonts.gold.cellHeight) break;
        for (const label of event.labels) {
          drawingLine = { line: label.sourceLine, text: label.text, mode: label.mode };
          labelBlits.push({ ...label, y: anchorY });
          fonts[label.mode].drawAnchored(sceneCtx, label.text, label.x, anchorY);
          drawingLine = null;
        }
      }
      sceneCtx.drawImage(exitButton.canvas, exitRect.x, exitRect.y);
      recordDc6(exitButton.path, 0, exitButton.frame, exitRect.x, exitRect.y);
      exitFont.drawAnchored(sceneCtx, exitLabel, exitRect.x + Math.max(Math.trunc((exitRect.width - exitFont.measure(exitLabel)) / 2), 0), exitRect.y + exitRect.height - 9);
      const v = view(width, height); ctx.fillStyle = '#000'; ctx.fillRect(0, 0, width, height); ctx.save(); ctx.imageSmoothingEnabled = false; ctx.drawImage(scene, v.ox, v.oy, SCREEN.width * v.scale, SCREEN.height * v.scale); ctx.restore();
    },
  };
}
