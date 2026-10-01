// F215 (SOUND OPTIONS) and F216 (VIDEO OPTIONS): the two pages the OPTIONS page's first two lines open.
// Both are cel art, not font text -- retail draws them from DATA\LOCAL\UI\ENG\ plus the widget sliders,
// so nothing here is derived from pixels: every anchor, frame, mode and box below is a record in
// retail's own draw lists (the retail captures, opt-video-page.txt and their
// clicked/toggled siblings), and s2134 asserts those records per blit (level 2 identity).
//
// Prior art: reference/OpenDiablo2/d2game/d2player/escape_menu.go holds the same option order and
// labels (newSoundOptionsLayout / newVideoOptionsLayout) but draws them as font text, so it fixes the
// structure and the click semantics, never the coordinates.
//
// The layout is one table: 7 rows on a 45 px pitch, the label cels at x=170 (bottom anchor), the slider
// track from x=341 to x=630, the knob at its left end for v=0 and 266 px along for v=1 (measured:
// Sound/Music/3DBias/Gamma/Contrast defaults and both click captures fit knobX = 339 + round(v*266)
// exactly, with the box split 13 px right of the knob).
import { recordDc6, recordPrim } from './drawlist.mjs';

const UI = 'data\\local\\ui\\eng\\';
const WID = 'data\\global\\ui\\widgets\\';
const PENT = 'data\\global\\ui\\CURSOR\\pentspin.DC6';
const ACT_PALETTE = (act) => `data\\global\\palette\\ACT${act}\\pal.dat`;

export const OPTIONS_PAGE_LAYOUT = Object.freeze({
  rowY0: 159, pitch: 45, titleY: 114,
  trackX1: 341, trackX2: 630, knobX0: 339, knobSpan: 266, splitDx: 13,
  barDx: 340, barDy: 2, pentDx: 15, pentX: [99, 649],
  bandAbove: 11, bandBelow: 34,
});

// 7 rows each, 45 px apart: the last is retail's PREVIOUS cel (SPrevious.dc6) in both pages.
// `labels` are [path, frame, anchorX] at the row's own y; `mode` is D2MOO's DrawMode (1 = dimmed, which
// is how retail draws the rows needing hardware the capture did not report: 3DSound/EAX/3DBias, and
// Perspective on the video page).
export const OPTIONS_PAGE_ROWS = Object.freeze({
  sound: [
    { key: 'sound', labels: [['Sound', 0, 170]], mode: 5, slider: { bar: 'OptBar', v: 1 } },
    { key: 'music', labels: [['Music', 0, 170]], mode: 5, slider: { bar: 'OptBar', v: 0.5 } },
    { key: '3dsound', labels: [['3DSound', 0, 170]], mode: 1, value: [['SmallOff', 0, 579]] },
    { key: 'eax', labels: [['EAX', 0, 170], ['EAX', 1, 426]], mode: 1, value: [['SmallOff', 0, 579]] },
    { key: '3dbias', labels: [['3DBias', 0, 170]], mode: 1, slider: { bar: 'OptBarC', v: 0.5 } },
    { key: 'npcspeech', labels: [['NPCSpeech', 0, 170]], mode: 5 },
    { key: 'previous', labels: [['SPrevious', 0, 284]], mode: 5 },
  ],
  video: [
    { key: 'resolution', labels: [['Resolution', 0, 170]], mode: 5, value: [['800x600', 0, 495]] },
    { key: 'lighting', labels: [['LightQuality', 0, 170], ['LightQuality', 1, 426]], mode: 5 },
    { key: 'shadows', labels: [['BlendShadow', 0, 170], ['BlendShadow', 1, 426]], mode: 5 },
    { key: 'perspective', labels: [['Perspective', 0, 170]], mode: 1, value: [['SmallOff', 0, 579]] },
    { key: 'gamma', labels: [['Gamma', 0, 170]], mode: 5, slider: { bar: 'OptBarC', v: 0.5 } },
    { key: 'contrast', labels: [['Contrast', 0, 170]], mode: 5, slider: { bar: 'OptBarC', v: 1 } },
    { key: 'previous', labels: [['SPrevious', 0, 284]], mode: 5 },
  ],
});
export const OPTIONS_PAGE_TITLES = Object.freeze({
  sound: { path: 'SoundOptions', x: [245, 501] },
  video: { path: 'VideoOptions', x: [252, 508] },
});

// The three multi-state rows: the cels retail drew for each state, from the page's toggle captures.
const NPC_SPEECH = [
  [['AudioText', 0, 370], ['AudioText', 1, 626]],   // both: the NPC's voice and the text
  [['TextOnly', 0, 461]],
  [['AudioOnly', 0, 446]],
];
const LIGHTING = [[['High', 0, 573]], [['Low', 0, 564]]];
const SHADOWS = [[['SmallOn', 0, 590]], [['SmallOff', 0, 579]]];

const knobX = (v) => 339 + Math.round(Math.max(0, Math.min(1, v)) * 266);

/** One page's full draw order (retail's own call order) from the row table and the current state. */
export function optionsPageDrawOrder(name, state = {}) {
  const L = OPTIONS_PAGE_LAYOUT, rows = OPTIONS_PAGE_ROWS[name];
  if (!rows) throw new Error(`unknown options page ${name}`);
  const title = OPTIONS_PAGE_TITLES[name], out = [];
  out.push({ op: 'cel', path: title.path, frame: 0, x: title.x[0], y: L.titleY, mode: 5 });
  out.push({ op: 'cel', path: title.path, frame: 1, x: title.x[1], y: L.titleY, mode: 5 });
  rows.forEach((row, i) => {
    const y = L.rowY0 + i * L.pitch;
    for (const [path, frame, x] of row.labels) out.push({ op: 'cel', path, frame, x, y, mode: row.mode });
    const value = row.key === 'npcspeech' ? NPC_SPEECH[state.npcspeech ?? 0]
      : row.key === 'lighting' ? LIGHTING[state.lighting ?? 0]
        : row.key === 'shadows' ? SHADOWS[state.shadows ?? 0] : row.value;
    for (const [path, frame, x] of value ?? []) out.push({ op: 'cel', path, frame, x, y, mode: row.mode });
    if (!row.slider) return;
    const v = state[row.key] ?? row.slider.v, split = knobX(v) + L.splitDx;
    const cell = row.slider.bar === 'OptBar' ? [2, 0] : [1, 1];
    out.push({ op: 'box', x1: L.trackX1, y1: y - 28, x2: split, y2: y + 2, color: 0, mode: cell[0] });
    out.push({ op: 'box', x1: split, y1: y - 28, x2: L.trackX2, y2: y + 2, color: 0, mode: cell[1] });
    out.push({ op: 'cel', path: row.slider.bar, frame: 0, x: L.barDx, y: y + L.barDy, mode: row.mode });
    out.push({ op: 'cel', path: row.slider.bar, frame: 1, x: 596, y: y + L.barDy, mode: row.mode });
    out.push({ op: 'cel', path: 'OptSkull', frame: 0, x: knobX(v), y: y + (row.slider.bar === 'OptBar' ? 0 : 1), mode: row.mode });
  });
  return out;
}

/** Which row a logical 800x600 point falls in, retail's click band (rowY - 11 .. rowY + 34). */
export function optionsPageRowAt(name, x, y) {
  const L = OPTIONS_PAGE_LAYOUT;
  if (!(x >= 0 && x <= 800)) return -1;
  return OPTIONS_PAGE_ROWS[name].findIndex((_, i) => {
    const ry = L.rowY0 + i * L.pitch;
    return y >= ry - L.bandAbove && y < ry + L.bandBelow;
  });
}

/** The sources are always cels except the three slider widgets, which live under data\global\ui\widgets. */
const isWidget = (path) => path === 'OptBar' || path === 'OptBarC' || path === 'OptSkull';
const sourcePath = (path) => `${isWidget(path) ? WID : UI}${path}.dc6`;

/**
 * The two pages, drawn and driven. Deps as elsewhere; `act` picks the in-game palette (the pages are
 * drawn over the game, so it is the act palette).
 */
export function createOptionsPages({ read, decodeDc6, decodeDatPalette, documentObject = globalThis.document, act = 1, initialVolume = null, onVolume = null }) {
  const palette = decodeDatPalette(read(ACT_PALETTE(act)));
  const decoded = new Map(), images = new Map();
  const dc6 = (path) => {
    let dc = decoded.get(path);
    if (!dc) { dc = decodeDc6(read(sourcePath(path))); decoded.set(path, dc); }
    return dc;
  };
  const toCanvas = (f) => {
    const c = documentObject.createElement('canvas');
    c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
    if (f.width && f.height) {
      const rgba = new Uint8ClampedArray(f.width * f.height * 4);
      for (let i = 0; i < f.indices.length; i++) if (f.opaque[i]) rgba.set(palette.subarray(f.indices[i] * 4, f.indices[i] * 4 + 4), i * 4);
      c.getContext('2d').putImageData(new ImageData(rgba, f.width, f.height), 0, 0);
    }
    return c;
  };
  const frameOf = (path, index) => dc6(path).frames[index];
  const imageOf = (path, index) => {
    const key = `${path}|${index}`;
    let c = images.get(key);
    if (!c) { c = toCanvas(frameOf(path, index)); images.set(key, c); }
    return c;
  };
  const pent = decodeDc6(read(PENT)), pentCanvases = pent.frames.map(toCanvas);

  // F235: Sound and Music start at the value the audio graph is holding (retail's own defaults on a
  // first visit), so the knob this page draws is the volume that will actually be played.
  const state = { npcspeech: 0, lighting: 0, shadows: 0, ...(initialVolume ?? {}) };
  let page = null, selected = 0, tick = 0;
  const rows = () => OPTIONS_PAGE_ROWS[page];
  const view = (width, height) => {
    const scale = Math.min(width / 800, height / 600);
    return { scale, ox: (width - 800 * scale) / 2, oy: (height - 600 * scale) / 2 };
  };
  const logical = (x, y, width, height) => { const v = view(width, height); return { x: (x - v.ox) / v.scale, y: (y - v.oy) / v.scale }; };
  const rowAt = (x, y, width, height) => { const p = logical(x, y, width, height); return optionsPageRowAt(page, p.x, p.y); };

  /** What a click on a row does: sliders take the value at the pointer, toggles cycle, PREVIOUS closes. */
  const choose = (i, lx) => {
    const L = OPTIONS_PAGE_LAYOUT, row = rows()[i];
    if (!row) return null;
    if (row.slider) { state[row.key] = Math.max(0, Math.min(1, (lx - L.trackX1) / (L.trackX2 - L.trackX1))); onVolume?.(row.key, state[row.key]); return { row: row.key, value: state[row.key] }; }
    if (row.key === 'npcspeech') { state.npcspeech = (state.npcspeech + 1) % NPC_SPEECH.length; return { row: row.key, value: state.npcspeech }; }
    if (row.key === 'lighting') { state.lighting = 1 - state.lighting; return { row: row.key, value: state.lighting }; }
    if (row.key === 'shadows') { state.shadows = 1 - state.shadows; return { row: row.key, value: state.shadows }; }
    // 3DSound/EAX/3DBias/Perspective are dimmed: retail reports the hardware absent and they do nothing.
    return null;
  };

  return {
    get isOpen() { return page !== null; },   // not `open`: an object literal's `open(name)` method would shadow the getter
    get page() { return page; },
    open(name) { if (!OPTIONS_PAGE_ROWS[name]) throw new Error(`unknown options page ${name}`); page = name; selected = rows().length - 1; },
    close() { page = null; },
    get state() { return { ...state }; },
    get evidence() {
      if (!page) return { open: false };
      const L = OPTIONS_PAGE_LAYOUT;
      return {
        open: true, page, selected: rows()[selected]?.key ?? null, state: { ...state },
        rows: rows().map((r, i) => ({ key: r.key, y: L.rowY0 + i * L.pitch, mode: r.mode, knobX: r.slider ? knobX(state[r.key] ?? r.slider.v) : null })),
        draws: optionsPageDrawOrder(page, state).length,
      };
    },
    hover(x, y, width, height) { const i = rowAt(x, y, width, height); if (i >= 0) selected = i; return i; },
    /** Returns { row, value } | 'previous' | null, so a caller can route PREVIOUS back to the menu. */
    click(x, y, width, height) {
      const i = rowAt(x, y, width, height);
      if (i < 0) return null;
      selected = i;
      const row = rows()[i];
      if (row.key === 'previous') return 'previous';
      return choose(i, logical(x, y, width, height).x);
    },
    /** Canvas pixel centre of each row, from the same rows click() tests. */
    targets(width, height) {
      if (!page) return {};
      const L = OPTIONS_PAGE_LAYOUT, v = view(width, height);
      return Object.fromEntries(rows().map((r, i) => [r.key, [Math.round(v.ox + 400 * v.scale), Math.round(v.oy + (L.rowY0 + i * L.pitch - 5) * v.scale)]]));
    },
    step() { tick++; },
    draw(ctx, width, height) {
      if (!page) return null;
      const v = view(width, height);
      ctx.save(); ctx.translate(v.ox, v.oy); ctx.scale(v.scale, v.scale);
      for (const item of optionsPageDrawOrder(page, state)) {
        if (item.op === 'box') {
          ctx.fillStyle = `rgba(0,0,0,${[0.25, 0.5, 0.75][item.mode] ?? 0.5})`;
          ctx.fillRect(item.x1, item.y1, item.x2 - item.x1, item.y2 - item.y1);
          recordPrim('box', item.x1, item.y1, item.x2, item.y2, item.color, item.mode);
          continue;
        }
        const f = frameOf(item.path, item.frame);
        const left = item.x + f.offsetX, top = item.y + f.offsetY - f.height + 1;
        ctx.drawImage(imageOf(item.path, item.frame), left, top);
        recordDc6(sourcePath(item.path), item.frame, f, left, top, { mode: item.mode });
      }
      // The pentagram cursor flanks the selected row (retail: x 99 and 649, 15 px below it). It spins,
      // so the frame is ours -- s2134 excludes it from the diff and asserts the flanking separately.
      const py = OPTIONS_PAGE_LAYOUT.rowY0 + selected * OPTIONS_PAGE_LAYOUT.pitch + OPTIONS_PAGE_LAYOUT.pentDx;
      const spin = Math.floor(tick / 2) % pent.frames.length, pf = pent.frames[spin];
      for (const x of OPTIONS_PAGE_LAYOUT.pentX) ctx.drawImage(pentCanvases[spin], x + pf.offsetX, py + pf.offsetY - pf.height + 1);
      ctx.restore();
      return this.evidence;
    },
  };
}
