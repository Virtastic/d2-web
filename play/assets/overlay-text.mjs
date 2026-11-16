// Text on the platform's DOM overlays (the controller's key grids and panels, the touch layer, the error notices)
// in retail's own font. These overlays have no retail screen behind them, so there is no layout to copy; what they
// share with the game is the lettering: a line is laid out as D2Win lays one out -- glyph by glyph from the left
// edge, each glyph the font cel its code point maps to, each advancing by its font-table width, whole pixels --
// and painted from those cels into a canvas that stands where the browser's text stood. No CSS font family is
// named anywhere on these overlays. Until the font can be read (no game data mounted yet) the element keeps its
// plain text in the page's own face and is painted as soon as the font is there.
let provider = () => null;
const pending = new Set();
let timer = null;

function retryLater() {
  if (timer || !pending.size || typeof setTimeout !== 'function') return;
  timer = setTimeout(() => { timer = null; flushOverlayText(); }, 500);
  timer?.unref?.();
}

/** The shell names the font once (its Font16, the game's label face); it may answer null until the data is mounted. */
export function setOverlayFont(get) { provider = get; flushOverlayText(); }

/** Break `text` at spaces so no line is wider than `wrap` pixels in `font` (explicit newlines are kept). */
export function wrapOverlayText(font, text, wrap) {
  const out = [];
  for (const para of String(text).split('\n')) {
    if (!(wrap > 0)) { out.push(para); continue; }
    let line = '';
    for (const word of para.split(' ')) {
      const next = line ? `${line} ${word}` : word;
      if (line && font.measure(next) > wrap) { out.push(line); line = word; } else line = next;
    }
    out.push(line);
  }
  return out;
}

/**
 * Paint `lines` with their left edge at 0, one font line height apart; returns the glyphs it drew (font cel index,
 * pen x, line). The cels go straight to this canvas: an overlay is not part of the game frame, so nothing is added
 * to the frame's draw list.
 */
export function paintOverlayLines(ctx, font, lines) {
  const glyphs = [];
  lines.forEach((line, i) => {
    let pen = 0;
    for (const ch of String(line)) {
      const g = font.glyph(ch.codePointAt(0));
      if (!g) continue;
      if (g.canvas.width && g.canvas.height && g.frame.width && g.frame.height) {
        ctx.drawImage(g.canvas, pen, i * font.lineHeight);
        glyphs.push({ character: ch, index: g.index, x: pen, line: i });
      }
      pen += g.advance;
    }
  });
  return glyphs;
}

function paint(el, text, opts) {
  const font = provider(), documentObject = opts.documentObject ?? globalThis.document;
  if (!font || !documentObject?.createElement) return false;
  const lines = wrapOverlayText(font, text, opts.wrap);
  const c = documentObject.createElement('canvas');
  c.width = Math.max(1, ...lines.map((l) => font.measure(l)));
  c.height = Math.max(1, lines.length * font.lineHeight);
  const glyphs = paintOverlayLines(c.getContext('2d'), font, lines);
  // The canvas scales with the element's own font SIZE (16px shows the cels 1:1); its fallback content is the
  // text itself, so the element still reads as its text to a screen reader and to `textContent`.
  if (c.style) Object.assign(c.style, { height: `${c.height / 16}em`, width: 'auto', maxWidth: '100%', imageRendering: 'pixelated', verticalAlign: 'middle' });
  c.textContent = text;
  el.replaceChildren(c);
  el.__overlayGlyphs = glyphs;
  return true;
}

/** Set an overlay element's text. Returns true when it is painted in the retail font. */
export function setOverlayText(el, text, opts = {}) {
  if (!el) return false;
  text = String(text ?? '');
  if (el.__overlayText === text && el.__overlayPainted) return true;
  el.textContent = text;
  el.__overlayText = text; el.__overlayOpts = opts; el.__overlayPainted = false; el.__overlayGlyphs = null;
  pending.delete(el);
  if (!text || typeof el.replaceChildren !== 'function') return false;
  if ((el.__overlayPainted = paint(el, text, opts))) return true;
  pending.add(el);
  retryLater();
  return false;
}

/** Paint every element still waiting for the font; returns how many still wait. */
export function flushOverlayText() {
  for (const el of [...pending]) if ((el.__overlayPainted = paint(el, el.__overlayText, el.__overlayOpts))) pending.delete(el);
  retryLater();
  return pending.size;
}
