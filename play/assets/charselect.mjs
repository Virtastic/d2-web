// Character select, laid out from retail 1.13c's own draw list (the retail captures,
// client-parity C3n; s274 compares the frame): characterselectscreenEXP.dc6 through Sky, the selected
// character's name in Font42 as the title, one card per character (charselectbox.dc6 behind the
// selected one; the name in gold, "Level N Class" in white, "EXPANSION CHARACTER" in green, Font16),
// and retail's buttons -- CREATE NEW CHARACTER, CONVERT TO EXPANSION (retail's draw mode 1, PL2
// AlphaBlend 50%: an expansion character cannot be converted), DELETE CHARACTER (TallButtonBlank), EXIT and OK
// (MediumButtonBlank), their labels in exocet10 through PL2 MultiplicativeBlend (mode 4).
// Portrait identity (client-parity C3g): every saved row resolves its OWN header appearance bytes into
// the layers retail draws (TN hth/1hs, frame 12, direction 0) at its own card position, each layer as its
// own cel with its own DCC path and its own draw-list record -- the COF's priority table gives retail's
// nine files and their order (s276 pairs them against retail's own draw list), and a layer that failed to
// resolve is reported, never dropped in silence.
//
// Everything about which characters exist comes from the store: real .d2s files the engine wrote.

import { createFont, decodeFontTbl, FRONTEND, TITLE_COLUMNS, composeTiled, createBlender, textColourPalette, componentColourTransform } from './frontend.mjs';
import { drawPath, fnv1a, recordDc6, recordDcc } from './drawlist.mjs';
import { CLASS_NAMES } from './save-store.mjs';
import { createDeleteConfirmFlow } from './delete-confirm-flow.mjs';
import { characterCardHitBox, createCharacterSelectCardInput } from './charselect-card-input.mjs';
import { isButtonHeldOverTarget } from './frontend-pointer-states.mjs';
import { decodeCof, decodeDcc } from './unit-formats.mjs';
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
  // DELETE CHARACTER's confirmation. Retail's own layout (OpenDiablo2 d2gamescreen/character_select.go
  // "Adding a client capability" prior art): a half-opacity black over the screen, PopUpOKCancel.dc6
  // on the Fechar palette at (270,175), the confirmation in Font16/Units centred on x=400 above it,
  // and Cancel/OK (CancelButtonBlank, FontRediculous) at (282,308) and (422,308).
  popup: 'data\\global\\ui\\FrontEnd\\PopUpOKCancel.dc6',
  okCancel: 'data\\global\\ui\\FrontEnd\\CancelButtonBlank.dc6',
  popupPalette: 'data\\global\\palette\\fechar\\pal.dat',
};

// Retail's delete-confirm box (OpenDiablo2 character_select.go): PopUpOkCancel at (270,175), the label
// centred at (400,185), the two buttons' TOP-LEFT corners at (282,308) and (422,308).
const DELETE_DIALOG = { box: { x: 270, top: 175 }, label: { x: 400, anchor: 185 }, no: { x: 282, top: 308 }, yes: { x: 422, top: 308 } };
const HALF_BLACK = 'rgba(0, 0, 0, 0.5)';

// Retail's placements (draw list anchors are bottom rows, inclusive; top = anchor - h + 1).
const CARD = { x: 37, top: 86, w: 272, h: 93, colStep: 272, rowStep: 95, textX: 113, lines: [113, 127, 141] };
const TITLE = { anchor: 63, centre: 318 };   // centre: retail's 'Oracle' starts at x 253 (one sample)
const BUTTONS = {
  create: { kind: 'tall', x: 33, top: 469, lines: ['CREATE NEW', 'CHARACTER'] },
  convert: { kind: 'tall', x: 233, top: 469, lines: ['CONVERT TO', 'EXPANSION'] },
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

// Retail 1.13c string-table indices, as d2_lang_string_by_index numbers them (found by value, the way
// charcreate.mjs found "Select Hero Class" = 5127, and read back from retail's own string.tbl through
// the engine): 5163 is the whole delete question ("Are you sure that you want to delete this
// character? Take note: this will delete all versions of this Character."), and 5166/5167 are the
// button labels. These were 0 placeholders, so the dialog could only ever draw its fallback text.
export const DELETE_STRINGS = { confirm: 5163, yes: 5166, no: 5167 };
// The portrait's card-local anchor (retail's draw list: the composite DCC at the card's (67,165)).
// Exported so a row can turn a layer's card-local `left`/`top` (charSelectEvidence.drawnPortraits[].
// layers[]) into the actual scene position: sceneX = layer.left + (portrait.x - PORTRAIT.x).
export const PORTRAIT = { x: 67, anchor: 165 };

/** `characters` is the store's listing. onPlay(name), onCreate(), onCancel(), onDelete(name). */
export function createCharacterSelect({ read, decodeDc6, decodeDatPalette, characters, onPlay, onPlayRefused = null, onCreate, onCancel, onDelete = null, onConvert = null, string = null, componentToken = null, documentObject = globalThis.document }) {
  // Keep the requested path separate from the bytes that were actually decoded.  The latter is
  // carried to every confirmation blit: a constant copied into both sides of an assertion cannot
  // make a substituted DC6 or palette look correct.
  const source = (path) => { const bytes = read(path); return { bytes, identity: { path: drawPath(path), bytesHash: `0x${fnv1a(bytes).toString(16).padStart(8, '0')}` } }; };
  const palette = decodeDatPalette(read(SELECT_ASSETS.palette));
  const pl2 = read(SELECT_ASSETS.pl2);
  const background = composeTiled(decodeDc6(read(SELECT_ASSETS.background)), palette, TITLE_COLUMNS, documentObject);
  const boxDc6 = decodeDc6(read(SELECT_ASSETS.box));
  const boxParts = boxDc6.frames.slice(0, 2).map((f) => frameCanvas(f, palette, documentObject));
  const button = (path, colours = palette) => { const dc6 = decodeDc6(read(path)); return { path, states: dc6.frames.map((frame) => ({ frame, canvas: frameCanvas(frame, colours, documentObject) })) }; };
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
  // The delete confirmation: PopUpOKCancel on the Fechar palette, CancelButtonBlank buttons on Units,
  // their labels in fontridiculous (retail's FontRediculous for ButtonTypeOkCancel).
  const popupPaletteSource = source(SELECT_ASSETS.popupPalette);
  const popupSource = source(SELECT_ASSETS.popup);
  const unitsPaletteSource = source('data\\global\\palette\\Units\\pal.dat');
  const popupPalette = decodeDatPalette(popupPaletteSource.bytes);
  const units = decodeDatPalette(unitsPaletteSource.bytes);
  // The portrait's own colour transform bank (client-parity C3o): retail ships NO
  // data\global\palette\Units\pal.pl2 at all (`mpq ls` against the retail MPQs -- every OTHER
  // palette directory, ACT1-5/menu0-4/fechar/loading/Trademark/EndGame/Sky, carries one; Units and
  // STATIC do not). D2CMP keeps ONE currently-loaded PL2 at a time, and
  // this screen's own is Sky's (SELECT_ASSETS.pl2, `pl2` above) -- the only transform bank this
  // screen ever loads -- so that is what a component's dye remaps a DCC's palette INDEX through,
  // before the (separate) Units palette turns that index into RGB for the portrait's own blit.
  const popupDc6 = decodeDc6(popupSource.bytes);
  const popupParts = popupDc6.frames.slice(0, 2).map((f) => frameCanvas(f, popupPalette, documentObject));
  const okCancelSource = source(SELECT_ASSETS.okCancel);
  const okCancelDc6 = decodeDc6(okCancelSource.bytes);
  const okCancel = { path: SELECT_ASSETS.okCancel, states: okCancelDc6.frames.map((frame) => ({ frame, canvas: frameCanvas(frame, units, documentObject) })) };
  let dialogDraws = [];
  const dialogDraw = (kind, requestedAsset, actualAsset, requestedPalette, actualPalette, record = null) => {
    dialogDraws.push({ kind, requested: { asset: drawPath(requestedAsset), palette: drawPath(requestedPalette) },
      decoded: { asset: actualAsset.identity, palette: actualPalette.identity },
      frame: record?.frame ?? null, x: record?.x ?? null, y: record?.y ?? null });
  };
  // The confirmation label and the OK/Cancel buttons do NOT share a font, and using one for both drew
  // the question in the wrong face. OpenDiablo2: `loadDeleteCharConfirm` builds the label with
  // d2resource.Font16 (`/data/local/FONT/<lang>/font16`, the face this file already carries as
  // FRONTEND.versionFont), while `ButtonTypeOkCancel` -- the YES / NO button -- carries FontRediculous
  // (d2ui/button.go). Wrap is `SplitIntoLinesWithMaxWidth(..., 29)`.
  const confirmFontSource = source(FRONTEND.versionFont);
  const confirmFont = createFont({ dc6: decodeDc6(confirmFontSource.bytes), tbl: decodeFontTbl(read(FRONTEND.versionFontTbl)), palette: units, documentObject, path: FRONTEND.versionFont, drawAs: { op: 'celcolor', mode: 5 },
    onGlyph: (record) => dialogDraw('question-glyph', FRONTEND.versionFont, confirmFontSource, 'data\\global\\palette\\Units\\pal.dat', unitsPaletteSource, record) });
  const confirmText = (() => { const t = string ? string(DELETE_STRINGS.confirm) : ''; return t || 'Delete this character?'; })();
  const yesText = (string && string(DELETE_STRINGS.yes)) || 'YES';
  const noText = (string && string(DELETE_STRINGS.no)) || 'NO';
  // Retail wraps the confirmation to 29 columns. This is OpenDiablo2's `SplitIntoLinesWithMaxWidth`
  // (d2common/d2util/stringutils.go) ported line for line: the running length carries a separator for
  // the first word too, so a line holds at most 28 visible characters -- a `line.length > 29` test
  // breaks later and drew five words where retail breaks after four.
  const wrap = (text, maxChars) => { const lines = []; let line = '', total = 0;
    for (const word of String(text).split(' ')) {
      total += 1 + word.length;
      if (total > maxChars) { total = word.length; lines.push(line); line = ''; } else if (line) line += ' ';
      line += word;
    }
    if (line.length) lines.push(line);
    return lines; };
  const confirmLines = wrap(confirmText, 29);
  // The OK/Cancel button's own face (d2ui/button.go ButtonTypeOkCancel -> FontRediculous), the same
  // one the front end's other small buttons use.
  const okCancelFontSource = source(FRONTEND.smallFont);
  const okCancelFont = createFont({ dc6: decodeDc6(okCancelFontSource.bytes), tbl: decodeFontTbl(read(FRONTEND.smallFontTbl)), palette: units, documentObject, path: FRONTEND.smallFont, drawAs: { op: 'celcolor', mode: 5 },
    onGlyph: (record) => dialogDraw('button-glyph', FRONTEND.smallFont, okCancelFontSource, 'data\\global\\palette\\Units\\pal.dat', unitsPaletteSource, record) });
  const cursorDc6 = decodeDc6(read(FRONTEND.cursor));
  const cursorCanvas = frameCanvas(cursorDc6.frames[0], palette, documentObject);
  const labelFont = createFont({ dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)), palette, documentObject, path: FRONTEND.font, drawAs: { op: 'celcolor', mode: 4, blend } });
  const colourFont = (dc6Path, tblPath, colour) => createFont({ dc6: decodeDc6(read(dc6Path)), tbl: decodeFontTbl(read(tblPath)),
    palette: colour ? textColourPalette(pl2, palette, colour) : palette, documentObject, path: dc6Path, drawAs: { op: 'celcolor', mode: 5 } });
  const titleFont = colourFont(SELECT_ASSETS.title, SELECT_ASSETS.titleTbl, 0);
  const cardFonts = [4, 0, 2].map((c) => colourFont(SELECT_ASSETS.card, SELECT_ASSETS.cardTbl, c));

  // The portrait (retail's draw list: the composite DCC at the card's (67,165), town-neutral 'TN',
  // frame 12, direction 0): each COF layer's component from the save's appearance bytes -- 0xFF the
  // class's plain 'lit', 1-3 lit/med/hvy, an item index through the engine's component table
  // (componentToken: d2_component_token) -- and the right hand's weapon class picking the COF.
  //
  // Retail draws EACH LAYER as its own cel, in the COF's priority order, and its draw list names the
  // nine files it drew (chars\BA\LG\BALGlitTNhth .. chars\BA\RH\BARHhaxTN1hs). One flattened canvas
  // could not be compared draw by draw, and a layer that failed to load would just vanish into it --
  // so each layer keeps its own canvas, its own DCC path and its own record, and a layer that does
  // not load is REPORTED (portraitMissing), never silently dropped.
  const COMP = ['HD', 'TR', 'LG', 'RA', 'LA', 'RH', 'LH', 'SH', 'S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8'];
  const portraitError = [];
  const layerError = (ch, path, error) => { portraitError.push(`${ch.name}: ${path}: ${error}`); };
  // `transform`, when given, is the 256-entry palette-index remap the layer's own saved colour byte
  // selects (componentColourTransform, client-parity C3o): each source DCC palette index is remapped
  // through it BEFORE the Units lookup, exactly as retail's saved dye applies to one equipped layer.
  const layerCanvas = (frame, transform = null) => {
    const c = documentObject.createElement('canvas');
    c.width = Math.max(1, frame.width); c.height = Math.max(1, frame.height);
    if (!frame.width || !frame.height) return c;
    const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
    for (let k = 0; k < frame.indexed.length; k++) if (frame.opaque[k]) {
      const paletteIndex = transform ? transform[frame.indexed[k]] : frame.indexed[k];
      const s = paletteIndex * 4; rgba[k * 4] = units[s]; rgba[k * 4 + 1] = units[s + 1]; rgba[k * 4 + 2] = units[s + 2]; rgba[k * 4 + 3] = 255;
    }
    c.getContext('2d').putImageData(new ImageData(rgba, frame.width, frame.height), 0, 0);
    return c;
  };
  const portrait = (ch) => {
    const empty = { trace: [], drawable: [], missing: [], wclass: null, width: 0, height: 0, identity: null };
    try {
      const token = playerClassInfo(ch.playerClass).token, app = ch.appearance || [], appColours = ch.appearanceColours || [];
      const item = (i) => { const v = app[i]; if (v === undefined || v === 0xFF || v === 0) return null; const t = componentToken?.(v) || ''; const [code, wclass] = t.split(' '); return code ? { code, wclass } : null; };
      const rh = item(5), lh = item(6);
      const wclass = (rh?.wclass || lh?.wclass || 'hth').toUpperCase();
      const cof = decodeCof(read(`data\\global\\CHARS\\${token}\\COF\\${token}TN${wclass}.COF`));
      // Two different direction indices, and conflating them drew retail's nine layers in the wrong
      // order (s276): the COF's priority table is indexed by the COF direction -- retail's record names
      // this one, `dir=0` -- while the DCC's OWN direction blocks are rotated relative to it and are what
      // `dccDirection` maps to (for this portrait, block 4). priority[0][12] is exactly retail's order
      // [LG,TR,LA,RA,S1,S2,HD,SH,RH]; priority[4][12] is a different permutation of the same nine.
      const cofDirection = Math.trunc(((0 % 64) + 64) % 64 * cof.directions / 64);
      const artDirection = dccDirection(0, cof.directions);
      const frameIndex = Math.min(12, cof.framesPerDirection - 1);
      // The COF, not the files that happened to load, defines the composition: every layer record gets a
      // trace, so a DCC that failed to resolve is reported rather than silently dropped and a partial
      // portrait cannot pass for a complete one (s291 asserts on these traces).
      const required = new Set(cof.priority[cofDirection][frameIndex]);
      const trace = [], frames = new Map(), colourTransforms = new Map(), missing = [];
      for (const layer of cof.layerRecords) {
        const comp = COMP[layer.component];
        const v = app[layer.component], it = item(layer.component);
        const base = it ? it.code : (v >= 1 && v <= 3 ? ['lit', 'med', 'hvy'][v - 1] : 'lit');
        const colourByte = appColours[layer.component];
        const transform = componentColourTransform(pl2, colourByte);
        const entry = { component: layer.component, appearance: v ?? null, token: base,
          weaponClass: layer.weaponClass.toUpperCase(), required: required.has(layer.component),
          // C3o: the byte this layer's colour slot carries and whether a transform was applied --
          // 0xFF/undefined draws the DCC's own palette indices unchanged, matching retail.
          colourByte: colourByte ?? null, colourTransformed: Boolean(transform),
          drawn: false, resolved: false, path: null, error: null };
        trace.push(entry);
        if (!comp) { entry.error = `COF component ${layer.component} has no DCC directory`; continue; }
        entry.path = `data\\global\\CHARS\\${token}\\${comp}\\${token}${comp}${base}TN${entry.weaponClass}.DCC`;
        try {
          const dcc = decodeDcc(read(entry.path), { directions: [artDirection] });
          if (artDirection >= dcc.directions || frameIndex >= dcc.framesPerDirection) throw new RangeError(`no direction ${artDirection} frame ${frameIndex}`);
          frames.set(layer.component, dcc.directionRecords[artDirection].frames[frameIndex]);
          if (transform) colourTransforms.set(layer.component, transform);
          entry.resolved = true;
          entry.drawn = entry.required;
        } catch (error) { entry.error = String(error && error.message || error); missing.push(entry.path); layerError(ch, entry.path, entry.error); }
      }
      // The cels to blit, in the COF's priority order -- which is retail's: its draw list names the nine
      // files and their order. Each keeps its own canvas and its own DCC path, so a layer that drew the
      // wrong art is visible draw by draw instead of dissolving into one flattened composite.
      const drawable = [];
      let minX = Infinity, minY = Infinity, maxX = -Infinity, maxY = -Infinity;
      for (const component of cof.priority[cofDirection][frameIndex]) {
        const frame = frames.get(component);
        if (!frame) continue;
        const left = PORTRAIT.x + frame.xOffset;
        const top = PORTRAIT.anchor + (frame.bottomUp ? frame.yOffset : frame.yOffset - frame.height + 1);
        drawable.push({ component, frameIndex, frame, canvas: layerCanvas(frame, colourTransforms.get(component) || null), left, top,
          path: trace.find((l) => l.component === component)?.path });
        // Back onto the trace entry too: the screen anchor (card-relative) a colour-transform row
        // needs to locate this layer's own drawn pixels in the scene, without re-deriving DCC offsets.
        const traced = trace.find((l) => l.component === component);
        if (traced) { traced.left = left; traced.top = top; traced.width = frame.width; traced.height = frame.height; }
        minX = Math.min(minX, left); minY = Math.min(minY, top);
        maxX = Math.max(maxX, left + frame.width); maxY = Math.max(maxY, top + frame.height);
      }
      return { trace, drawable, missing, wclass, width: maxX > minX ? maxX - minX : 0, height: maxY > minY ? maxY - minY : 0,
        identity: { name: ch.name, playerClass: ch.playerClass, classToken: token,
          appearance: Array.from(app), appearanceColours: Array.from(ch.appearanceColours || []),
          wclass, direction: 0, dccDirection: artDirection, frame: frameIndex,
          // The components of the cels that were blitted, in blit order: retail's own priority order.
          drawnOrder: drawable.map((l) => l.component), layers: trace } };
    } catch (error) { portraitError.push(`${ch.name}: ${error && error.message || error}`); return empty; }
  };
  const list = characters.slice(0, 8);
  let portraits = list.map(portrait);
  let drawnPortraits = [];
  let selected = list.length ? 0 : -1;
  // The delete confirmation retail opens over the screen (character_select.go
  // toggleDeleteCharacterDialog): while it is open nothing behind it takes a click.
  const deleteFlow = createDeleteConfirmFlow(onDelete);
  // The pointer in 800x600 screen pixels, retail's own anchor for the ohand cel (D2Win draws it last,
  // at (mouse x, mouse y - 1)); null until the pointer has moved over the canvas.
  let pointer = null, pressed = null, hot = null;
  const cardInput = createCharacterSelectCardInput();
  const scene = documentObject.createElement('canvas'); scene.width = SCREEN.width; scene.height = SCREEN.height;
  const sceneCtx = scene.getContext('2d', { willReadFrequently: true });

  const view = (width, height) => {
    const scale = Math.min(width / SCREEN.width, height / SCREEN.height);
    return { scale, ox: Math.round((width - SCREEN.width * scale) / 2), oy: Math.round((height - SCREEN.height * scale) / 2) };
  };
  const toScreen = (x, y, width, height) => { const v = view(width, height); return { x: (x - v.ox) / v.scale, y: (y - v.oy) / v.scale }; };
  const cardBox = (i) => ({ x: CARD.x + (i % 2) * CARD.colStep, y: CARD.top + (i >> 1) * CARD.rowStep, w: CARD.w, h: CARD.h });
  const buttonBox = (id) => { const b = BUTTONS[id], k = kinds[b.kind].states[0]; return { x: b.x, y: b.top, w: k.frame.width, h: k.frame.height }; };
  // The popup box is drawn as two x-segments of PopUpOKCancel (frame 0 then frame 1), like the card.
  const dialogBox = () => ({ x: DELETE_DIALOG.box.x, y: DELETE_DIALOG.box.top,
    w: popupDc6.frames[0].width + (popupDc6.frames[1]?.width || 0), h: popupDc6.frames[0].height });
  const dialogButtonBox = (id) => ({ x: DELETE_DIALOG[id].x, y: DELETE_DIALOG[id].top, w: okCancel.states[0].frame.width, h: okCancel.states[0].frame.height });
  const inBox = (p, b) => p.x >= b.x && p.x < b.x + b.w && p.y >= b.y && p.y < b.y + b.h;
  const hitControl = (p) => {
    if (deleteFlow.armed >= 0) {
      if (inBox(p, dialogButtonBox('no'))) return 'delete-no';
      if (inBox(p, dialogButtonBox('yes'))) return 'delete-yes';
      return null;
    }
    for (const id of ORDER) if (inBox(p, buttonBox(id))) return `button-${id}`;
    for (let i = 0; i < list.length; i++) if (inBox(p, characterCardHitBox(i))) return `card-${i}`;
    return null;
  };
  // Retail's rule: CONVERT is live only for a classic character (an expansion one cannot be converted); with no character it is dead too.
  const convertDisabled = () => selected < 0 || Boolean(list[selected].expansion);
  const isDisabled = (id) => id === 'convert' ? convertDisabled() :
    (id === 'ok' || id === 'delete') ? selected < 0 : Boolean(BUTTONS[id].disabled);
  const describe = (ch) => `Level ${ch.level} ${CLASS_NAMES[ch.playerClass] ?? ch.className ?? ''}`;

  return {
    get evidence() {
      return {
        background: { width: background.width, height: background.height, rows: background.rows, columns: background.columns },
        characters: list.map((ch) => ({ name: ch.name, className: ch.className, level: ch.level, expansion: Boolean(ch.expansion) })),
        selected: selected >= 0 ? list[selected].name : null,
        pointerState: { pressed },
        buttons: Object.fromEntries(ORDER.map((id) => [id, { ...buttonBox(id), disabled: isDisabled(id) }])),
        // The portrait per character, as the identity the DRAWS carry: the save's own appearance bytes
        // (and colour bytes), the class token, the COF/COF-direction/DCC-direction/frame asked for, and
        // one trace per COF layer -- its own DCC path, whether the COF requires it, and whether it
        // resolved. `drawn` names the cels actually blitted, in the order they were blitted (retail's
        // priority order); `missing` is a layer the COF names that could not be loaded.
        portraits: portraits.map((p, i) => p && { ...p.identity, width: p.width, height: p.height, row: i,
          drawn: p.drawable.map((l) => l.path), missing: p.missing }),
        drawnPortraits: drawnPortraits.map((p) => ({ ...p, layers: p.layers.map((layer) => ({ ...layer })) })),
        portraitError,
        deleteDialog: deleteFlow.armed >= 0 ? { character: list[deleteFlow.armed].name, box: dialogBox(), lines: confirmLines,
          yes: { ...dialogButtonBox('yes'), label: yesText }, no: { ...dialogButtonBox('no'), label: noText }, text: confirmText,
          // The two faces the confirmation blits with -- the label's (frontend.mjs FRONTEND.versionFont,
          // OpenD2's Font16) and the buttons' (FRONTEND.smallFont, ButtonTypeOkCancel's FontRediculous)
          // -- so a row can decode the records back to characters and compare them with `lines`.
          labelFontPath: FRONTEND.versionFont, buttonFontPath: FRONTEND.smallFont,
          // The anchor row each confirmation line is drawn on. The card's own labels are Font16 too
          // (SELECT_ASSETS.card), so a row decoding glyphs by face alone would read the card text as
          // part of the question; these name the dialog's own rows and nothing else.
          labelAnchors: confirmLines.map((_, n) => DELETE_DIALOG.label.anchor + n * confirmFont.lineHeight),
          state: deleteFlow.state, error: deleteFlow.error, draws: dialogDraws.map((draw) => JSON.parse(JSON.stringify(draw))) } : null,
        pointer,
      };
    },
    selectByName(name) { const i = list.findIndex((ch) => ch.name === name); if (i >= 0) selected = i; return i; },
    play() {
      if (deleteFlow.blocked || selected < 0) return false;
      const character = list[selected];
      // D2Launch's selected-character OK callback refuses a save carrying both HARDCORE and DEAD
      // before it enters game creation (FUN_6FA4DEC0, 1.13c). Preserve that front-door result here.
      if (character.hardcore && character.dead) {
        onPlayRefused?.({ name: character.name, stringId: 0x14B8 });
        return false;
      }
      onPlay?.(character.name);
      return true;
    },
    /** The pointer over this screen, in 800x600 pixels (retail's ohand anchor). */
    hover(x, y, width, height) { const v = view(width, height), p = { x: (x - v.ox) / v.scale, y: (y - v.oy) / v.scale };
      pointer = [Math.trunc(p.x), Math.trunc(p.y)]; hot = hitControl(p); },
    /** The store's listing changed under us (a character was deleted): rebuild the portraits. */
    setCharacters(next) { list.length = 0; list.push(...next.slice(0, 8)); portraits = list.map(portrait);
      selected = list.length ? Math.max(0, Math.min(selected, list.length - 1)) : -1; deleteFlow.cancel(); },
    /** Canvas-pixel centres of the SAME boxes click() tests. */
    targets(width, height) {
      const v = view(width, height);
      const centre = (b) => [Math.round(v.ox + (b.x + b.w / 2) * v.scale), Math.round(v.oy + (b.y + b.h / 2) * v.scale)];
      const characters = Object.fromEntries(list.map((ch, i) => [ch.name, centre(characterCardHitBox(i))]));
      const play = centre(buttonBox('ok')), create = centre(buttonBox('create')), exit = centre(buttonBox('exit'));
      // The confirmation's own targets: they exist only while it is open (retail's dialog is modal).
      const dialog = deleteFlow.armed >= 0 ? { DELETE_YES: centre(dialogButtonBox('yes')), DELETE_NO: centre(dialogButtonBox('no')) } : {};
      return { characters, rows: characters, play, create, exit, OK: play, 'CREATE NEW CHARACTER': create, EXIT: exit,
        'DELETE CHARACTER': centre(buttonBox('delete')), 'CONVERT TO EXPANSION': centre(buttonBox('convert')), ...dialog };
    },
    click(x, y, width, height) {
      const p = toScreen(x, y, width, height);
      if (deleteFlow.armed >= 0) {
        if (inBox(p, dialogButtonBox('no'))) { deleteFlow.cancel(); return 'delete-cancel'; }
        if (inBox(p, dialogButtonBox('yes'))) { void deleteFlow.confirm(list[deleteFlow.armed].name); return 'delete-confirm'; }
        return null;   // modal: nothing under the confirmation takes a click
      }
      if (deleteFlow.blocked) return null;
      if (inBox(p, buttonBox('ok'))) { this.play(); return 'play'; }
      if (inBox(p, buttonBox('create'))) { onCreate?.(); return 'create'; }
      if (inBox(p, buttonBox('exit'))) { onCancel?.(); return 'exit'; }
      if (inBox(p, buttonBox('convert'))) { if (convertDisabled()) return null; void onConvert?.(list[selected].name); return 'convert'; }
      if (inBox(p, buttonBox('delete'))) { if (selected >= 0 && deleteFlow.arm(selected)) return 'delete-arm'; return null; }
      for (let i = 0; i < list.length; i++) if (inBox(p, cardBox(i))) { selected = i; return list[i].name; }
      return null;
    },
    press(x, y, width, height) {
      const p = toScreen(x, y, width, height);
      pressed = hitControl(p); hot = pressed;
      if (pressed?.startsWith('card-') && !deleteFlow.blocked) {
        const index = Number(pressed.slice('card-'.length));
        const wasSelected = selected;
        selected = index;
        if (cardInput.down(index, wasSelected, globalThis.performance?.now?.() ?? Date.now()) && selected >= 0) {
          this.play();
        }
      }
      return pressed;
    },
    release(token, x, y, width, height) {
      const p = toScreen(x, y, width, height), now = hitControl(p); pressed = null; hot = now;
      // D2Launch's type-4 card callback runs on WM_LBUTTONDOWN and has no mouse-up callback.
      if (token?.startsWith('card-')) return null;
      // The release point decides: a press on YES released anywhere else cancels the dialog
      // instead of deleting the character the player was dragging away from.
      return now === token ? this.click(x, y, width, height) : null;
    },
    /** A cancelled gesture drops the down cel without activating the control. */
    cancel() { pressed = null; },
    draw(ctx, width, height) {
      const g = sceneCtx;
      drawnPortraits = [];
      g.globalAlpha = 1; g.fillStyle = '#000'; g.fillRect(0, 0, SCREEN.width, SCREEN.height);
      g.drawImage(background.canvas, 0, 0);
      for (const p of background.placements) recordDc6(SELECT_ASSETS.background, p.i, p.frame, p.left, p.top);
      // Buttons, then their labels (exocet10, mode 4): a tall button's two lines anchored 28 and 12
      // rows above its bottom, a medium button's one line 8 above (retail's anchors).
      //
      // A tall button's label lines are emitted LAST LINE FIRST. Retail's own draw list says so for all
      // three tall buttons -- CREATE emits "CHARACTER" (bottom 516) before "CREATE NEW" (bottom 500),
      // CONVERT emits "EXPANSION" before "CONVERT TO", DELETE emits "CHARACTER" before "DELETE" -- and
      // the order is part of the record, not an artefact of the anchors: the anchor each line lands on is
      // retail's either way, only the call order moves. Ours is a comparison of call order, so it counts.
      for (const id of ORDER) {
        const b = { ...BUTTONS[id], disabled: isDisabled(id) }, token = `button-${id}`, state = isButtonHeldOverTarget(pressed, hot) && pressed === token ? 1 : 0, k = kinds[b.kind].states[state] || kinds[b.kind].states[0], bottom = b.top + k.frame.height - 1;
        if (b.disabled) trans50(g, k.frame, b.x, b.top); else g.drawImage(k.canvas, b.x, b.top);
        recordDc6(kinds[b.kind].path, state < kinds[b.kind].states.length ? state : 0, k.frame, b.x, b.top, { op: 'cel', mode: b.disabled ? 1 : 5 });
        // Retail's pressedButtonOffset: a held button's label moves 2 px left and 2 px down with its
        // own cel.
        const lx = state ? -2 : 0, ly = state ? 2 : 0;
        const anchors = b.lines.length === 2 ? [bottom - 28 + ly, bottom - 12 + ly] : [bottom - 8 + ly];
        for (let n = b.lines.length - 1; n >= 0; n--) {
          const line = b.lines[n];
          labelFont.drawAnchored(g, line, b.x + lx + Math.max(Math.trunc((k.frame.width - labelFont.measure(line)) / 2), 0), anchors[n]);
        }
      }
      if (selected >= 0) {
        const ch = list[selected];
        titleFont.drawAnchored(g, ch.name, Math.trunc(TITLE.centre - titleFont.measure(ch.name) / 2), TITLE.anchor);
      }
      for (let i = 0; i < list.length; i++) {
        const ch = list[i], box = cardBox(i), dx = box.x - CARD.x, dy = box.y - CARD.top;
        [ch.name, describe(ch), ...(ch.expansion ? ['EXPANSION CHARACTER'] : [])].forEach((text, n) => cardFonts[n].drawAnchored(g, text, CARD.textX + dx, CARD.lines[n] + dy));
        // The selected card's box, then its portrait on top of it: retail's call order is the three card
        // labels, charselectbox frame 0 and 1, then the nine portrait layers (the retail captures
        // select-oracle.txt records 84..139 that way). Two cels, not one -- frame 0 is 256 wide at x=37,
        // frame 1 is the 16-wide right edge at x=293 -- and BOTH are recorded, or the draw list reports a
        // card edge retail draws and we do not.
        if (i === selected) {
          g.drawImage(boxParts[0], box.x, box.y); if (boxParts[1]) g.drawImage(boxParts[1], box.x + boxParts[0].width, box.y);
          recordDc6(SELECT_ASSETS.box, 0, boxDc6.frames[0], box.x, box.y);
          if (boxParts[1]) recordDc6(SELECT_ASSETS.box, 1, boxDc6.frames[1], box.x + boxParts[0].width, box.y);
        }
        const pt = portraits[i];
        if (pt) {
          // One cel per layer, in the COF's priority order, each recorded at retail's own anchor for the
          // portrait so the draw list names the same nine files, in the same order, as retail's does.
          for (const l of pt.drawable) { g.drawImage(l.canvas, l.left + dx, l.top + dy); recordDcc(l.path, l.frameIndex, PORTRAIT.x + dx, PORTRAIT.anchor + dy); }
          drawnPortraits.push({ ...pt.identity, row: i, x: PORTRAIT.x + dx, y: PORTRAIT.anchor + dy, width: pt.width, height: pt.height });
        }
      }
      if (deleteFlow.armed >= 0) {
        dialogDraws = [];
        g.fillStyle = HALF_BLACK; g.fillRect(0, 0, SCREEN.width, SCREEN.height);
        const db = DELETE_DIALOG.box;
        g.drawImage(popupParts[0], db.x, db.top); if (popupParts[1]) g.drawImage(popupParts[1], db.x + popupParts[0].width, db.top);
        dialogDraw('popup', SELECT_ASSETS.popup, popupSource, SELECT_ASSETS.popupPalette, popupPaletteSource, recordDc6(SELECT_ASSETS.popup, 0, popupDc6.frames[0], db.x, db.top));
        // Retail's draw-list hook records PopUpOKCancel as its composed cel.  Keep its second canvas
        // segment out of that retail-facing list, but retain it in the independent source evidence.
        if (popupParts[1]) dialogDraw('popup', SELECT_ASSETS.popup, popupSource, SELECT_ASSETS.popupPalette, popupPaletteSource);
        confirmLines.forEach((line, n) => confirmFont.drawAnchored(g, line, Math.trunc(DELETE_DIALOG.label.x - confirmFont.measure(line) / 2), DELETE_DIALOG.label.anchor + n * confirmFont.lineHeight));
        for (const [id, text] of [['no', noText], ['yes', yesText]]) {
          const b = dialogButtonBox(id);
          const token = `delete-${id}`, state = isButtonHeldOverTarget(pressed, hot) && pressed === token ? 1 : 0, face = okCancel.states[state] || okCancel.states[0];
          g.drawImage(face.canvas, b.x, b.y);
          dialogDraw('button', SELECT_ASSETS.okCancel, okCancelSource, 'data\\global\\palette\\Units\\pal.dat', unitsPaletteSource, recordDc6(okCancel.path, state < okCancel.states.length ? state : 0, face.frame, b.x, b.y));
          okCancelFont.drawAnchored(g, text, b.x + Math.trunc((b.w - okCancelFont.measure(text)) / 2), b.y + Math.trunc((b.h + okCancelFont.cellHeight) / 2) - 1);        }
      }
      // Retail's pointer last, an ohand cel at (mouse x, mouse y - 1) with the DC6's own offsets
      // (D2MOO D2WinMain.cpp; the title draws it the same way, and the retail captures
      // records it at x=799 y=598 for the capture's pointer at (799,599)).
      let cursor = 0;
      if (pointer) {
        const f = cursorDc6.frames[0];
        const left = pointer[0] + f.offsetX, top = pointer[1] - 1 + f.offsetY - f.height + 1;
        g.drawImage(cursorCanvas, left, top); recordDc6(FRONTEND.cursor, 0, f, left, top); cursor = 1;
      }
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, width, height);
      const v = view(width, height);
      ctx.imageSmoothingEnabled = false;
      ctx.drawImage(scene, v.ox, v.oy, SCREEN.width * v.scale, SCREEN.height * v.scale);
      return { characters: list.length, selected, cursor };
    },
  };
}
