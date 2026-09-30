// Character creation: the screen between SINGLE PLAYER and the game.
//
// Retail puts the seven classes on a dark hall background, each standing at a fixed spot with its
// own idle animation, and walking forward when you pick it. Hovering a class plays its "selected"
// idle; clicking commits it, and a name of at least two characters enables OK.
//
// Prior art used: OpenDiablo2
// `d2game/d2gamescreen/select_hero_class.go` for the per-class anchor positions, hit rectangles and
// animation play lengths, and `d2common/d2resource/resource_paths.go` for the sprite paths. Those
// are retail layout numbers the community already recovered; re-deriving them by eye would be both
// slower and worse. Everything drawn here is retail art.
//
// Gotcha worth keeping: this screen does NOT use the menu palette. The class sprites and the
// background are indexed into `palette\fechar\pal.dat` ("front end character"), while the labels
// use `palette\Units\pal.dat` -- both per OpenDiablo2's loadSprite/NewLabel calls. Painting the
// background with the Units palette does not fail, it just comes out blown out in blue and white,
// which is a wrong picture rather than an error.

import { composeTiled, createBlender, createFont, decodeFontTbl, FRONTEND, recordTiled, textColourPalette, TITLE_COLUMNS } from './frontend.mjs';
import { createCharacterCreateCheckboxes } from './charcreate-checkboxes.mjs';
import { createCharacterNameRenderer, NAME_FIELD } from './charcreate-name.mjs';
import { recordDc6 } from './drawlist.mjs';
import { createHeroStances, HERO_STANCE } from './charcreate-stances.mjs';

/**
 * Per-class layout, from OpenDiablo2's select_hero_class.go.
 *
 * `anchor` is where the sprite is pinned; `hit` is the clickable rectangle as {x, y, w, h} (the
 * source names it a Rectangle with a Max field but stores width/height there, which is why the
 * numbers look wrong as a Max). `forwardMs` is how long the forward-walk animation runs.
 *
 * `idleEvery` and `selectedEvery` are TICKS of the front end's 25 Hz clock per DC6 frame of the idle
 * (NU1/NU2) loop and of the chosen idle (NU3) loop. They are separate numbers because retail's are
 * separate, MEASURED from retail's own consecutive-frame captures of this screen on 2026-09-27
 * (`the retail captures`, and the idle classes drawn in the same frames):
 * every non-chosen class's NU1 steps once per `idleEvery` frames there (2 for barbarian, sorceress,
 * paladin, druid; 3 for amazon, assassin, necromancer -- which is where `idleEvery` came from), and
 * the CHOSEN class's NU3 steps faster: 1 for the barbarian and the sorceress, 2 for the amazon,
 * assassin, necromancer, paladin and druid. Driving the chosen art from `idleEvery` played it 1.5x to
 * 2x too slowly (the necromancer's ghost-hands loop ran 1320 ms against retail's 880). `druid`'s own
 * NU3 capture is irregular (run lengths 1, 2 and 3 over one loop), so its 2 is the dominant run
 * length rather than a clean measurement, and the comments carries that caveat.
 *
 * `playerClass` is OUR engine's class index, which is the order the session's class dropdown uses:
 * 0 Amazon, 1 Sorceress, 2 Necromancer, 3 Paladin, 4 Barbarian, 5 Druid, 6 Assassin.
 */
export const CLASSES = [
  { key: 'amazon',      label: 'Amazon',      descriptionIndex: 5128,  playerClass: 0, dir: 'amazon',      code: 'AM', anchor: { x: 100, y: 337 }, hit: { x:  70, y: 220, w: 55, h: 200 }, idleEvery: 3, idleMs: 2500, forwardMs: 2200, backMs: 1500, selectedEvery: 2 },
  { key: 'assassin',    label: 'Assassin',    descriptionIndex: 22519, playerClass: 6, dir: 'assassin',    code: 'AS', anchor: { x: 232, y: 364 }, hit: { x: 175, y: 235, w: 50, h: 180 }, idleEvery: 3, idleMs: 2500, forwardMs: 3800, backMs: 1500, selectedEvery: 2 },
  { key: 'necromancer', label: 'Necromancer', descriptionIndex: 5129,  playerClass: 2, dir: 'necromancer', code: 'NE', anchor: { x: 301, y: 333 }, hit: { x: 265, y: 220, w: 55, h: 175 }, idleEvery: 3, idleMs: 1200, forwardMs: 2000, backMs: 1500, selectedEvery: 2 },
  { key: 'barbarian',   label: 'Barbarian',   descriptionIndex: 5130,  playerClass: 4, dir: 'barbarian',   code: 'BA', anchor: { x: 400, y: 330 }, hit: { x: 364, y: 201, w: 90, h: 170 }, idleEvery: 2, idleMs:    0, forwardMs: 2500, backMs: 1000, selectedEvery: 1 },
  { key: 'paladin',     label: 'Paladin',     descriptionIndex: 5132,  playerClass: 3, dir: 'paladin',     code: 'PA', anchor: { x: 521, y: 339 }, hit: { x: 490, y: 210, w: 65, h: 180 }, idleEvery: 2, idleMs: 2500, forwardMs: 3400, backMs: 1300, selectedEvery: 2 },
  { key: 'sorceress',   label: 'Sorceress',   descriptionIndex: 5131,  playerClass: 1, dir: 'sorceress',   code: 'SO', anchor: { x: 626, y: 353 }, hit: { x: 580, y: 240, w: 65, h: 160 }, idleEvery: 2, idleMs: 2500, forwardMs: 2300, backMs: 1200, selectedEvery: 1 },
  { key: 'druid',       label: 'Druid',       descriptionIndex: 22518, playerClass: 5, dir: 'druid',       code: 'DZ', anchor: { x: 720, y: 370 }, hit: { x: 680, y: 220, w: 70, h: 195 }, idleEvery: 2, idleMs: 1500, forwardMs: 4800, backMs: 1500, selectedEvery: 2 },
];

/**
 * Retail 1.13c draws a chosen hero as TWO cels, not one: the base sprite plus an OVERLAY sprite at the
 * same anchor. The Sorceress's lightning and the Necromancer's ghost hands ARE that overlay -- which is
 * why they were missing while every row stayed green: this screen drew one DC6 per state and the
 * overlay files were never opened.
 *
 * Measured, not guessed. A `create` draw list's cel-load list (tools/drawlist.py over the capture's
 * `cels.bin`) names every front-end hero file 1.13c loads for this screen; the s-suffixed ones are
 * exactly these eight, and no others -- Amazon's AMFWs.DC6 exists in the 1.10f table OpenDiablo2 carries
 * but this build never asks for it, and Amazon, Assassin and Druid load no overlay at all:
 *   SOFWs SONU3s SOBWs   NEFWs NENU3s NEBWs   BAFWs   PAFWs
 *
 * `blend` is retail's own load flag (OpenDiablo2 select_hero_class.go `loadSprite(..., blend)` ->
 * DrawEffectModulate): the selected overlay is loaded blended for every class that has one, the forward
 * overlay per class, and a class with no overlay keeps drawing exactly what it drew before.
 */
export const SELECT_OVERLAYS = {
  amazon: {},
  assassin: {},
  necromancer: { forward: 'FWS', selected: 'NU3s', back: 'BWs', forwardBlend: true, backBlend: true },
  barbarian: { forward: 'FWS', forwardBlend: false },
  paladin: { forward: 'FWS', forwardBlend: false },
  sorceress: { forward: 'FWS', selected: 'NU3s', back: 'BWs', forwardBlend: true, backBlend: true },
  druid: {},
};
// Retail's overlay draw mode: `blend` in that load flag is the same table the campfire's cel uses,
// which this screen already draws at mode 3 (see CREATE_ASSETS.campfire).
const OVERLAY_MODE = 3;

// Label and widget positions, same source.
export const LAYOUT = {
  // Measured against retail's frame (s176): the heading lands 1 px left and 3 px lower than
  // OpenDiablo2's (400, 17), the buttons 1 px lower. The class name and OK button take the same
  // corrections (same font / same button art; not yet in a retail frame of their own).
  heading: { x: 399, y: 20 },
  // Retail's draw list with the Barbarian chosen (the retail captures,
  // create-name-selected.txt): the class NAME's Font30 cels have their bottom row at y=99, i.e. their
  // top at 70 -- `draw` anchors a glyph by its TOP, so this number IS a top and 68 sat 2 px high. The
  // same table's heading (top 20, from the Font30 cels' y=49) is exact, so the difference was this
  // one number, not the font or the anchor rule.
  className: { x: 399, y: 70 },
  // OpenDiablo2's recovered SelectHeroClass labels: three centred Font16 lines. Retail's own list puts
  // their bottoms at 120, 134 and (when this language's text needs the third) 148 -- tops 105, 119,
  // 133, 14 px apart, not 15. `draw` anchors by the top.
  classDescription: [{ x: 400, y: 105 }, { x: 400, y: 119 }, { x: 400, y: 133 }],
  // Retail's draw list with a name typed (scene `create-named`): the Font16 label "CHARACTER NAME"
  // is a celcolor at first-glyph x=321, bottom anchor y=490 -> top 475, and textbox.dc6 frame 0 is a
  // cel at x=319 y=519 h=26 -> top-left (319, 494). Both were 1 px right/low of the old layout.
  nameLabel: { x: 321, y: 475 },
  nameBox: { x: 319, y: 494 },
  // The typed name itself: FontFormal12, first glyph at pen x=326 with its BOTTOM row at 514 (same
  // draw list: 7 celcolor blits, x=326, 336, 344, 355, 364, 369, 379, all y=514, w=35 h=30). Nothing
  // here came from retail before: the name was drawn in Font30 at nameBox.x + 8, centred in the box,
  // which is why it was both too large and sitting outside the field.
  nameField: NAME_FIELD,
  exitButton: { x: 33, y: 538 },
  // Retail's draw list puts BOTH buttons' cels at y=572 (bottom row), h=35 -> top 538, and the OK
  // cel's x at 627, not OpenDiablo2's 630: retail's own frame has the button's first lit pixel at
  // 628 in every capture that shows it, so the cel starts at 627. EXIT was already right at 33.
  okButton: { x: 627, y: 538 },
};

// OpenDiablo2 uses Font30 for the heading and class name and Font16 for the smaller labels; using
// Font30 everywhere makes "CHARACTER NAME" collide with the name box directly beneath it.
export const FONT16 = {
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTbl: 'data\\local\\font\\latin\\font16.tbl',
};

// FontFormal12, the same face the trademark screen's copyright lines use. It is the face the
// character-creation NAME FIELD types in (retail's own draw list, above) -- one screen, three fonts:
// Font30 for the heading and the class name, Font16 for the labels, FontFormal12 for the name.
// The name field's CARET. Retail's own multi-frame draw list of the typed name
// (the retail captures, 30 consecutive whole frames of the Barbarian chosen
// with a four-letter name typed) draws one extra FontFormal12 cel after the last glyph: frame=95 at x=366, bottom
// 514, the same 35x30 cell. Frame 95 is the `_` glyph -- in these fonts the DC6 frame index IS the
// character's own code (the name typed is frames 77, 105, 107, 101).
//
// It blinks, and the capture fixes the rate: the cel is drawn in whole frames 0-1, 8-14 and 21-26 and
// absent in 2-7, 15-20 and 27-29 -- transitions 6.25 frames apart. A whole frame is one 25 Hz
// front-end tick (the same capture's fire.dc6 advances one frame per whole frame, and the hovered
// barbarian's banu2 one frame per two), so the caret is 250 ms lit and 250 ms dark.
export const CREATE_ASSETS = {
  background: 'data\\global\\ui\\FrontEnd\\charactercreationscreenEXP.dc6',
  // Retail's draw list draws the NAME FIELD from textbox.dc6, not TextBox2.dc6 (both are in the
  // MPQs, and the page used TextBox2); the retail captures, and three separate
  // captures agree. Its own cel record is at x=319 y=519 h=26.
  textBox: 'data\\global\\ui\\FrontEnd\\textbox.dc6',
  // Retail's draw list (scene `create`): EXIT is MediumSelButtonBlank frame 0, anchored (33, 572).
  button: 'data\\global\\ui\\FrontEnd\\MediumSelButtonBlank.dc6',
  // The screen's own palette; the fonts keep the menu palette (see the header comment).
  palette: 'data\\global\\palette\\fechar\\pal.dat',
  // OpenDiablo2 select_hero_class.go: the campfire the heroes stand around, at (380, 335).
  campfire: 'data\\global\\ui\\FrontEnd\\fire.DC6',
};
// Retail's draw list (the retail capture tool/drawlist-linux.sh scene `create`): every hero's NU1
// cel at the anchors in CLASSES (bottom row inclusive), drawn AFTER the heading and the buttons, in
// this order; then fire.dc6 at (379, 338) in draw mode 3 -- the act palette's PL2 additive table
// (4,558 of 4,593 fire pixels of retail's own 8-bit frame are AdditiveBlend[under][fire]).
// Retail's clock (60 recorded frames of `createlong`, 480 of 480 frame numbers fitted by ONE clock):
// the front end runs at 25 Hz; each idle loop shows frame floor(t / idleEvery) mod (frames - 1) --
// every 2nd tick for barbarian, sorceress, paladin, druid, every 3rd for the others -- and the fire
// frame t mod (frames - 1). A DC6's last frame is never shown in the loop.
const RETAIL_HZ = 25, UPDATE_HZ = 60;
const DRAW_ORDER = ['barbarian', 'sorceress', 'paladin', 'necromancer', 'assassin', 'amazon', 'druid'];
const CAMPFIRE = { x: 379, y: 338 };
// Retail string-table indices as d2_lang_string_by_index numbers them (found by value; OpenDiablo2's
// numeric_labels.go 1697/1694 count within its own table order): "Select Hero Class", "Character Name".
const STR_SELECT_HERO_CLASS = 5127, STR_CHARACTER_NAME = 5125;
const DESCRIPTION_LINE_WIDTH = 37;

// Retail's character-create control breaks a description on whole words into three 37-character
// labels. This is the same recovery OpenDiablo2 uses for SelectHeroClass; the words themselves
// always come from the retail .tbl through `string`, never from a JS copy.
function descriptionLines(text) {
  const lines = [];
  let line = '';
  for (const word of String(text).split(' ')) {
    if (!word) continue;
    if (line && line.length + 1 + word.length > DESCRIPTION_LINE_WIDTH) {
      lines.push(line);
      line = word;
    } else line += `${line ? ' ' : ''}${word}`;
  }
  if (line) lines.push(line);
  // Retail owns three label slots even when this language's text only needs two; the unused
  // slot is an empty Font16 label rather than omitted layout.
  while (lines.length < 3) lines.push('');
  return lines.slice(0, 3);
}

const SCREEN = { width: 800, height: 600 };

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


/** A lazily-decoded DC6 animation: frames painted on demand, drawn at their own offsets. */
function animation(read, decodeDc6, path, palette, documentObject) {
  const dc6 = decodeDc6(read(path));
  const canvases = new Array(dc6.frames.length).fill(null);
  return {
    path, count: dc6.frames.length,
    /** Draw frame `index`; returns the frame index it actually drew, so a caller can record the
     * drawn identity rather than the requested one (the two differ whenever `index` wraps). */
    draw(ctx, index, anchorX, anchorY) {
      const i = ((index % dc6.frames.length) + dc6.frames.length) % dc6.frames.length;
      const frame = dc6.frames[i];
      if (!frame || !frame.width || !frame.height) return i;
      const canvas = canvases[i] ??= frameCanvas(frame, palette, documentObject);
      // A DC6 frame's offset is its BOTTOM-left relative to the anchor (the retail frame puts every
      // hero ~160px higher than top-left anchoring did).
      const left = Math.round(anchorX + frame.offsetX), top = Math.round(anchorY + frame.offsetY - frame.height + 1);   // bottom row inclusive (drawlist.mjs)
      ctx.drawImage(canvas, left, top);
      // Record it the way retail's hook records its own cel draw: same file, same frame, same anchor,
      // same mode. Without this the screen's sprites were invisible to a draw-list row, which is how
      // a missing overlay layer stayed green.
      recordDc6(path, i, frame, left, top);
      return i;
    },
    /** Draw frame `index` through a blended draw mode (frontend.mjs createBlender). */
    drawBlended(ctx, index, anchorX, anchorY, blend, mode) {
      const i = ((index % dc6.frames.length) + dc6.frames.length) % dc6.frames.length;
      const frame = dc6.frames[i];
      if (!frame) return;
      const left = Math.round(anchorX + frame.offsetX), top = Math.round(anchorY + frame.offsetY - frame.height + 1);
      blend(ctx, frame, left, top, mode);
      recordDc6(path, i, frame, left, top, { mode });
    },
  };
}

/** The overlay cels a class's two drawn states carry, named from its own code (SO -> SOFWs, SONU3s). */
function selectOverlayAnimations(overlay, cls, base, read, decodeDc6, palette, documentObject) {
  const out = {};
  if (overlay?.forward) out.forwardOverlay = animation(read, decodeDc6, `${base}${cls.dir}\\${cls.code}${overlay.forward}.DC6`, palette, documentObject);
  if (overlay?.selected) out.idleSelectedOverlay = animation(read, decodeDc6, `${base}${cls.dir}\\${cls.code}${overlay.selected}.DC6`, palette, documentObject);
  if (overlay?.back) out.backOverlay = animation(read, decodeDc6, `${base}${cls.dir}\\${cls.code}${overlay.back}.DC6`, palette, documentObject);
  return out;
}

/**
 * Build the character-creation screen.
 *
 * `onCreate({ playerClass, name, key, checkboxes })` is called when OK is pressed with a class picked
 * and a name of at least two characters -- retail's own rule. `checkboxes` carries the two boxes'
 * states (`{ expansion, hardcore }`), which are retail's client save flags and belong to the engine:
 * the screen reports them, it does not store them anywhere else.
 * `onSound({ key, trigger })` lets the shell play the
 * screen's retail WAV without teaching this visual component how an AudioContext works.
 */
export function createCharacterCreate({ read, decodeDc6, decodeDatPalette, onCreate, onCancel, onSound, string = () => '', documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(CREATE_ASSETS.palette));
  const fontPalette = decodeDatPalette(read(FRONTEND.palette));
  const background = composeTiled(decodeDc6(read(CREATE_ASSETS.background)), palette, TITLE_COLUMNS, documentObject);
  const font = createFont({
    dc6: decodeDc6(read(FRONTEND.headingFont)),
    tbl: decodeFontTbl(read(FRONTEND.headingFontTbl)),
    palette: fontPalette, documentObject, path: FRONTEND.headingFont,
    // The heading and the class name are both Font30 cels, and retail's own draw list carries every
    // one of them -- so this font records its blits like the other three, and the class name's are
    // kept beside the blit that made them (s640 compares them with retail's records per glyph).
    onGlyph: (blit) => { if (classNameGlyphs) classNameGlyphs.push(blit); },
  });
  let descriptionGlyphs = null, classNameGlyphs = null;
  // The name field's own font, recorded like the description's so a row can assert the FONT, the
  // glyph and the position that were really blitted rather than the text that was requested.
  const nameRenderer = createCharacterNameRenderer({ read, decodeDc6, decodeFontTbl, createFont,
    palette: fontPalette, documentObject });
  const smallFont = createFont({
    dc6: decodeDc6(read(FONT16.font)),
    tbl: decodeFontTbl(read(FONT16.fontTbl)),
    palette: fontPalette, documentObject, path: FONT16.font,
    // The scenario needs the renderer's output, not a second description-to-glyph calculation:
    // preserve each real Font16 cel call only while the selected hero's three labels are painted.
    onGlyph: (blit) => { if (descriptionGlyphs) descriptionGlyphs.push(blit); },
  });
  // The three small labels (CHARACTER NAME and the two checkbox labels) are Font16 celcolor draws with
  // extra=4 in retail's own list: the D2Win gold text shift, the same
  // one the trademark screen's copyright uses. The description lines are extra=0, so they keep smallFont.
  // Each glyph blit is kept so a row can assert font, frame, anchor and extra per glyph.
  const labelGlyphs = [];
  const goldLabelFont = createFont({
    dc6: decodeDc6(read(FONT16.font)), tbl: decodeFontTbl(read(FONT16.fontTbl)),
    palette: textColourPalette(read(CREATE_ASSETS.palette.replace(/pal\.dat$/i, 'pal.pl2')), palette, 4),
    documentObject, path: FONT16.font, drawAs: { op: 'celcolor', mode: 5, extra: 4 },
    onGlyph: (blit) => labelGlyphs.push(blit),
  });
  // OpenDiablo2's SelectHeroClass gives clickbox.dc6's fechar cels and these anchors.  The
  // labels use this screen's existing Units Font16, exactly as its other small labels do.
  const checkboxes = createCharacterCreateCheckboxes({ read, decodeDc6, palette, smallFont: goldLabelFont, string, documentObject });

  const blend = createBlender(read(CREATE_ASSETS.palette.replace(/pal\.dat$/i, 'pal.pl2')), palette);
  // Button labels: exocet10 glyphs in draw mode 4, fechar's PL2 MultiplicativeBlend over the button
  // (retail's create draw list; the rule measured on the title's own frame buffer).
  const labelFont = createFont({
    dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)),
    palette, documentObject, path: FRONTEND.font, drawAs: { op: 'celcolor', mode: 4, blend },
  });
  const campfire = animation(read, decodeDc6, CREATE_ASSETS.campfire, palette, documentObject);
  // The scene is composed at 800x600 (the blend reads pixels back), then scaled onto the page.
  const scene = documentObject.createElement('canvas'); scene.width = 800; scene.height = 600;
  const sceneCtx = scene.getContext('2d', { willReadFrequently: true });
  const heading = string(STR_SELECT_HERO_CLASS) || 'Select Hero Class';
  const nameLabel = string(STR_CHARACTER_NAME) || 'Character Name';

  const base = 'data\\global\\ui\\FrontEnd\\';
  const heroes = CLASSES.map((c) => ({
    ...c, overlay: SELECT_OVERLAYS[c.key] || {},
    description: string(c.descriptionIndex),
    idle: animation(read, decodeDc6, `${base}${c.dir}\\${c.code}NU1.DC6`, palette, documentObject),
    // NU2 is retail's HOVER art, and it is a recolour of NU1 rather than a different animation:
    // read out of the retail MPQs, it carries NU1's frame count, per-frame width/height/offsets and
    // opaque mask, with about three quarters of the opaque pixels at a different palette index. The
    // sprite swaps and the pose does not move, which is what a highlight is. Prior art names it the
    // same way: OpenDiablo2 `select_hero_class.go`/`resource_paths.go` calls NU1 "Unselected" and
    // NU2 "UnselectedH". Drawing NU3 here instead was the visible defect -- NU3 is the CHOSEN art
    // (a different sequence: 18 frames for the Amazon against her idle's 26), so a hovered class
    // played the selected pose. Measured s439, engine b6bb41793228.
    hover: animation(read, decodeDc6, `${base}${c.dir}\\${c.code}NU2.DC6`, palette, documentObject),
    // NU3: retail's CHOSEN art, held after the forward walk -- not the hover art.
    idleSelected: animation(read, decodeDc6, `${base}${c.dir}\\${c.code}NU3.DC6`, palette, documentObject),
    forward: animation(read, decodeDc6, `${base}${c.dir}\\${c.code}FW.DC6`, palette, documentObject),
    back: animation(read, decodeDc6, `${base}${c.dir}\\${c.code}BW.DC6`, palette, documentObject),
    // The overlay half of each of those two states, for the classes 1.13c has one for. Built only
    // where the file is in retail's own load list, so a class without one cannot open a missing cel.
    ...selectOverlayAnimations(SELECT_OVERLAYS[c.key], c, base, read, decodeDc6, palette, documentObject),
  }));

  // The widgets are menu chrome, not part of the character scene: they index the SAME palette the
  // title screen's buttons do, not fechar. Painting them with fechar gives blue-and-white noise --
  // again a wrong picture, not an error, which is why each of these is pinned deliberately.
  const stances = createHeroStances(heroes, { retailHz: RETAIL_HZ, updateHz: UPDATE_HZ });
  const textBoxDc6 = decodeDc6(read(CREATE_ASSETS.textBox));
  const textBox = frameCanvas(textBoxDc6.frames[0], fontPalette, documentObject);
  const buttonDc6 = decodeDc6(read(CREATE_ASSETS.button));
  // ...and it is painted with fechar (95% of its pixels exact in retail's frame; 38% with Units).
  const buttons = buttonDc6.frames.map((f) => frameCanvas(f, palette, documentObject));

  // D2Win's button label rule (frontend.mjs drawLabel; retail's draw list puts EXIT's glyphs at
  // y 564 from x 76): exocet10, centred by whole pixels, anchored at bottom + 4 - (h - line) / 2.
  const buttonLabel = (ctx, text, at) => {
    const w = buttons[0].width, h = buttons[0].height;
    labelFont.drawAnchored(ctx, text, at.x + Math.max(Math.trunc((w - labelFont.measure(text)) / 2), 0), at.y + h - 1 + 4 - Math.trunc((h - labelFont.lineHeight) / 2));
  };
  let hovered = -1, chosen = -1, name = '', tick = 0, drawnDescription = null, drawnClassName = null, drawnName = null, pressed = null, drawnHeroes = [], releaseEvidence = null;
  const clock = () => Math.floor(tick * RETAIL_HZ / UPDATE_HZ);   // retail's 25 Hz front-end tick
  // Retail drives NU1 (idle) and NU3 (selected idle) from the SAME 25 Hz front-end clock, and its
  // loops show frames `floor(t / idleEvery) mod (frames - 1)` -- the last DC6 frame is never shown.
  // The update `tick` is 60 Hz and unbounded, so passing it as a frame index (what this screen did)
  // cycled every frame at 60 Hz through the extra last frame: the "rapid, wrong" class animation.
  const idleFrame = (hero, anim) => Math.floor(clock() / hero.idleEvery) % Math.max(1, anim.count - 1);
  // The CHOSEN idle walks the same `mod (frames - 1)` rule at its own per-class cadence (the comment
  // on CLASSES has the measurement). Same last-frame rule: the DC6's last frame is never shown.
  const selectedFrame = (hero, anim) => Math.floor(clock() / hero.selectedEvery) % Math.max(1, anim.count - 1);
  // The name renderer's caret half-period is retail's 250 ms on the 25 Hz clock: 6.25
  // ticks, so on the integer tick grid the lit and dark runs come out 7 and 6 ticks long -- the same
  // irregularity the capture shows (lit 0-1, 8-14 of 7, dark 2-7, 15-20 of 6). Its phase is an epoch
  // no single capture fixes, so the row aligns the two sequences and compares them frame for frame.
  const caretLit = () => nameRenderer.caretLit(clock());

  // Everything is authored for 800x600; one transform maps it onto whatever the canvas is.
  const view = (width, height) => {
    const scale = Math.min(width / SCREEN.width, height / SCREEN.height);
    return { scale, ox: Math.round((width - SCREEN.width * scale) / 2), oy: Math.round((height - SCREEN.height * scale) / 2) };
  };
  const toScreen = (x, y, width, height) => {
    const v = view(width, height);
    return { x: (x - v.ox) / v.scale, y: (y - v.oy) / v.scale };
  };
  const okBox = () => ({ x: LAYOUT.okButton.x, y: LAYOUT.okButton.y, w: buttons[0].width, h: buttons[0].height });
  const exitBox = () => ({ x: LAYOUT.exitButton.x, y: LAYOUT.exitButton.y, w: buttons[0].width, h: buttons[0].height });
  const inBox = (p, b) => p.x >= b.x && p.x < b.x + b.w && p.y >= b.y && p.y < b.y + b.h;
  const hitControl = (p) => {
    if (inBox(p, okBox())) return 'ok';
    const checkbox = chosen >= 0 ? checkboxes.hitTest(p) : null;
    if (checkbox) return `checkbox-${checkbox}`;
    if (inBox(p, exitBox())) return 'exit';
    const i = heroes.findIndex((h) => inBox(p, h.hit));
    return i >= 0 ? `class-${heroes[i].key}` : null;
  };
  const releaseMatches = (token, p) => {
    if (!token) return false;
    if (token === 'ok') return inBox(p, okBox());
    if (token === 'exit') return inBox(p, exitBox());
    if (token.startsWith('checkbox-')) return checkboxes.contains(token.slice('checkbox-'.length), p);
    if (token.startsWith('class-')) return inBox(p, heroes.find((h) => h.key === token.slice('class-'.length))?.hit || {});
    return false;
  };

  const canCreate = () => chosen >= 0 && name.trim().length >= 2;
  // A hover that began mid-walk lands once the walk ends (Codex F99 review).
  const rehover = () => { if (hovered >= 0) stances.hover(hovered, tick); };
  const sound = (index, trigger) => { if (index >= 0) onSound?.({ key: heroes[index].key, trigger }); };
  const choose = (index) => {
    if (index < 0) return index;
    if (!stances.choose(index, tick)) return chosen;
    const previous = chosen;
    if (previous >= 0 && previous !== index) sound(previous, 'deselect');
    chosen = index; sound(index, 'select');
    return index;
  };

  return {
    get evidence() {
      rehover();
      return {
        background: { width: background.width, height: background.height, rows: background.rows, columns: background.columns },
        fonts: { large: font.lineHeight, small: smallFont.lineHeight },
        classes: heroes.map((h) => ({ key: h.key, playerClass: h.playerClass,
          idle: h.idle.count, hover: h.hover.count, idleSelected: h.idleSelected.count, forward: h.forward.count, back: h.back.count,
          // 0 means this class has NO overlay in retail's own load list for this screen, which is a
          // different fact from "the overlay was there and we did not draw it".
          forwardOverlay: h.forwardOverlay ? h.forwardOverlay.count : 0,
          idleSelectedOverlay: h.idleSelectedOverlay ? h.idleSelectedOverlay.count : 0,
          backOverlay: h.backOverlay ? h.backOverlay.count : 0,
          forwardOverlayMode: h.overlay.forwardBlend ? OVERLAY_MODE : 0,
          idleSelectedOverlayMode: h.idleSelectedOverlay ? OVERLAY_MODE : 0,
          // The cadence the frame was actually driven at, per state, so a row compares the drawn
          // sequence with retail's own capture instead of with the rule that chose it.
          idleEvery: h.idleEvery, selectedEvery: h.selectedEvery,
          descriptionSource: h.descriptionIndex })),
        // This is populated by draw(), not selection: it is the description identity carried to
        // the Font16 blits in this frame (s289 compares it to the requested retail T1 string).
        drawnDescription,
        // The class NAME above the description, carried to the evidence the same way: the font that
        // was blitted, retail's anchor, and the cel row of every glyph that reached the canvas.
        drawnClassName,
        // The name the frame actually painted: the font that was blitted and the per-glyph cel row,
        // carried to the evidence so a row compares painted identity against retail's draw list.
        drawnName,
        textBox: { width: textBox.width, height: textBox.height },
        button: { width: buttons[0].width, height: buttons[0].height, frames: buttons.length },
        // OK is on screen from the moment a class is chosen -- retail draws it dimmed (mode 1) until
        // the name reaches two characters, and in mode 5 with its label after. Reported as the state
        // the frame DREW in, so a row can compare it with retail's own record rather than with the
        // rule that chose it.
        okButton: { drawn: chosen >= 0, mode: chosen < 0 ? 0 : (canCreate() ? 5 : 1),
          labelled: chosen >= 0, x: LAYOUT.okButton.x, y: LAYOUT.okButton.y },
        // The caret's phase in this frame, and the cel it blitted when it drew one.
        caretLit: caretLit(),
        checkboxes: checkboxes.evidence,
        labelGlyphs: labelGlyphs.map((g) => ({ ...g })),
        chosen: chosen >= 0 ? heroes[chosen].key : null,
        hovered: hovered >= 0 ? heroes[hovered].key : null,
        pressed,
        release: releaseEvidence,
        clock: clock(), stances: Object.fromEntries(heroes.map((h, i) => [h.key, stances.stance(i, tick)])), drawnHeroes,
        name, canCreate: canCreate(),
      };
    },
    /** Pick a class by name, so a scenario does not depend on pixel positions. */
    chooseByKey(key) {
      const i = heroes.findIndex((h) => h.key === key);
      return choose(i);
    },
    setName(value) { name = String(value).slice(0, 15); return name; },
    typeKey(code, key) {
      if (code === 'Backspace') { name = name.slice(0, -1); return true; }
      if (code === 'Enter') { if (canCreate()) this.submit(); return true; }
      if (code === 'Escape') { onCancel?.(); return true; }
      if (typeof key === 'string' && key.length === 1 && /[A-Za-z_-]/.test(key) && name.length < 15) { name += key; return true; }
      return false;
    },
    submit() { if (canCreate()) onCreate?.({ playerClass: heroes[chosen].playerClass, name: name.trim(), key: heroes[chosen].key, checkboxes: checkboxes.states() }); },
    hover(x, y, width, height) {
      const p = toScreen(x, y, width, height);
      const next = heroes.findIndex((h) => inBox(p, h.hit));
      if (next !== hovered && next >= 0) sound(next, 'hover');
      if (hovered >= 0 && hovered !== next) stances.unhover(hovered, tick);
      if (next >= 0) stances.hover(next, tick);
      hovered = next;
      return hovered;
    },
    /**
     * Where to click, in canvas pixels, computed from the SAME boxes click() tests -- so a scenario
     * can deliver a real pointer event at a class or at OK without hardcoding positions.
     */
    targets(width, height) {
      const v = view(width, height);
      const centre = (b) => [Math.round(v.ox + (b.x + b.w / 2) * v.scale), Math.round(v.oy + (b.y + b.h / 2) * v.scale)];
      const checkboxTargets = Object.fromEntries(Object.entries(checkboxes.targets()).map(([key, [x, y]]) => [key, [Math.round(v.ox + x * v.scale), Math.round(v.oy + y * v.scale)]]));
      return { classes: Object.fromEntries(heroes.map((h) => [h.key, centre(h.hit)])), ok: centre(okBox()), exit: centre(exitBox()), checkboxes: checkboxTargets };
    },
    click(x, y, width, height) {
      const p = toScreen(x, y, width, height);
      if (inBox(p, okBox())) { this.submit(); return 'ok'; }
      if (chosen >= 0) {
        const checkbox = checkboxes.click(p);
        if (checkbox) return checkbox;
      }
      if (inBox(p, exitBox())) { onCancel?.(); return 'exit'; }
      const i = heroes.findIndex((h) => inBox(p, h.hit));
      if (i >= 0) { choose(i); return heroes[i].key; }
      return null;
    },
    press(x, y, width, height) {
      const p = toScreen(x, y, width, height);
      return (pressed = hitControl(p));
    },
    release(token, x, y, width, height) {
      if (!token) { pressed = null; return null; }
      const p = toScreen(x, y, width, height), now = hitControl(p), matches = releaseMatches(token, p); pressed = null;
      // Checkbox hit regions overlap vertically in the retail DC6; release preserves the captured
      // checkbox identity instead of allowing that overlap to redirect the action to its neighbour.
      const action = matches
        ? token.startsWith('checkbox-') ? checkboxes.toggle(token.slice('checkbox-'.length)) : this.click(x, y, width, height)
        : null;
      releaseEvidence = { token, now, matches, action };
      return action;
    },
    /** A cancelled gesture drops the down cel without activating the control. */
    cancel() { pressed = null; },
    step() { tick++; },
    /** The screen's clock in 60 Hz updates since it opened; a row pins it to a retail capture's tick. */
    get tick() { return tick; }, set tick(v) { tick = v; },
    draw(outCtx, width, height) {
      rehover();
      outCtx.fillStyle = '#000'; outCtx.fillRect(0, 0, width, height);
      const v = view(width, height);
      const ctx = sceneCtx;
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, 800, 600);
      // D2Win's background placement, shared with the title (frontend.mjs tiledPlacements): rows
      // anchored at y 256 and 512 and the last at 599, so row 0 is black and rows 2/3 overlap one line.
      ctx.drawImage(background.canvas, 0, 0);
      recordTiled(CREATE_ASSETS.background, background, 0, 0);

      const centred = (text, x, y) => font.draw(ctx, text, x - font.measure(text) / 2, y);
      centred(heading, LAYOUT.heading.x, LAYOUT.heading.y);
      drawnDescription = null; drawnClassName = null; drawnName = null;
      if (chosen >= 0 || hovered >= 0) {
        const hero = heroes[chosen >= 0 ? chosen : hovered];
        // The class name's own identity: the font, the anchor and each glyph's recorded cel row, so a
        // row compares what was PAINTED with retail's Font30 records rather than with the constant it
        // was told to paint at.
        drawnClassName = { font: FRONTEND.headingFont, text: hero.label, x: LAYOUT.className.x,
          anchorY: LAYOUT.className.y, glyphs: [] };
        classNameGlyphs = drawnClassName.glyphs;
        centred(hero.label, LAYOUT.className.x, LAYOUT.className.y);
        classNameGlyphs = null;
        const lines = descriptionLines(hero.description);
        drawnDescription = {
          key: hero.key, source: hero.descriptionIndex, text: hero.description,
          lines: lines.map((text, index) => ({ text, ...LAYOUT.classDescription[index], font: FONT16.font, glyphs: [] })),
        };
        drawnDescription.lines.forEach((line) => {
          const { text, x, y } = line;
          descriptionGlyphs = line.glyphs;
          // D2Win centres a label with `x -= width / 2` in INTEGER arithmetic, i.e. truncating: the
          // first line of the Barbarian's description is 265 px wide in Font16, so retail's own start
          // is 400 - 132 = 267, not the 268 that rounding the 267.5 produces (retail's records put
          // that line's glyphs at 267..525 and ours at 268..526 -- every glyph, one pixel right).
          // The second line is 260 px wide, so the two rules agree there, which is why only the
          // odd-width line ever differed. Measured: the retail captures, band y=120.
          smallFont.draw(ctx, text, Math.trunc(x - smallFont.measure(text) / 2), y);
          descriptionGlyphs = null;
        });
      }

      // Retail's call order in the name-control band, from its own draw list
      //: EXIT and its label, THEN the text box,
      // then the "CHARACTER NAME" label, then the typed name, then each checkbox's label before its
      // box, and OK and its label last. Drawing them in another order is not a different picture, but
      // it IS a different draw list -- and the draw list is what this screen's identity rows read.
      ctx.drawImage(buttons[0], LAYOUT.exitButton.x, LAYOUT.exitButton.y);
      recordDc6(CREATE_ASSETS.button, 0, buttonDc6.frames[0], LAYOUT.exitButton.x, LAYOUT.exitButton.y);
      buttonLabel(ctx, 'EXIT', LAYOUT.exitButton);

      if (chosen >= 0) {
        ctx.drawImage(textBox, LAYOUT.nameBox.x, LAYOUT.nameBox.y);
        recordDc6(CREATE_ASSETS.textBox, 0, textBoxDc6.frames[0], LAYOUT.nameBox.x, LAYOUT.nameBox.y);
        // Left-aligned at the position the layout gives, in the small font, so it sits above the box
        // rather than through it (retail's own Font16 record starts at x=321 with its bottom at 490).
        labelGlyphs.length = 0;
        goldLabelFont.draw(ctx, nameLabel, LAYOUT.nameLabel.x, LAYOUT.nameLabel.y);
        drawnName = nameRenderer.draw(ctx, name, clock());
        checkboxes.draw(ctx);
        // Retail DRAWS the OK button whether or not it can be used -- what changes is the cel's draw
        // MODE: mode 5 enabled and mode 1 dimmed while
        // the name is under two characters (create-selected.txt record 161, `cel
        // MediumSelButtonBlank.dc6 frame=0 x=627 y=572 mode=1`). Its label is drawn either way: that
        // same frame carries OK's two FontExocet10 cels after the mode-1 button (records 162-163).
        // Drawing nothing made the button appear where retail has it on screen and unlit.
        if (canCreate()) {
          ctx.drawImage(buttons[0], LAYOUT.okButton.x, LAYOUT.okButton.y);
          recordDc6(CREATE_ASSETS.button, 0, buttonDc6.frames[0], LAYOUT.okButton.x, LAYOUT.okButton.y);
        } else {
          blend(ctx, buttonDc6.frames[0], LAYOUT.okButton.x, LAYOUT.okButton.y, 1);
          recordDc6(CREATE_ASSETS.button, 0, buttonDc6.frames[0], LAYOUT.okButton.x, LAYOUT.okButton.y, { mode: 1 });
        }
        buttonLabel(ctx, 'OK', LAYOUT.okButton);
      }

      drawnHeroes = [];
      // `sprite` names which of the hero's retail DC6s was drawn and `path` IS that file, so a row can
      // assert the identity of what was painted rather than that something was. `requestedFrame` and
      // `drawnFrame` are kept apart on purpose: they differ whenever an index wraps.
      const show = (h, stance, pass, sprite, frame) => drawnHeroes.push({ key: h.key, stance, pass, sprite, path: h[sprite].path, count: h[sprite].count,
        requestedFrame: frame, drawnFrame: h[sprite].draw(ctx, frame, h.anchor.x, h.anchor.y) });
      // The overlay half of the SAME state, drawn immediately after its base and at the same anchor --
      // retail's call order. It carries its own path/identity into `drawnHeroes`, so a row can tell a
      // drawn overlay from a missing one (the whole defect) rather than counting cels.
      const showOverlay = (h, stance, pass, sprite, frame, mode) => {
        const anim = h[sprite];
        if (!anim) return;
        const drawnFrame = ((frame % anim.count) + anim.count) % anim.count;
        drawnHeroes.push({ key: h.key, stance, pass, sprite, path: anim.path, count: anim.count,
          requestedFrame: frame, drawnFrame, mode });
        if (mode) anim.drawBlended(ctx, drawnFrame, h.anchor.x, h.anchor.y, blend, mode);
        else anim.draw(ctx, drawnFrame, h.anchor.x, h.anchor.y);
      };
      const renderHero = (i, pass) => {
        const h = heroes[i], stance = stances.stance(i, tick);
        if (stance === HERO_STANCE.APPROACHING) {
          const since = stances.elapsed(i, tick);
          const frame = since;   // one frame per retail tick (F84)
          show(h, stance, pass, 'forward', frame);
          if (h.forwardOverlay) showOverlay(h, stance, pass, 'forwardOverlay', frame, h.overlay.forwardBlend ? OVERLAY_MODE : 0);
        } else if (stance === HERO_STANCE.RETREATING) {
          const since = stances.elapsed(i, tick);
          const duration = Math.max(1, Math.round(h.backMs * RETAIL_HZ / 1000));
          const frame = Math.floor(since * h.back.count / duration);
          show(h, stance, pass, 'back', frame);
          if (h.backOverlay) showOverlay(h, stance, pass, 'backOverlay', frame, h.overlay.backBlend ? OVERLAY_MODE : 0);
        } else if (stance === HERO_STANCE.SELECTED) {
          const frame = selectedFrame(h, h.idleSelected);
          show(h, stance, pass, 'idleSelected', frame);
          if (h.idleSelectedOverlay) showOverlay(h, stance, pass, 'idleSelectedOverlay', frame, OVERLAY_MODE);
        } else if (stance === HERO_STANCE.IDLE_SELECTED) {
          show(h, stance, pass, 'hover', idleFrame(h, h.hover));
        } else {
          show(h, stance, pass, 'idle', idleFrame(h, h.idle));
        }
      };
      // Retail's SelectHeroClass.Render has two passes: all stationary heroes first, then every
      // walker. A moving hero therefore stays above an idle hero where their cels overlap.
      for (const pass of ['stationary', 'moving']) for (const key of DRAW_ORDER) {
        const i = heroes.findIndex((h) => h.key === key);
        const stance = stances.stance(i, tick);
        const moving = stance === HERO_STANCE.APPROACHING || stance === HERO_STANCE.SELECTED || stance === HERO_STANCE.RETREATING;
        if ((pass === 'moving') === moving) renderHero(i, pass);
      }
      campfire.drawBlended(ctx, clock() % (campfire.count - 1), CAMPFIRE.x, CAMPFIRE.y, blend, 3);
      outCtx.save(); outCtx.imageSmoothingEnabled = false;
      outCtx.drawImage(scene, v.ox, v.oy, 800 * v.scale, 600 * v.scale);
      outCtx.restore();
      return { chosen: chosen >= 0 ? heroes[chosen].key : null, hovered: hovered >= 0 ? heroes[hovered].key : null };
    },
  };
}
