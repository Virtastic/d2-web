// Character select, laid out from retail 1.13c's own draw list (the retail captures,
// client-parity C3n; s274 compares the frame): characterselectscreenEXP.dc6 through Sky, the selected
// character's name in Font42 as the title, one card per character (charselectbox.dc6 behind the
// selected one; the name in gold, "Level N Class" in white, "EXPANSION CHARACTER" in green, Font16),
// and retail's buttons -- CREATE NEW CHARACTER, CONVERT TO EXPANSION (retail's draw mode 1, PL2
// AlphaBlend 50%: an expansion character cannot be converted), DELETE CHARACTER (TallButtonBlank), EXIT and OK
// (MediumButtonBlank), their labels in exocet10 through PL2 MultiplicativeBlend (mode 4).
// Declared gap (client-parity C3f): the portrait -- retail draws the character's composite DCC
// (tokens TN hth/1hs, frame 12, direction 0) at the card's (67,165); this client does not yet.
//
// Everything about which characters exist comes from the store: real .d2s files the engine wrote.

import { createFont, decodeFontTbl, FRONTEND, TITLE_COLUMNS, composeTiled, createBlender, textColourPalette } from './frontend.mjs';
import { recordDc6 } from './drawlist.mjs';
import { CLASS_NAMES } from './save-store.mjs';
import { decodeCof, decodeDcc, composeCofFrame } from './unit-formats.mjs';
import { playerClassInfo, dccDirection } from './unit-renderer.mjs';

export const SELECT_ASSETS = {
  background: 'data\\global\\ui\\CharSelect\\characterselectscreenEXP.dc6',
  box: 'data\\global\\ui\\CharSelect\\charselectbox.dc6',
  tall: 'data\\global\\ui\\CharSelect\\TallButtonBlank.dc6',
  medium: 'data\\global\\ui\\FrontEnd\\MediumButtonBlank.dc6',
  title: 'data\\local\\font\\latin\\font42.dc6', titleTbl: 'data\\local\\font\\latin\\font42.tbl',
  card: 'data\\local\\font\\latin\\font16.dc6', cardTbl: 'data\\local\\font\\latin\\font16.tbl',
  // THIS screen is Sky (retail's frame buffer); the creation screen next door is fechar.
  palette: 'data\\global\\palette\\Sky\\pal.dat', pl2: 'data\\global\\palette\\Sky\\pal.pl2',
};

// Retail's placements (draw list anchors are bottom rows, inclusive; top = anchor - h + 1).
const CARD = { x: 37, top: 86, w: 272, h: 93, colStep: 272, rowStep: 95, textX: 113, lines: [113, 127, 141] };
const TITLE = { anchor: 63, centre: 318 };   // centre: retail's 'Oracle' starts at x 253 (one sample)
const BUTTONS = {
  create: { kind: 'tall', x: 33, top: 469, lines: ['CREATE NEW', 'CHARACTER'] },
  convert: { kind: 'tall', x: 233, top: 469, lines: ['CONVERT TO', 'EXPANSION'], disabled: true },
  delete: { kind: 'tall', x: 433, top: 469, lines: ['DELETE', 'CHARACTER'] },
  exit: { kind: 'medium', x: 33, top: 538, lines: ['EXIT'] },
  ok: { kind: 'medium', x: 627, top: 538, lines: ['OK'] },
};
const ORDER = ['ok', 'exit', 'create', 'convert', 'delete'];   // retail's draw order
const SCREEN = { width: 800, height: 600 };

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  if (!frame.width || !frame.height) return canvas;
  const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
  for (let i = 0; i < frame.indices.length; i++) {
    if (!frame.opaque[i]) continue;
    const s = frame.indices[i] * 4; rgba.set(palette.subarray(s, s + 4), i * 4);
  }
  canvas.getContext('2d').putImageData(new ImageData(rgba, frame.width, frame.height), 0, 0);
  return canvas;
}

/** `characters` is the store's listing. onPlay(name), onCreate(), onCancel(). */
export function createCharacterSelect({ read, decodeDc6, decodeDatPalette, characters, onPlay, onCreate, onCancel, componentToken = null, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(SELECT_ASSETS.palette));
  const pl2 = read(SELECT_ASSETS.pl2);
  const background = composeTiled(decodeDc6(read(SELECT_ASSETS.background)), palette, TITLE_COLUMNS, documentObject);
  const boxDc6 = decodeDc6(read(SELECT_ASSETS.box));
  const boxParts = boxDc6.frames.slice(0, 2).map((f) => frameCanvas(f, palette, documentObject));
  const button = (path) => { const dc6 = decodeDc6(read(path)); return { path, frame: dc6.frames[0], canvas: frameCanvas(dc6.frames[0], palette, documentObject) }; };
  const kinds = { tall: button(SELECT_ASSETS.tall), medium: button(SELECT_ASSETS.medium) };
  const blend = createBlender(pl2, palette);
  // Retail's DrawMode 1 (the disabled CONVERT button): PL2 AlphaBlend[1] (50%), dst = table[under][src],
  // index-exact like the title's modes 3 and 4 (OpenDiablo2 d2pl2: AlphaBlend follows the base
  // palette, 32 light levels, 16 inverse colours and the selected-unit shift).
  const ALPHA50 = 1024 + 32 * 256 + 16 * 256 + 256 + 65536;
  const index = new Map(); for (let j = 255; j >= 0; j--) index.set((palette[j * 4] << 16) | (palette[j * 4 + 1] << 8) | palette[j * 4 + 2], j);
  const trans50 = (ctx, frame, left, top) => {
    const img = ctx.getImageData(left, top, frame.width, frame.height), d = img.data;
    for (let k = 0; k < frame.indices.length; k++) {
      if (!frame.opaque[k]) continue;
      const under = index.get((d[k * 4] << 16) | (d[k * 4 + 1] << 8) | d[k * 4 + 2]);
      const c = under === undefined ? frame.indices[k] : pl2[ALPHA50 + under * 256 + frame.indices[k]];
      d[k * 4] = palette[c * 4]; d[k * 4 + 1] = palette[c * 4 + 1]; d[k * 4 + 2] = palette[c * 4 + 2]; d[k * 4 + 3] = 255;
    }
    ctx.putImageData(img, left, top);
  };
  const labelFont = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette, documentObject, path: FRONTEND.font, drawAs: { op: 'celcolor', mode: 4, blend } });
  const colourFont = (dc6Path, tblPath, colour) => createFont({ dc6: decodeDc6(read(dc6Path)), tbl: decodeFontTbl(read(tblPath)),
    palette: colour ? textColourPalette(pl2, palette, colour) : palette, documentObject, path: dc6Path, drawAs: { op: 'celcolor', mode: 5 } });
  const titleFont = colourFont(SELECT_ASSETS.title, SELECT_ASSETS.titleTbl, 0);
  const cardFonts = [4, 0, 2].map((c) => colourFont(SELECT_ASSETS.card, SELECT_ASSETS.cardTbl, c));

  // The portrait (retail's draw list: the composite DCC at the card's (67,165), town-neutral 'TN',
  // frame 12, direction 0): each COF layer's component from the save's appearance bytes -- 0xFF the
  // class's plain 'lit', 1-3 lit/med/hvy, an item index through the engine's component table
  // (componentToken: d2_component_token) -- and the right hand's weapon class picking the COF.
  const COMP = ['HD', 'TR', 'LG', 'RA', 'LA', 'RH', 'LH', 'SH', 'S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8'];
  const units = decodeDatPalette(read('data\\global\\palette\\Units\\pal.dat'));
  const portraitError = [];
  const portrait = (ch) => {
    try {
      const token = playerClassInfo(ch.playerClass).token, app = ch.appearance || [];
      const item = (i) => { const v = app[i]; if (v === undefined || v === 0xFF || v === 0) return null; const t = componentToken?.(v) || ''; const [code, wclass] = t.split(' '); return code ? { code, wclass } : null; };
      const rh = item(5), lh = item(6);
      const wclass = (rh?.wclass || lh?.wclass || 'hth').toUpperCase();
      const cof = decodeCof(read(`data\\global\\CHARS\\${token}\\COF\\${token}TN${wclass}.COF`));
      // Retail's record names direction 0 of the 64-step compass; the DCC's own index is dccDirection's.
      const dir = dccDirection(0, cof.directions);
      const components = new Map();
      for (const layer of cof.layerRecords) {
        const comp = COMP[layer.component]; if (!comp) continue;
        const v = app[layer.component], it = item(layer.component);
        const base = it ? it.code : (v >= 1 && v <= 3 ? ['lit', 'med', 'hvy'][v - 1] : 'lit');
        try { components.set(layer.component, decodeDcc(read(`data\\global\\CHARS\\${token}\\${comp}\\${token}${comp}${base}TN${layer.weaponClass.toUpperCase()}.DCC`), { directions: [dir] })); }
        catch (e) { /* a layer the COF names but this token lacks is not drawn (retail skips it too) */ }
      }
      const f = composeCofFrame(cof, dir, Math.min(12, cof.framesPerDirection - 1), components);
      const c = documentObject.createElement('canvas'); c.width = Math.max(1, f.width); c.height = Math.max(1, f.height);
      if (f.width && f.height) { const rgba = new Uint8ClampedArray(f.width * f.height * 4);
        for (let k = 0; k < f.indexed.length; k++) if (f.opaque[k]) { const s = f.indexed[k] * 4; rgba[k * 4] = units[s]; rgba[k * 4 + 1] = units[s + 1]; rgba[k * 4 + 2] = units[s + 2]; rgba[k * 4 + 3] = 255; }
        c.getContext('2d').putImageData(new ImageData(rgba, f.width, f.height), 0, 0); }
      return { canvas: c, x: f.xOffset, y: f.yOffset, wclass };
    } catch (error) { portraitError.push(String(error)); return null; }
  };
  const list = characters.slice(0, 8);
  const portraits = list.map(portrait);
  let selected = list.length ? 0 : -1;
  const scene = documentObject.createElement('canvas'); scene.width = SCREEN.width; scene.height = SCREEN.height;
  const sceneCtx = scene.getContext('2d', { willReadFrequently: true });

  const view = (width, height) => {
    const scale = Math.min(width / SCREEN.width, height / SCREEN.height);
    return { scale, ox: Math.round((width - SCREEN.width * scale) / 2), oy: Math.round((height - SCREEN.height * scale) / 2) };
  };
  const toScreen = (x, y, width, height) => { const v = view(width, height); return { x: (x - v.ox) / v.scale, y: (y - v.oy) / v.scale }; };
  const cardBox = (i) => ({ x: CARD.x + (i % 2) * CARD.colStep, y: CARD.top + (i >> 1) * CARD.rowStep, w: CARD.w, h: CARD.h });
  const buttonBox = (id) => { const b = BUTTONS[id], k = kinds[b.kind]; return { x: b.x, y: b.top, w: k.frame.width, h: k.frame.height }; };
  const inBox = (p, b) => p.x >= b.x && p.x < b.x + b.w && p.y >= b.y && p.y < b.y + b.h;
  const describe = (ch) => `Level ${ch.level} ${CLASS_NAMES[ch.playerClass] ?? ch.className ?? ''}`;

  return {
    get evidence() {
      return {
        background: { width: background.width, height: background.height, rows: background.rows, columns: background.columns },
        characters: list.map((ch) => ({ name: ch.name, className: ch.className, level: ch.level })),
        selected: selected >= 0 ? list[selected].name : null,
        buttons: Object.fromEntries(ORDER.map((id) => [id, { ...buttonBox(id), disabled: Boolean(BUTTONS[id].disabled) }])),
        portraits: portraits.map((p) => p && { wclass: p.wclass, width: p.canvas.width, height: p.canvas.height }), portraitError,
      };
    },
    selectByName(name) { const i = list.findIndex((ch) => ch.name === name); if (i >= 0) selected = i; return i; },
    play() { if (selected >= 0) onPlay?.(list[selected].name); },
    /** Canvas-pixel centres of the SAME boxes click() tests. */
    targets(width, height) {
      const v = view(width, height);
      const centre = (b) => [Math.round(v.ox + (b.x + b.w / 2) * v.scale), Math.round(v.oy + (b.y + b.h / 2) * v.scale)];
      const characters = Object.fromEntries(list.map((ch, i) => [ch.name, centre(cardBox(i))]));
      const play = centre(buttonBox('ok')), create = centre(buttonBox('create')), exit = centre(buttonBox('exit'));
      return { characters, rows: characters, play, create, exit, OK: play, 'CREATE NEW CHARACTER': create, EXIT: exit, 'DELETE CHARACTER': centre(buttonBox('delete')) };
    },
    click(x, y, width, height) {
      const p = toScreen(x, y, width, height);
      if (inBox(p, buttonBox('ok'))) { this.play(); return 'play'; }
      if (inBox(p, buttonBox('create'))) { onCreate?.(); return 'create'; }
      if (inBox(p, buttonBox('exit'))) { onCancel?.(); return 'exit'; }
      for (let i = 0; i < list.length; i++) if (inBox(p, cardBox(i))) { selected = i; return list[i].name; }
      return null;
    },
    draw(ctx, width, height) {
      const g = sceneCtx;
      g.globalAlpha = 1; g.fillStyle = '#000'; g.fillRect(0, 0, SCREEN.width, SCREEN.height);
      g.drawImage(background.canvas, 0, 0);
      for (const p of background.placements) recordDc6(SELECT_ASSETS.background, p.i, p.frame, p.left, p.top);
      // Buttons, then their labels (exocet10, mode 4): a tall button's two lines anchored 28 and 12
      // rows above its bottom, a medium button's one line 8 above (retail's anchors).
      for (const id of ORDER) {
        const b = BUTTONS[id], k = kinds[b.kind], bottom = b.top + k.frame.height - 1;
        if (b.disabled) trans50(g, k.frame, b.x, b.top); else g.drawImage(k.canvas, b.x, b.top);
        recordDc6(k.path, 0, k.frame, b.x, b.top, { op: 'cel', mode: b.disabled ? 1 : 5 });
        const anchors = b.lines.length === 2 ? [bottom - 28, bottom - 12] : [bottom - 8];
        b.lines.forEach((line, n) => labelFont.drawAnchored(g, line, b.x + Math.max(Math.trunc((k.frame.width - labelFont.measure(line)) / 2), 0), anchors[n]));
      }
      if (selected >= 0) {
        const ch = list[selected];
        titleFont.drawAnchored(g, ch.name, Math.trunc(TITLE.centre - titleFont.measure(ch.name) / 2), TITLE.anchor);
      }
      for (let i = 0; i < list.length; i++) {
        const ch = list[i], box = cardBox(i), dx = box.x - CARD.x, dy = box.y - CARD.top;
        [ch.name, describe(ch), 'EXPANSION CHARACTER'].forEach((text, n) => cardFonts[n].drawAnchored(g, text, CARD.textX + dx, CARD.lines[n] + dy));
        const pt = portraits[i];
        if (pt) g.drawImage(pt.canvas, 67 + dx + pt.x, 165 + dy + pt.y);
        if (i === selected) {
          g.drawImage(boxParts[0], box.x, box.y); if (boxParts[1]) g.drawImage(boxParts[1], box.x + boxParts[0].width, box.y);
          recordDc6(SELECT_ASSETS.box, 0, boxDc6.frames[0], box.x, box.y);
        }
      }
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, width, height);
      const v = view(width, height);
      ctx.imageSmoothingEnabled = false;
      ctx.drawImage(scene, v.ox, v.oy, SCREEN.width * v.scale, SCREEN.height * v.scale);
      return { characters: list.length, selected };
    },
  };
}
