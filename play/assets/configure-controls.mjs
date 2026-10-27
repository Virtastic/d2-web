// F218: retail's CONFIGURE CONTROLS panel (Esc -> OPTIONS -> CONFIGURE
// CONTROLS). Every constant below is a record of retail 1.13c's OWN draw list -- the 11 captures in
// `the retail captures` plus the 26-view click-by-click scroll sweep they came
// from -- never derived from pixels. the decompile carries the table and the per-item evidence.
//
// Geometry:
//   * outer fill `box 90 70 710 439` colour 0 mode 1
//   * the panel frame is 270 `data\global\ui\menu\boxpieces.dc6` cels: an outer and an inner box
//     sharing the bottom edge, generated below from the measured cycles (the inner bottom row really
//     is the same 50 records drawn twice -- retail's own frame has them twice)
//   * `data\global\ui\Menu\TextSlid.dc6`: the trough (frame 13, 21 cels from y 129 step 12), the thumb
//     (frame 14, 5 cels), the up arrow (frame 11 at 679,117) and the down arrow (frame 10 at 679,375)
//   * the header at y 100 ('Function' x 108, 'Key/Button One' x 298, 'Key/Button Two' x 488) and the
//     rows at x 120 / 310 / 500, y 122 + 18n
//   * the focus rectangle `box 108 <rowY-17> 678 <rowY+1>`, drawn immediately before the focused row
//
// Text is `data\local\font\latin\FontInGameChat.dc6` (retail's font 13); its glyph frames ARE the
// ASCII codes (retail's own record for 'F' is frame 70), so the identity row compares frame-for-frame.
// The colour index is retail's own and is drawn through the act palette's .pl2 shift table:
//   0 description, 4 bound key, 5 'None' on a function that has another binding,
//   1 red 'None' on a function with neither cell bound, 3 the focused (or armed) row.
//
// The list is 56 functions in 62 raw slots (including blank sentinels), with 15 visible. Native
// FUN_6FAC1A80 changes the raw top index by one per accepted arrow tick; captured views are samples
// taken after different amounts of held input, not a table of one-click transitions.
import { decodeDatPalette, decodeDc6 } from './d2-formats.mjs';
import { createFont, decodeFontTbl, textColourPalette } from './frontend.mjs';
import { createPanelChrome } from './panel-chrome.mjs';
import { recordPrim } from './drawlist.mjs';
import { createConfigureScrollHold } from './configure-scroll-hold.mjs';

const FONT = 'data\\local\\font\\latin\\fontingamechat.dc6';
const BOXPIECES = 'data\\global\\ui\\MENU\\boxpieces.dc6';
const TEXTSLID = 'data\\global\\ui\\Menu\\TextSlid.dc6';
export const CONFIGURE_CONTROLS_STORAGE_KEY = 'd2web.controls.bindings';

/** Retail footer actions arm on left-button down and run only when that same pointer releases over
 * the same footer entry. */
export function configureFooterRelease(press, pointerId, button, releasedButton) {
  if (!press || press.pointerId !== pointerId) return { owned: false, activate: null };
  return { owned: true, activate: button === 0 && releasedButton === press.name ? press.name : null };
}

/** Retail's own default binding table, as its Configure Controls screen prints it: 62 slots, 56
 *  functions, `null` where the sweep shows a blank line. `[name, key one, key two]`. */
export const CONFIGURE_CONTROLS_SLOTS = Object.freeze([
  ['Character Screen', 'A', 'C'], ['Inventory Screen', 'I', 'B'], ['Party Screen', 'P', null],
  ['Hireling Screen', 'O', null], ['Message Log', 'M', null], ['Quest Log', 'Q', null], ['Help Screen', 'H', null],
  null,
  ['Skill Tree', 'T', null], ['Skill Speed Bar', 'S', null],
  ['Skill 1', 'F1', null], ['Skill 2', 'F2', null], ['Skill 3', 'F3', null], ['Skill 4', 'F4', null],
  ['Skill 5', 'F5', null], ['Skill 6', 'F6', null], ['Skill 7', 'F7', null], ['Skill 8', 'F8', null],
  ['Skill 9', null, null], ['Skill 10', null, null], ['Skill 11', null, null], ['Skill 12', null, null],
  ['Skill 13', null, null], ['Skill 14', null, null], ['Skill 15', null, null], ['Skill 16', null, null],
  ['Select Previous Skill', null, null], ['Select Next Skill', null, null],
  null,
  ['Show Belt', '~', null], ['Use Belt 1', '1', null], ['Use Belt 2', '2', null], ['Use Belt 3', '3', null],
  ['Use Belt 4', '4', null],
  ['Swap Weapons', 'W', null],
  null,
  ['Chat', 'Enter', null], ['Run', 'Ctrl', null], ['Toggle Run/Walk', 'R', null], ['Stand Still', 'Shift', null],
  ['Show Items', 'Alt', null], ['Show Portraits', 'Z', null],
  null,
  ['Automap', 'Tab', null], ['Center Automap', 'F9', null], ['Fade Automap', 'F10', null],
  ['Party on Automap', 'F11', null], ['Names on Automap', 'F12', null], ['Toggle MiniMap', 'V', null],
  null,
  ["Say 'Help'", 'Num Pad 0', null], ["Say 'Follow me'", 'Num Pad 1', null], ["Say 'This is for you'", 'Num Pad 2', null],
  ["Say 'Thanks'", 'Num Pad 3', null], ["Say 'Sorry'", 'Num Pad 4', null], ["Say 'Bye'", 'Num Pad 5', null],
  ["Say 'Now you die'", 'Num Pad 6', null], ["Say 'Retreat'", 'Num Pad 7', null],
  null,
  ['Screen Shot', 'Print Screen', null], ['Clear Screen', 'Space', null], ['Clear Messages', 'N', null],
]);

/** Captured examples of first nonblank text slot and thumb anchor. Blank raw slots may precede that
 *  text row; these are observations after varying held durations, not one-click transitions. */
export const CONFIGURE_CONTROLS_CAPTURED_VIEWS = Object.freeze([
  [0, 129], [2, 136], [4, 144], [8, 156], [9, 164], [12, 176], [14, 184], [16, 192], [18, 200],
  [21, 212], [23, 220], [25, 227], [27, 235], [29, 243], [31, 251], [33, 259], [36, 267], [37, 275],
  [39, 283], [41, 291], [43, 299], [46, 311], [47, 315],
]);

export const CONFIGURE_CONTROLS_LAYOUT = Object.freeze({
  fill: [90, 70, 710, 439], focusX: [108, 678],
  descX: 120, key1X: 310, key2X: 500, headerX: [108, 298, 488], headerY: 100,
  rowY: 122, rowPitch: 18, rows: 15,
  buttons: { Cancel: [171, 421], Default: [374, 421], Accept: [583, 421] },
  slider: { trough: [679, 129, 21], thumb: [678, 5], up: [679, 117], down: [679, 375], step: 12 },
});
const L = CONFIGURE_CONTROLS_LAYOUT;
const MAX_SCROLL_INDEX = Math.max(0, CONFIGURE_CONTROLS_SLOTS.length - L.rows);
const THUMB_TRAVEL = 0xf6 - L.slider.thumb[1] * L.slider.step;

// D2Client stores the top table index and maps it linearly to the thumb with integer division.
// The native screen-relative base resolves to the captured top anchor (129) at this panel size.
function thumbYForScrollIndex(index) {
  return 129 + Math.trunc(THUMB_TRAVEL * index / MAX_SCROLL_INDEX);
}

/** The 270 boxpieces cels of the panel frame, in retail's own call order. */
export function configureControlsPieces() {
  const out = [], top = [3, 4, 5, 6, 7, 2], bot = [19, 20, 21, 16, 17, 18];
  const run = (frames, x0, y, n) => { for (let i = 0; i < n; i++) out.push([frames[i % frames.length], x0 + 12 * i, y]); };
  const runV = (frames, x, y0, n) => { for (let i = 0; i < n; i++) out.push([frames[i % frames.length], x, y0 + 12 * i]); };
  run(top, 102, 85, 50); run(bot, 103, 449, 50);
  runV([12, 10, 11], 86, 95, 29); runV([14, 15, 13], 701, 97, 29);
  out.push([0, 90, 85], [1, 696, 85], [8, 90, 440], [9, 696, 440]);
  run(top, 102, 405, 50); run(bot, 103, 449, 50);
  out.push([12, 86, 415], [10, 86, 427], [14, 701, 417], [15, 701, 429]);
  out.push([0, 90, 405], [1, 696, 405], [8, 90, 440], [9, 696, 440]);
  return out;
}

/** The browser key a physical key press names, in retail's own words: the panel prints these, and
 *  retail's key-name strings are `A`..`Z`, `F1`..`F12`, `Num Pad 0`..`Num Pad 9`, `Print Screen`,
 *  `Space`, `Ctrl`, `Shift`, `Alt`, `Tab`, `Enter`, `~`, `1`..`9`. */
export function keyName(code) {
  if (/^Key[A-Z]$/.test(code)) return code.slice(3);
  if (/^Digit[0-9]$/.test(code)) return code.slice(5);
  if (/^Numpad[0-9]$/.test(code)) return `Num Pad ${code.slice(6)}`;
  if (/^F([1-9]|1[0-2])$/.test(code)) return code;
  if (code === 'Backquote') return '~';
  if (code === 'Space') return 'Space';
  if (code === 'Tab') return 'Tab';
  if (code === 'Enter' || code === 'NumpadEnter') return 'Enter';
  if (code === 'PrintScreen') return 'Print Screen';
  if (code === 'ShiftLeft' || code === 'ShiftRight') return 'Shift';
  if (code === 'ControlLeft' || code === 'ControlRight') return 'Ctrl';
  if (code === 'AltLeft' || code === 'AltRight') return 'Alt';
  return null;
}

/** Whether a physical DOM key is assigned to a named retail control in a binding snapshot. */
export function controlBindingMatches(bindings, functionName, code) {
  return controlActionForKey(bindings, code) === functionName;
}

/** Resolve the configured retail action for a DOM key. Last-writer-wins keeps each key unique. */
export function controlActionForKey(bindings, code) {
  const name = keyName(code);
  if (!name) return null;
  for (let slot = 0; slot < CONFIGURE_CONTROLS_SLOTS.length; slot++) {
    const entry = CONFIGURE_CONTROLS_SLOTS[slot];
    if (entry && bindings?.[slot]?.some((key) => key === name)) return entry[0];
  }
  return null;
}

/** The key name bound to each Use Belt 1-4 slot (primary, else secondary), null when unbound: the belt's hotkey labels. */
export function beltKeyNames(bindings) {
  return [1, 2, 3, 4].map((n) => {
    const slot = CONFIGURE_CONTROLS_SLOTS.findIndex((e) => e && e[0] === `Use Belt ${n}`);
    return bindings?.[slot]?.find((k) => k) || null;
  });
}

const blank = () => CONFIGURE_CONTROLS_SLOTS.map((s) => (s ? [s[1], s[2]] : null));

const keyCodes = [
  ...'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.split('').map((letter) => [`Key${letter}`, letter]),
  ...'0123456789'.split('').map((digit) => [`Digit${digit}`, digit]),
  ...Array.from({ length: 12 }, (_, i) => [`F${i + 1}`, `F${i + 1}`]),
  ...Array.from({ length: 10 }, (_, i) => [`Numpad${i}`, `Num Pad ${i}`]),
  ['Backquote', '~'], ['Space', 'Space'], ['Tab', 'Tab'], ['Enter', 'Enter'],
  ['NumpadEnter', 'Enter'], ['PrintScreen', 'Print Screen'], ['ShiftLeft', 'Shift'],
  ['ShiftRight', 'Shift'], ['ControlLeft', 'Ctrl'], ['ControlRight', 'Ctrl'], ['AltLeft', 'Alt'], ['AltRight', 'Alt'],
];
const validKeyNames = new Set(keyCodes.map(([, name]) => name));

function decodeSavedBindings(raw) {
  try {
    const parsed = JSON.parse(raw);
    if (!Array.isArray(parsed) || parsed.length !== CONFIGURE_CONTROLS_SLOTS.length) return null;
    const seen = new Set();
    for (let i = 0; i < parsed.length; i++) {
      const binding = parsed[i];
      if (!CONFIGURE_CONTROLS_SLOTS[i]) {
        if (binding !== null) return null;
        continue;
      }
      if (!Array.isArray(binding) || binding.length !== 2) return null;
      for (const name of binding) {
        if (name === null) continue;
        if (typeof name !== 'string' || !validKeyNames.has(name) || seen.has(name)) return null;
        seen.add(name);
      }
    }
    return parsed.map((binding) => binding ? [...binding] : null);
  } catch { return null; }
}

/** Restore only a complete, non-conflicting Configure Controls table; invalid profile data uses retail defaults. */
export function restoreControlBindings(raw) {
  return decodeSavedBindings(raw) ?? blank();
}

export function createConfigureControls({ read, palettePath = () => 'data\\global\\palette\\ACT1\\pal.dat', documentObject = globalThis.document,
  storage = undefined, now = () => performance.now() }) {
  const chrome = createPanelChrome(read, documentObject);
  const fontDc6 = decodeDc6(read(FONT)), tbl = decodeFontTbl(read(FONT.replace(/\.dc6$/i, '.tbl')));
  const fonts = new Map();
  const resolved = () => (typeof palettePath === 'function' ? palettePath() : palettePath);
  const fontFor = (colour) => {
    const key = `${resolved()}#${colour}`;
    let f = fonts.get(key);
    if (!f) {
      const pal = decodeDatPalette(read(resolved())), at = resolved();
      let pl2 = null; try { pl2 = read(at.replace(/pal\.dat$/i, 'pal.pl2')); } catch { /* the plain palette then */ }
      // F713: colour 0 is "no shift" -- the glyphs draw through the RAW act palette, exactly as every
      // other colour-0 font here (npc-menu's white, charselect's title, the version label). The act
      // PL2's TextColorShifts[0] is all-zero bytes, so passing 0 into it maps EVERY ink pixel to
      // palette[0] and the Function column drew black-on-dark. Retail's own frame draws that column
      // white. A colourless font must not consult the shift table at all.
      f = createFont({ dc6: fontDc6, tbl, documentObject, path: FONT, drawAs: { op: 'celcolor', mode: 5, extra: colour },
        palette: (pl2 && colour) ? textColourPalette(pl2, pal, colour) : pal });
      fonts.set(key, f);
    }
    return f;
  };

  let preferenceStore = storage;
  if (preferenceStore === undefined) { try { preferenceStore = globalThis.localStorage; } catch { preferenceStore = null; } }
  let committed = blank();
  try {
    const saved = preferenceStore?.getItem(CONFIGURE_CONTROLS_STORAGE_KEY);
    if (saved !== null && saved !== undefined) committed = restoreControlBindings(saved);
  } catch { /* blocked, absent or corrupt preference storage leaves retail's captured defaults */ }
  let pending = blank();           // the panel's own copy: what the panel draws and edits
  let open = false, tick = 0, scrollIndex = 0;
  const scrollHold = createConfigureScrollHold();
  let focus = null, armed = null;  // focus/armed are {slot, cell}, cell 0 = Key/Button One
  let buttonHover = null, buttonPress = null;

  const slotAt = (i) => scrollIndex + i;   // native renderer advances through raw slots, including blanks
  const rowY = (i) => L.rowY + L.rowPitch * i;
  const focusBox = () => (focus ? [L.focusX[0], rowY(focus.slot - scrollIndex) - L.rowPitch + 1,
    L.focusX[1], rowY(focus.slot - scrollIndex) + 1] : null);
  const blinkOff = () => (tick >> 1) % 2 === 1;

  /** What a canvas point lands on, at retail's own geometry. Retail's own capture clicks these exact
   *  points (`the retail capture tool/drawlist-session.sh`: (310,122) arms row 0's Key/Button One, (310,140)
   *  row 1's, (171/374/421) the buttons, (683,367) the down arrow), so the rects must accept them. */
  const hit = (px, py, width, height) => {
    const scale = Math.min(width / 800, height / 600);
    const x = (px - (width - 800 * scale) / 2) / scale, y = (py - (height - 600 * scale) / 2) / scale;
    for (const [name, [bx, by]] of Object.entries(L.buttons)) {
      if (x >= bx - 45 && x < bx + 90 && y >= by - 20 && y < by + 16) return { button: name };
    }
    if (x >= 671 && x < 696) {
      if (y >= 105 && y < 129) return { scroll: 'up' };
      if (y >= 363 && y < 387) return { scroll: 'down' };
    }
    for (let i = 0; i < L.rows; i++) {
      if (y < rowY(i) - L.rowPitch + 1 || y >= rowY(i) + 2) continue;
      if (x >= 298 && x < 488) return { cell: { slot: slotAt(i), cell: 0 } };
      if (x >= 488 && x < 678) return { cell: { slot: slotAt(i), cell: 1 } };
      if (x >= L.focusX[0] && x < L.focusX[1]) return { row: slotAt(i) };
    }
    return null;
  };

  const arm = (slot, cell) => { if (pending[slot]) { armed = { slot, cell }; focus = { slot, cell }; tick = 0; } };
  const scrollBy = (delta) => { scrollIndex = Math.min(MAX_SCROLL_INDEX, Math.max(0, scrollIndex + delta));
    // Retail's own scroll frame has no focus rectangle and no colour 3 anywhere: scrolling drops it.
    focus = null; armed = null; };

  /** Retail's conflict rule, measured (`configure-controls-conflict.txt`): last writer wins -- the new
   *  owner takes the key and the old one's cell becomes unbound, with no warning. */
  const assign = (slot, cell, name) => {
    for (let s = 0; s < pending.length; s++) {
      const b = pending[s]; if (!b) continue;
      for (let c = 0; c < 2; c++) if (b[c] === name && !(s === slot && c === cell)) b[c] = null;
    }
    pending[slot][cell] = name;
    armed = null;
  };

  return {
    get open() { return open; },
    get bindings() { return committed.map((b) => (b ? [...b] : null)); },
    isBound(functionName, code) { return controlBindingMatches(committed, functionName, code); },
    actionFor(code) { return controlActionForKey(committed, code); },
    get evidence() { return { open, stop: scrollIndex, scrollIndex, firstSlot: scrollIndex, thumbY: thumbYForScrollIndex(scrollIndex),
      focus, armed, tick, blinkOff: blinkOff(), pendingRows: pending.filter(Boolean).length,
      focusBox: focusBox(), bindings: this.bindings }; },
    openPanel() { scrollHold.cancel(); open = true; scrollIndex = 0; focus = { slot: 0, cell: 0 }; armed = null; buttonHover = null; buttonPress = null; pending = committed.map((b) => (b ? [...b] : null)); tick = 0; },
    close() { scrollHold.cancel(); buttonPress = null; open = false; },
    pointerDown(px, py, width, height, pointerId, button) {
      if (!scrollHold.accepts(pointerId)) return null;
      const target = open ? hit(px, py, width, height) : null;
      if (target?.button) {
        scrollHold.down(pointerId, null, button, now());
        buttonHover = target.button;
        buttonPress = button === 0 ? { name: target.button, pointerId } : null;
        return { ...target, armed: Boolean(buttonPress) };
      }
      buttonPress = null;
      const time = now();
      scrollHold.down(pointerId, target?.scroll, button, time);
      if (button === 0 && target?.scroll) {
        const delta = scrollHold.poll(time);
        if (delta) scrollBy(delta);
        return target;
      }
      return button === 0 ? this.click(px, py, width, height) : null;
    },
    pointerMove(px, py, width, height, pointerId, buttons) {
      const target = open ? hit(px, py, width, height) : null;
      scrollHold.move(pointerId, target?.scroll ?? null, buttons);
      buttonHover = target?.button ?? null;
    },
    pointerUp(pointerId, button, px, py, width, height) {
      const press = buttonPress;
      const released = open ? hit(px, py, width, height)?.button ?? null : null;
      const result = configureFooterRelease(press, pointerId, button, released);
      if (result.owned) {
        buttonPress = null;
        this.pointerMove(px, py, width, height, pointerId, 0);
        if (result.activate) this.click(px, py, width, height);
        return result;
      }
      return scrollHold.release(pointerId, button);
    },
    pointerCancel(pointerId) {
      if (pointerId === undefined) {
        buttonPress = null;
        return scrollHold.cancel();
      }
      if (buttonPress?.pointerId === pointerId) { buttonPress = null; return true; }
      return scrollHold.cancel(pointerId);
    },
    step() { tick++; },
    /** `Esc` from an armed cell cancels the arm and leaves the focus ONE SLOT LOWER: measured, on
     *  retail's own capture, which shows the focus rectangle and the colour-3 cell on row 1 after the
     *  capture script armed row 0 and pressed Escape (its MANIFEST prose says the clicked cell; the
     *  vector is the oracle). Esc with nothing armed leaves the panel, back to the Options page. */
    key(code) {
      if (!open) return false;
      if (code === 'Escape') {
        if (!armed) { scrollHold.cancel(); open = false; return true; }
        const was = armed; armed = null;
        let slot = was.slot + 1;
        while (slot < pending.length && !pending[slot]) slot++;
        focus = { slot: Math.min(slot, pending.length - 1), cell: was.cell };
        tick = 0; return true;
      }
      const name = keyName(code);
      if (armed && name) { assign(armed.slot, armed.cell, name); return true; }
      return false;
    },
    click(px, py, width, height) {
      if (!open) return null;
      const t = hit(px, py, width, height);
      if (!t) return null;
      if (t.scroll) { scrollBy(t.scroll === 'down' ? 1 : -1); return t; }
      if (t.cell) { arm(t.cell.slot, t.cell.cell); return t; }
      if (t.button === 'Cancel') { scrollHold.cancel(); pending = committed.map((b) => (b ? [...b] : null)); open = false; return t; }
      if (t.button === 'Default') { pending = blank(); return t; }   // retail: Default resets the list and leaves the focus alone (configure-controls-after-default.txt)
      if (t.button === 'Accept') {
        scrollHold.cancel();
        committed = pending.map((binding) => binding ? [...binding] : null);
        try { preferenceStore?.setItem(CONFIGURE_CONTROLS_STORAGE_KEY, JSON.stringify(committed)); }
        catch { /* a blocked/full browser preference store does not disable the controls panel */ }
        open = false; return t;
      }
      return t;
    },
    draw(ctx, width, height) {
      if (!open) return null;
      const delta = scrollHold.poll(now());
      if (delta) scrollBy(delta);
      const scale = Math.min(width / 800, height / 600);
      ctx.save(); ctx.translate((width - 800 * scale) / 2, (height - 600 * scale) / 2); ctx.scale(scale, scale);
      const act = /ACT(\d)/i.exec(resolved()); const actNo = act ? Number(act[1]) : 1;
      const box = (b) => { if (!b) return; ctx.fillStyle = 'rgba(0,0,0,0.6)'; ctx.fillRect(b[0], b[1], b[2] - b[0], b[3] - b[1]);
        recordPrim('box', b[0], b[1], b[2], b[3], 0, 1); };
      box(L.fill);
      for (const [fr, x, y] of configureControlsPieces()) chrome.cel(ctx, actNo, BOXPIECES, fr, x, y);
      for (let i = 0; i < L.headerX.length; i++) fontFor(4).drawAnchored(ctx, ['Function', 'Key/Button One', 'Key/Button Two'][i], L.headerX[i], L.headerY);
      for (let i = 0; i < L.rows; i++) {
        const slot = slotAt(i), fn = CONFIGURE_CONTROLS_SLOTS[slot];
        if (!fn || slot >= pending.length) continue;
        // Retail emits the focus band immediately before the focused row's own text (measured on
        // the retail captures: it sits right before row 0 in the base capture and
        // right before row 1 in the focus-after-Esc one), NOT once after the header.
        if (focus && focus.slot === slot) box(focusBox());
        const b = pending[slot], y = rowY(i), armedRow = armed && armed.slot === slot;
        const isFocus = (cell) => (focus && focus.slot === slot && focus.cell === cell);
        const cell = (c) => (armedRow ? 3 : isFocus(c) ? 3 : b[c] ? 4 : (b[0] || b[1]) ? 5 : 1);
        fontFor(armedRow ? 3 : 0).drawAnchored(ctx, fn[0], L.descX, y);
        for (let c = 0; c < 2; c++) {
          if (armedRow && armed.cell === c && blinkOff()) continue;   // the armed label blinks
          fontFor(cell(c)).drawAnchored(ctx, b[c] || 'None', c ? L.key2X : L.key1X, y);
        }
      }
      for (let i = 0; i < L.slider.trough[2]; i++) chrome.cel(ctx, actNo, TEXTSLID, 13, L.slider.trough[0], L.slider.trough[1] + L.slider.step * i);
      for (let i = 0; i < L.slider.thumb[1]; i++) chrome.cel(ctx, actNo, TEXTSLID, 14, L.slider.thumb[0], thumbYForScrollIndex(scrollIndex) + L.slider.step * i);
      chrome.cel(ctx, actNo, TEXTSLID, 11, L.slider.up[0], L.slider.up[1]);
      chrome.cel(ctx, actNo, TEXTSLID, 10, L.slider.down[0], L.slider.down[1]);
      const buttonColour = { Cancel: 1, Default: 3, Accept: 2 };
      for (const [name, [x, y]] of Object.entries(L.buttons))
        fontFor(name === buttonHover ? buttonColour[name] : 4).drawAnchored(ctx, name, x, y);
      ctx.restore();
      return this.evidence;
    },
  };
}
