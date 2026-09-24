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

import { createFont, decodeFontTbl, FRONTEND, TITLE_COLUMNS } from './frontend.mjs';

/**
 * Per-class layout, from OpenDiablo2's select_hero_class.go.
 *
 * `anchor` is where the sprite is pinned; `hit` is the clickable rectangle as {x, y, w, h} (the
 * source names it a Rectangle with a Max field but stores width/height there, which is why the
 * numbers look wrong as a Max). `forwardMs` is how long the forward-walk animation runs.
 *
 * `playerClass` is OUR engine's class index, which is the order the session's class dropdown uses:
 * 0 Amazon, 1 Sorceress, 2 Necromancer, 3 Paladin, 4 Barbarian, 5 Druid, 6 Assassin.
 */
export const CLASSES = [
  { key: 'amazon',      label: 'Amazon',      playerClass: 0, dir: 'amazon',      code: 'AM', anchor: { x: 100, y: 339 }, hit: { x:  70, y: 220, w: 55, h: 200 }, idleMs: 2500, forwardMs: 2200 },
  { key: 'assassin',    label: 'Assassin',    playerClass: 6, dir: 'assassin',    code: 'AS', anchor: { x: 231, y: 365 }, hit: { x: 175, y: 235, w: 50, h: 180 }, idleMs: 2500, forwardMs: 3800 },
  { key: 'necromancer', label: 'Necromancer', playerClass: 2, dir: 'necromancer', code: 'NE', anchor: { x: 300, y: 335 }, hit: { x: 265, y: 220, w: 55, h: 175 }, idleMs: 1200, forwardMs: 2000 },
  { key: 'barbarian',   label: 'Barbarian',   playerClass: 4, dir: 'barbarian',   code: 'BA', anchor: { x: 400, y: 330 }, hit: { x: 364, y: 201, w: 90, h: 170 }, idleMs:    0, forwardMs: 2500 },
  { key: 'paladin',     label: 'Paladin',     playerClass: 3, dir: 'paladin',     code: 'PA', anchor: { x: 521, y: 338 }, hit: { x: 490, y: 210, w: 65, h: 180 }, idleMs: 2500, forwardMs: 3400 },
  { key: 'sorceress',   label: 'Sorceress',   playerClass: 1, dir: 'sorceress',   code: 'SO', anchor: { x: 626, y: 352 }, hit: { x: 580, y: 240, w: 65, h: 160 }, idleMs: 2500, forwardMs: 2300 },
  { key: 'druid',       label: 'Druid',       playerClass: 5, dir: 'druid',       code: 'DZ', anchor: { x: 720, y: 370 }, hit: { x: 680, y: 220, w: 70, h: 195 }, idleMs: 1500, forwardMs: 4800 },
];

// Label and widget positions, same source.
export const LAYOUT = {
  heading: { x: 400, y: 17 },
  className: { x: 400, y: 65 },
  nameLabel: { x: 321, y: 475 },
  nameBox: { x: 318, y: 493 },
  exitButton: { x: 33, y: 537 },
  okButton: { x: 630, y: 537 },
};

// OpenDiablo2 uses Font30 for the heading and class name and Font16 for the smaller labels; using
// Font30 everywhere makes "CHARACTER NAME" collide with the name box directly beneath it.
export const FONT16 = {
  font: 'data\\local\\font\\latin\\font16.dc6',
  fontTbl: 'data\\local\\font\\latin\\font16.tbl',
};

export const CREATE_ASSETS = {
  background: 'data\\global\\ui\\FrontEnd\\charactercreationscreenEXP.dc6',
  textBox: 'data\\global\\ui\\FrontEnd\\TextBox2.dc6',
  button: 'data\\global\\ui\\FrontEnd\\MediumButtonBlank.dc6',
  // The screen's own palette; the fonts keep the menu palette (see the header comment).
  palette: 'data\\global\\palette\\fechar\\pal.dat',
  // OpenDiablo2 select_hero_class.go: the campfire the heroes stand around, at (380, 335).
  campfire: 'data\\global\\ui\\FrontEnd\\fire.DC6',
};
const CAMPFIRE = { x: 380, y: 335 };
// Retail string-table indices as d2_lang_string_by_index numbers them (found by value; OpenDiablo2's
// numeric_labels.go 1697/1694 count within its own table order): "Select Hero Class", "Character Name".
const STR_SELECT_HERO_CLASS = 5127, STR_CHARACTER_NAME = 5125;

const SCREEN = { width: 800, height: 600 };
const FRAME_MS = 1000 / 25;   // retail front-end sprites run at 25fps

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

/** A lazily-decoded DC6 animation: frames painted on demand, drawn at their own offsets. */
function animation(read, decodeDc6, path, palette, documentObject) {
  const dc6 = decodeDc6(read(path));
  const canvases = new Array(dc6.frames.length).fill(null);
  return {
    path, count: dc6.frames.length,
    draw(ctx, index, anchorX, anchorY) {
      const i = ((index % dc6.frames.length) + dc6.frames.length) % dc6.frames.length;
      const frame = dc6.frames[i];
      if (!frame || !frame.width || !frame.height) return;
      const canvas = canvases[i] ??= frameCanvas(frame, palette, documentObject);
      // A DC6 frame's offset is its BOTTOM-left relative to the anchor (the retail frame puts every
      // hero ~160px higher than top-left anchoring did).
      ctx.drawImage(canvas, Math.round(anchorX + frame.offsetX), Math.round(anchorY + frame.offsetY - frame.height));
    },
  };
}

/**
 * Build the character-creation screen.
 *
 * `onCreate({ playerClass, name })` is called when OK is pressed with a class picked and a name of
 * at least two characters -- retail's own rule.
 */
export function createCharacterCreate({ read, decodeDc6, decodeDatPalette, onCreate, onCancel, string = () => '', documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(CREATE_ASSETS.palette));
  const fontPalette = decodeDatPalette(read(FRONTEND.palette));
  const background = composeTiled(decodeDc6(read(CREATE_ASSETS.background)), palette, TITLE_COLUMNS, documentObject);
  const font = createFont({
    dc6: decodeDc6(read(FRONTEND.headingFont)),
    tbl: decodeFontTbl(read(FRONTEND.headingFontTbl)),
    palette: fontPalette, documentObject,
  });
  const smallFont = createFont({
    dc6: decodeDc6(read(FONT16.font)),
    tbl: decodeFontTbl(read(FONT16.fontTbl)),
    palette: fontPalette, documentObject,
  });

  // Button labels are exocet10 modulated to grey, as on the title screen (d2ui/button.go).
  const labelFont = createFont({
    dc6: decodeDc6(read(FRONTEND.font)), tbl: decodeFontTbl(read(FRONTEND.fontTbl)),
    palette: fontPalette.map((v, i) => (i % 4 === 3 ? v : Math.round(v * 0x64 / 0xff))), documentObject,
  });
  const campfire = animation(read, decodeDc6, CREATE_ASSETS.campfire, palette, documentObject);
  const heading = string(STR_SELECT_HERO_CLASS) || 'Select Hero Class';
  const nameLabel = string(STR_CHARACTER_NAME) || 'Character Name';

  const base = 'data\\global\\ui\\FrontEnd\\';
  const heroes = CLASSES.map((c) => ({
    ...c,
    idle: animation(read, decodeDc6, `${base}${c.dir}\\${c.code}NU1.DC6`, palette, documentObject),
    idleSelected: animation(read, decodeDc6, `${base}${c.dir}\\${c.code}NU3.DC6`, palette, documentObject),
    forward: animation(read, decodeDc6, `${base}${c.dir}\\${c.code}FW.DC6`, palette, documentObject),
  }));

  // The widgets are menu chrome, not part of the character scene: they index the SAME palette the
  // title screen's buttons do, not fechar. Painting them with fechar gives blue-and-white noise --
  // again a wrong picture, not an error, which is why each of these is pinned deliberately.
  const textBoxDc6 = decodeDc6(read(CREATE_ASSETS.textBox));
  const textBox = frameCanvas(textBoxDc6.frames[0], fontPalette, documentObject);
  const buttonDc6 = decodeDc6(read(CREATE_ASSETS.button));
  const buttons = buttonDc6.frames.map((f) => frameCanvas(f, fontPalette, documentObject));

  let hovered = -1, chosen = -1, name = '', chosenAtTick = 0, tick = 0;

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

  const canCreate = () => chosen >= 0 && name.trim().length >= 2;

  return {
    get evidence() {
      return {
        background: { width: background.width, height: background.height, rows: background.rows, columns: background.columns },
        fonts: { large: font.lineHeight, small: smallFont.lineHeight },
        classes: heroes.map((h) => ({ key: h.key, playerClass: h.playerClass,
          idle: h.idle.count, idleSelected: h.idleSelected.count, forward: h.forward.count })),
        textBox: { width: textBox.width, height: textBox.height },
        button: { width: buttons[0].width, height: buttons[0].height, frames: buttons.length },
        chosen: chosen >= 0 ? heroes[chosen].key : null,
        name, canCreate: canCreate(),
      };
    },
    /** Pick a class by name, so a scenario does not depend on pixel positions. */
    chooseByKey(key) {
      const i = heroes.findIndex((h) => h.key === key);
      if (i >= 0) { chosen = i; chosenAtTick = tick; }
      return i;
    },
    setName(value) { name = String(value).slice(0, 15); return name; },
    typeKey(code, key) {
      if (code === 'Backspace') { name = name.slice(0, -1); return true; }
      if (code === 'Enter') { if (canCreate()) this.submit(); return true; }
      if (code === 'Escape') { onCancel?.(); return true; }
      if (typeof key === 'string' && key.length === 1 && /[A-Za-z_-]/.test(key) && name.length < 15) { name += key; return true; }
      return false;
    },
    submit() { if (canCreate()) onCreate?.({ playerClass: heroes[chosen].playerClass, name: name.trim(), key: heroes[chosen].key }); },
    hover(x, y, width, height) {
      const p = toScreen(x, y, width, height);
      hovered = heroes.findIndex((h) => inBox(p, h.hit));
      return hovered;
    },
    /**
     * Where to click, in canvas pixels, computed from the SAME boxes click() tests -- so a scenario
     * can deliver a real pointer event at a class or at OK without hardcoding positions.
     */
    targets(width, height) {
      const v = view(width, height);
      const centre = (b) => [Math.round(v.ox + (b.x + b.w / 2) * v.scale), Math.round(v.oy + (b.y + b.h / 2) * v.scale)];
      return { classes: Object.fromEntries(heroes.map((h) => [h.key, centre(h.hit)])), ok: centre(okBox()), exit: centre(exitBox()) };
    },
    click(x, y, width, height) {
      const p = toScreen(x, y, width, height);
      if (inBox(p, okBox())) { this.submit(); return 'ok'; }
      if (inBox(p, exitBox())) { onCancel?.(); return 'exit'; }
      const i = heroes.findIndex((h) => inBox(p, h.hit));
      if (i >= 0) { chosen = i; chosenAtTick = tick; return heroes[i].key; }
      return null;
    },
    step() { tick++; },
    draw(ctx, width, height) {
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, width, height);
      const v = view(width, height);
      ctx.save();
      ctx.translate(v.ox, v.oy); ctx.scale(v.scale, v.scale);
      ctx.drawImage(background.canvas, 0, 0);

      for (let i = 0; i < heroes.length; i++) {
        const h = heroes[i];
        if (i === chosen) {
          // The chosen class walks forward once, then holds its selected idle.
          const forwardFrames = Math.max(1, Math.round(h.forwardMs / FRAME_MS));
          const since = tick - chosenAtTick;
          if (since < forwardFrames) h.forward.draw(ctx, Math.floor(since * h.forward.count / forwardFrames), h.anchor.x, h.anchor.y);
          else h.idleSelected.draw(ctx, tick, h.anchor.x, h.anchor.y);
        } else if (i === hovered) {
          h.idleSelected.draw(ctx, tick, h.anchor.x, h.anchor.y);
        } else {
          h.idle.draw(ctx, tick, h.anchor.x, h.anchor.y);
        }
      }
      campfire.draw(ctx, tick, CAMPFIRE.x, CAMPFIRE.y);

      const centred = (text, x, y) => font.draw(ctx, text, x - font.measure(text) / 2, y);
      centred(heading, LAYOUT.heading.x, LAYOUT.heading.y);
      if (chosen >= 0 || hovered >= 0) centred(heroes[chosen >= 0 ? chosen : hovered].label, LAYOUT.className.x, LAYOUT.className.y);

      if (chosen >= 0) {
        // Left-aligned at the position the layout gives, in the small font, so it sits above the
        // box rather than through it.
        smallFont.draw(ctx, nameLabel, LAYOUT.nameLabel.x, LAYOUT.nameLabel.y);
        ctx.drawImage(textBox, LAYOUT.nameBox.x, LAYOUT.nameBox.y);
        font.draw(ctx, name, LAYOUT.nameBox.x + 8, LAYOUT.nameBox.y + (textBox.height - font.lineHeight) / 2);
        // OK is only offered once retail's rule is satisfied: a class and a name of 2+ characters.
        if (canCreate()) {
          ctx.drawImage(buttons[0], LAYOUT.okButton.x, LAYOUT.okButton.y);
          labelFont.draw(ctx, 'OK', LAYOUT.okButton.x + (buttons[0].width - labelFont.measure('OK')) / 2, LAYOUT.okButton.y + (buttons[0].height - labelFont.cellHeight) / 2);
        }
      }
      ctx.drawImage(buttons[0], LAYOUT.exitButton.x, LAYOUT.exitButton.y);
      labelFont.draw(ctx, 'EXIT', LAYOUT.exitButton.x + (buttons[0].width - labelFont.measure('EXIT')) / 2, LAYOUT.exitButton.y + (buttons[0].height - labelFont.cellHeight) / 2);
      ctx.restore();
      return { chosen: chosen >= 0 ? heroes[chosen].key : null, hovered: hovered >= 0 ? heroes[hovered].key : null };
    },
  };
}
