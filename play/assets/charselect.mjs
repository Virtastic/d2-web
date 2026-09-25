// Character select: the saved characters you can pick up where you left off.
//
// Retail draws eight slots with a rendered portrait of each character and its name, class and
// level. This draws the retail background and the retail font, and lists the saved characters as
// text rows. The PORTRAITS are the declared gap: they need the
// character's equipment composed the way the in-game player sprite is, which is a bigger job than
// the list itself and is not required to pick a character.
//
// Everything about which characters exist comes from the store, and everything the store holds is a
// real .d2s produced by the engine -- nothing here invents character state.

import { createFont, decodeFontTbl, FRONTEND, TITLE_COLUMNS } from './frontend.mjs';
import { CLASS_NAMES } from './save-store.mjs';

export const SELECT_ASSETS = {
  // Note the directory: this one lives under CharSelect, not FrontEnd.
  background: 'data\\global\\ui\\CharSelect\\characterselectscreenEXP.dc6',
  button: 'data\\global\\ui\\FrontEnd\\MediumButtonBlank.dc6',
  // THIS screen is Sky, not fechar -- the creation screen next door uses fechar, and swapping them
  // renders a blown-out blue picture with no error. Per OpenDiablo2's character_select.go, which
  // loads the background with PaletteSky and its labels with PaletteUnits.
  palette: 'data\\global\\palette\\Sky\\pal.dat',
};

// Retail labels these rows with the 16px face, not the 30px one used for headings.
const FONT16 = { font: 'data\\local\\font\\latin\\font16.dc6', fontTbl: 'data\\local\\font\\latin\\font16.tbl' };

const SCREEN = { width: 800, height: 600 };
const ROW_HEIGHT = 34;
const LIST = { x: 40, y: 100, width: 420, rows: 8 };

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

/**
 * `characters` is the store's listing. `onPlay(name)` fires for the chosen character,
 * `onCreate()` for a new one, `onCancel()` to go back.
 */
export function createCharacterSelect({ read, decodeDc6, decodeDatPalette, characters, onPlay, onCreate, onCancel, documentObject = globalThis.document }) {
  const palette = decodeDatPalette(read(SELECT_ASSETS.palette));
  const fontPalette = decodeDatPalette(read(FRONTEND.palette));
  const background = composeTiled(decodeDc6(read(SELECT_ASSETS.background)), palette, TITLE_COLUMNS, documentObject);
  const font = createFont({
    dc6: decodeDc6(read(FRONTEND.headingFont)), tbl: decodeFontTbl(read(FRONTEND.headingFontTbl)),
    palette: fontPalette, documentObject,
  });
  const rowFont = createFont({
    dc6: decodeDc6(read(FONT16.font)), tbl: decodeFontTbl(read(FONT16.fontTbl)),
    palette: fontPalette, documentObject,
  });
  const buttonDc6 = decodeDc6(read(SELECT_ASSETS.button));
  const button = frameCanvas(buttonDc6.frames[0], fontPalette, documentObject);

  const list = characters.slice(0, LIST.rows);
  let selected = list.length ? 0 : -1;

  const view = (width, height) => {
    const scale = Math.min(width / SCREEN.width, height / SCREEN.height);
    return { scale, ox: Math.round((width - SCREEN.width * scale) / 2), oy: Math.round((height - SCREEN.height * scale) / 2) };
  };
  const toScreen = (x, y, width, height) => {
    const v = view(width, height);
    return { x: (x - v.ox) / v.scale, y: (y - v.oy) / v.scale };
  };
  const rowBox = (i) => ({ x: LIST.x, y: LIST.y + i * ROW_HEIGHT, w: LIST.width, h: ROW_HEIGHT });
  const playBox = () => ({ x: 630, y: 500, w: button.width, h: button.height });
  const createBox = () => ({ x: 630, y: 455, w: button.width, h: button.height });
  const exitBox = () => ({ x: 33, y: 500, w: button.width, h: button.height });
  const inBox = (p, b) => p.x >= b.x && p.x < b.x + b.w && p.y >= b.y && p.y < b.y + b.h;

  return {
    get evidence() {
      return {
        background: { width: background.width, height: background.height, rows: background.rows, columns: background.columns },
        characters: list.map((ch) => ({ name: ch.name, className: ch.className, level: ch.level })),
        selected: selected >= 0 ? list[selected].name : null,
        button: { width: button.width, height: button.height },
        fonts: { heading: font.lineHeight, row: rowFont.lineHeight },
      };
    },
    selectByName(name) {
      const i = list.findIndex((ch) => ch.name === name);
      if (i >= 0) selected = i;
      return i;
    },
    play() { if (selected >= 0) onPlay?.(list[selected].name); },
    /** Canvas-pixel centres of the SAME boxes click() tests: each listed character, and the buttons. */
    targets(width, height) {
      const v = view(width, height);
      const centre = (b) => [Math.round(v.ox + (b.x + b.w / 2) * v.scale), Math.round(v.oy + (b.y + b.h / 2) * v.scale)];
      const characters = Object.fromEntries(list.map((ch, i) => [ch.name, centre(rowBox(i))]));
      const play = centre(playBox()), create = centre(createBox()), exit = centre(exitBox());
      // `rows`, OK, 'CREATE NEW CHARACTER', EXIT: the names s000b/s000d aim by (same points).
      return { characters, rows: characters, play, create, exit, OK: play, 'CREATE NEW CHARACTER': create, EXIT: exit };
    },
    click(x, y, width, height) {
      const p = toScreen(x, y, width, height);
      if (inBox(p, playBox())) { this.play(); return 'play'; }
      if (inBox(p, createBox())) { onCreate?.(); return 'create'; }
      if (inBox(p, exitBox())) { onCancel?.(); return 'exit'; }
      for (let i = 0; i < list.length; i++) {
        if (inBox(p, rowBox(i))) { selected = i; return list[i].name; }
      }
      return null;
    },
    draw(ctx, width, height) {
      ctx.fillStyle = '#000'; ctx.fillRect(0, 0, width, height);
      const v = view(width, height);
      ctx.save();
      ctx.translate(v.ox, v.oy); ctx.scale(v.scale, v.scale);
      ctx.drawImage(background.canvas, 0, 0);

      const centred = (text, x, y) => font.draw(ctx, text, x - font.measure(text) / 2, y);
      centred('SELECT YOUR HERO', SCREEN.width / 2, 24);

      for (let i = 0; i < list.length; i++) {
        const ch = list[i], box = rowBox(i);
        if (i === selected) {
          ctx.fillStyle = 'rgba(120,96,40,.35)';
          ctx.fillRect(box.x, box.y, box.w, box.h);
        }
        rowFont.draw(ctx, ch.name, box.x + 8, box.y + 6);
        const right = `${(CLASS_NAMES[ch.playerClass] ?? ch.className ?? '').toUpperCase()}  LVL ${ch.level}`;
        rowFont.draw(ctx, right, box.x + box.w - rowFont.measure(right) - 8, box.y + 6);
      }
      if (!list.length) centred('NO SAVED CHARACTERS', SCREEN.width / 2, LIST.y + 20);

      for (const [label, box] of [['PLAY', playBox()], ['NEW', createBox()], ['EXIT', exitBox()]]) {
        ctx.drawImage(button, box.x, box.y);
        centred(label, box.x + box.w / 2, box.y + (box.h - font.lineHeight) / 2);
      }
      ctx.restore();
      return { characters: list.length, selected };
    },
  };
}
