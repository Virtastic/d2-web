import { test } from 'node:test';
import assert from 'node:assert/strict';
import { installTouchInput } from '../play/assets/input-touch.mjs';

function element() {
  const classes = new Set();
  return {
    style: {}, dataset: {}, children: [],
    classList: {
      add(name) { classes.add(name); },
      contains(name) { return classes.has(name); },
      toggle(name, on) { on ? classes.add(name) : classes.delete(name); },
    },
    addEventListener() {},
    appendChild(child) { this.children.push(child); return child; },
    setAttribute() {},
    querySelector() { return null; },
    focus() {},
    blur() {},
    getBoundingClientRect() { return { left: 0, top: 0, width: 800, height: 600 }; },
  };
}

function mount(windowExtra = {}) {
  const canvas = element();
  const container = element();
  const head = element();
  const documentObject = {
    head,
    getElementById(id) {
      if (id === 'game') return canvas;
      if (id === 'touch-overlay') return container;
      return null;
    },
    createElement: element,
    addEventListener() {},
  };
  const windowObject = {
    matchMedia: () => ({ matches: true }),
    __d2Shell: { screen: 'title' },
    ...windowExtra,
  };
  return { canvas, container, documentObject, windowObject };
}

// Run installTouchInput with its polling interval captured instead of started.
function withPoll(fn) {
  const realSetInterval = globalThis.setInterval;
  let poll = null, period = null;
  globalThis.setInterval = (cb, ms) => { poll = cb; period = ms; return 1; };
  try { return fn(() => poll(), () => period); } finally { globalThis.setInterval = realSetInterval; }
}

test('a coarse pointer reveals the touch layer, and there is no virtual stick or key bar to show', () => {
  const { container, documentObject, windowObject } = mount();
  withPoll(() => {
    const touch = installTouchInput({ documentObject, windowObject });
    assert.ok(container.classList.contains('d2-touch-on'), 'coarse pointer reveals the touch input layer');
    assert.equal(touch.stickBase, undefined, 'the virtual stick is gone');
    assert.equal(touch.bar, undefined, 'the button bar is gone');
    assert.equal(touch.shiftButton, undefined, 'the shift button is gone');
    assert.equal(typeof touch.isShiftArmed, 'function');
    assert.equal(touch.isShiftArmed(), false, 'no rest finger is down at start');
  });
});

test('the only page element the layer adds is the hero-name input', () => {
  const { container, documentObject, windowObject } = mount();
  withPoll(() => {
    const touch = installTouchInput({ documentObject, windowObject });
    assert.equal(container.children.length, 1, 'one child: the name input');
    assert.equal(container.children[0], touch.nameInput);
    assert.equal(touch.nameInput.id, 'd2-touch-name');
    assert.equal(touch.nameInput.style.display, 'none', 'hidden until the name box is due');
  });
});

test('the poll drives only the name box: nothing shows on the title or game screen', () => {
  const shell = { screen: 'title' };
  const { container, documentObject, windowObject } = mount({ __d2Shell: shell });
  withPoll((tick) => {
    const touch = installTouchInput({ documentObject, windowObject });
    tick();
    assert.equal(touch.nameInput.style.display, 'none');
    shell.screen = 'game';
    tick();
    assert.equal(touch.nameInput.style.display, 'none', 'the game screen has no name box');
    assert.equal(container.children.length, 1, 'no chrome is added by the poll');
  });
});

test('the name box shows on the create screen once a class is chosen, and hides when it is not', () => {
  const shell = { screen: 'create', charCreateEvidence: { chosen: null } };
  const { documentObject, windowObject } = mount({ __d2Shell: shell });
  withPoll((tick, period) => {
    const touch = installTouchInput({ documentObject, windowObject });
    assert.equal(period(), 200, 'the poll period is unchanged');
    tick();
    assert.equal(touch.nameInput.style.display, 'none', 'no class chosen yet');
    shell.charCreateEvidence.chosen = 2;
    tick();
    assert.equal(touch.nameInput.style.display, 'block', 'a chosen class shows the name box');
    shell.charCreateEvidence.chosen = null;
    tick();
    assert.equal(touch.nameInput.style.display, 'none', 'and it hides again without one');
  });
});
