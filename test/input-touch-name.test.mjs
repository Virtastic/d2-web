import { test } from 'node:test';
import assert from 'node:assert/strict';
import { installTouchInput } from '../play/assets/input-touch.mjs';
import { isRetailEditBoxCharacter } from '../play/assets/charcreate-name-input.mjs';

// The phone's name box over the hero-creation screen: what it forwards to the shell, and what it shows.
// The shell here keeps the real rules the box meets -- charcreate.mjs's typeKey (retail's edit-box
// characters, 15 at most, Backspace, Enter submits) and the shell's window keydown (Enter in the game
// opens chat) -- and a browser keydown on the box runs the box's own listeners, then bubbles to the
// window unless one stopped it.

function element() {
  const classes = new Set();
  const listeners = {};
  return {
    style: {}, dataset: {}, children: [], value: '', listeners,
    classList: {
      add(name) { classes.add(name); },
      contains(name) { return classes.has(name); },
      toggle(name, on) { on ? classes.add(name) : classes.delete(name); },
    },
    addEventListener(type, fn) { (listeners[type] ??= []).push(fn); },
    fire(type, e = {}) { for (const fn of listeners[type] ?? []) fn(e); },
    appendChild(child) { this.children.push(child); return child; },
    setAttribute() {},
    querySelector() { return null; },
    focusCalls: 0, blurCalls: 0,
    focus() { this.focusCalls++; },
    blur() { this.blurCalls++; },
    getBoundingClientRect() { return { left: 0, top: 0, width: 800, height: 600 }; },
  };
}

function shellModel() {
  const shell = { screen: 'create', chat: null, created: null, name: '' };
  Object.defineProperty(shell, 'charCreateEvidence', { get: () => ({ name: shell.name, chosen: 1 }) });
  shell.keydown = (e) => {
    if (shell.screen === 'create') {
      if (e.code === 'Backspace') { shell.name = shell.name.slice(0, -1); return true; }
      if (e.code === 'Enter') { if (shell.name) { shell.created = shell.name; shell.screen = 'game'; } return true; }
      if (isRetailEditBoxCharacter(e.key) && shell.name.length < 15) { shell.name += e.key; return true; }
      return false;
    }
    if (shell.screen === 'game' && e.code === 'Enter') shell.chat = '';
    return false;
  };
  return shell;
}

function mount({ width = 800, height = 600, name = '', targets = true } = {}) {
  const canvas = element(), container = element(), head = element();
  Object.assign(canvas, { width, height });
  const documentObject = {
    head, createElement: element, addEventListener() {},
    getElementById: (id) => (id === 'game' ? canvas : id === 'touch-overlay' ? container : null),
  };
  const shell = shellModel();
  shell.name = name;
  // The create screen's own rect for its name box, in canvas pixels, under its one view transform:
  // the 800x600 scene scaled to fit and centred (charcreate.mjs view()).
  if (targets) shell.uiTargets = () => {
    const scale = Math.min(canvas.width / 800, canvas.height / 600);
    const ox = Math.round((canvas.width - 800 * scale) / 2), oy = Math.round((canvas.height - 600 * scale) / 2);
    return { screen: 'create', create: { name: [Math.round(ox + 319 * scale), Math.round(oy + 494 * scale), Math.round(220 * scale), Math.round(31 * scale)] } };
  };
  const windowObject = { matchMedia: () => ({ matches: true }), __d2Shell: shell, addEventListener() {} };
  const saved = { setInterval: globalThis.setInterval, dispatchEvent: globalThis.dispatchEvent,
    KeyboardEvent: globalThis.KeyboardEvent, shell: globalThis.__d2Shell };
  let poll = null;
  globalThis.setInterval = (cb) => { poll = cb; return 1; };
  globalThis.KeyboardEvent = class { constructor(type, init) { Object.assign(this, { type, ...init }); } };
  globalThis.__d2Shell = shell;
  // The window: the shell's keydown listener, as tapKey reaches it.
  globalThis.dispatchEvent = (e) => { if (e.type === 'keydown' && shell.keydown(e)) e.defaultPrevented = true; return true; };
  let touch;
  try { touch = installTouchInput({ documentObject, windowObject }); } finally { globalThis.setInterval = saved.setInterval; }
  const restore = () => Object.assign(globalThis, { dispatchEvent: saved.dispatchEvent, KeyboardEvent: saved.KeyboardEvent, __d2Shell: saved.shell });
  poll();   // a class is chosen: the box shows
  const box = touch.nameInput;
  // The phone keyboard edits the box's value and fires 'input'.
  const type = (value) => { box.value = value; box.fire('input', {}); };
  // A keydown in the box: its own listeners, then the window's unless stopped, then the browser's
  // default action (an inserted character) unless prevented, then keyup.
  const press = (code, key) => {
    let stopped = false, prevented = false;
    const e = { code, key, preventDefault() { prevented = true; }, stopPropagation() { stopped = true; } };
    box.fire('keydown', e);
    if (!stopped && shell.keydown(e)) prevented = true;
    if (!prevented && key.length === 1) type(box.value + key);
    box.fire('keyup', { code, key });
  };
  return { shell, box, canvas, poll, type, press, restore };
}

test('Enter in the name box creates the hero once and the game starts with the chat bar closed', () => {
  const { shell, box, type, press, restore } = mount();
  try {
    assert.equal(box.style.display, 'block', 'the box shows once a class is chosen');
    type('A'); type('Ak'); type('Aka');
    assert.equal(shell.name, 'Aka');
    press('Enter', 'Enter');
    assert.equal(shell.created, 'Aka', 'the hero is created with the typed name');
    assert.equal(shell.screen, 'game');
    assert.equal(shell.chat, null, 'the real Enter must not reach the game that just started');
    // Negative control: without the box stopping it, the same real Enter would open chat.
    shell.keydown({ code: 'Enter', key: 'Enter' });
    assert.equal(shell.chat, '', 'the model does open chat on an Enter in the game');
  } finally { restore(); }
});

test('a character the game refuses leaves the box, so Backspace there deletes the letter it shows', () => {
  const { shell, box, type, restore } = mount();
  try {
    type('A'); type('Ab');
    type('Ab\u{1F600}');   // an emoji: not one of retail's edit-box characters
    assert.equal(shell.name, 'Ab', 'the game refuses it');
    assert.equal(box.value, 'Ab', 'and the box no longer shows it');
    type('A');   // the player deletes the last thing the box shows
    assert.equal(shell.name, 'A', 'one Backspace takes one real letter');
    type('A€');   // a euro sign: outside the font too
    assert.equal(box.value, 'A');
    type('Ax');
    assert.equal(shell.name, 'Ax', 'typing carries on from what the name holds');
  } finally { restore(); }
});

test('a hardware or iOS key the shell already took shows in the box, typed once', () => {
  const { shell, box, press, restore } = mount();
  try {
    press('KeyH', 'H'); press('KeyI', 'i');
    assert.equal(shell.name, 'Hi', 'each key reached the name once');
    assert.equal(box.value, 'Hi', 'the box shows the name the keys built');
    press('Backspace', 'Backspace');
    assert.equal(shell.name, 'H');
    assert.equal(box.value, 'H');
  } finally { restore(); }
});

test('the box is never rewritten mid-composition, and shows the name once the word is done', () => {
  const { shell, box, type, restore } = mount();
  try {
    box.fire('compositionstart', {});
    type('Ab'); type('Abé́');   // a combining accent mid-word: refused by the game
    assert.equal(box.value, 'Abé́', 'the keyboard keeps its word while composing');
    box.fire('compositionend', {});
    assert.equal(shell.name, 'Abé');
    assert.equal(box.value, 'Abé', 'after composing, the box shows what the name took');
  } finally { restore(); }
});

// The box as fractions of the canvas, which the overlay matches (both fill #viewport).
const placed = (box) => ['left', 'top', 'width', 'height'].map((k) => Number.parseFloat(box.style[k]) / 100);

test('on a landscape phone the name box sits over the drawn box, not where a 4:3 screen has it', () => {
  // A wide phone: the canvas is 1333x600 and the 800x600 scene is centred in it, 267 px in from the left.
  const { box, canvas, poll, restore } = mount({ width: 1333, height: 600 });
  try {
    const [left, top, width, height] = placed(box);
    assert.ok(Math.abs(left * 1333 - (267 + 319)) < 1, `left edge on the drawn box (${left * 1333})`);
    assert.ok(Math.abs(width * 1333 - 220) < 1, 'as wide as the drawn box, not 27.5% of the screen');
    assert.ok(Math.abs(top * 600 - 494) < 1 && Math.abs(height * 600 - 31) < 1);
    // Turned to portrait: 800 columns, a taller canvas, the scene centred top to bottom.
    Object.assign(canvas, { width: 800, height: 1422 });
    poll();
    const [, top2, width2] = placed(box);
    assert.ok(Math.abs(top2 * 1422 - (411 + 494)) < 1, `follows the rotation (${top2 * 1422})`);
    assert.ok(Math.abs(width2 * 800 - 220) < 1);
  } finally { restore(); }
});

test('a screen that reports no rect leaves the stylesheet 4:3 place alone', () => {
  const { box, restore } = mount({ targets: false });
  try {
    assert.equal(box.style.display, 'block');
    assert.equal(box.style.left, undefined, 'no inline position: the 4:3 percentages apply');
  } finally { restore(); }
});

test('a name typed before the box showed is in the box when it opens', () => {
  const { shell, box, type, restore } = mount({ name: 'Ka' });
  try {
    assert.equal(box.value, 'Ka', 'the box opens on the name the game holds');
    type('K');   // one Backspace on the phone keyboard
    assert.equal(shell.name, 'K', 'deletes the last real letter');
  } finally { restore(); }
});

test('the phone keyboard opens on a tap on the box, not when the box shows, and Enter closes it', () => {
  const { shell, box, poll, type, press, restore } = mount();
  try {
    assert.equal(box.style.display, 'block');
    assert.equal(box.focusCalls, 0, 'showing the box does not focus it');
    poll(); poll();
    assert.equal(box.focusCalls, 0, 'nor does a later poll');
    box.fire('pointerdown', {});
    assert.equal(box.focusCalls, 1, 'a tap on the box focuses it');
    type('Ak');
    assert.equal(box.blurCalls, 0);
    press('Enter', 'Enter');
    assert.equal(box.blurCalls, 1, 'Enter blurs the box');
    assert.equal(shell.created, 'Ak', 'the engine still receives the typed name and Enter once');
  } finally { restore(); }
});
