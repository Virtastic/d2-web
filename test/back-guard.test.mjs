// Android Back never leaves the game: faked window and history, exact sequences of calls recorded.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { createBackGuard, installBackGuard, isBackKey, GUARD_STATE } from '../play/assets/back-guard.mjs';

function fakeWindow({ coarse = false } = {}) {
  const listeners = new Map(), log = [];
  const entries = [{ state: null }];
  let at = 0;
  const historyObject = {
    get state() { return entries[at].state; },
    get length() { return entries.length; },
    pushState(state) { entries.length = at + 1; entries.push({ state }); at++; log.push('push'); },
  };
  const windowObject = {
    history: historyObject,
    matchMedia: (q) => ({ matches: coarse && q === '(pointer: coarse)' }),
    addEventListener(type, fn) { listeners.set(type, [...(listeners.get(type) || []), fn]); },
    fire(type, event = {}) { for (const fn of listeners.get(type) || []) fn(event); },
  };
  // Back as Chrome does it: one entry down, then popstate. Off the first entry the tab would close.
  const back = () => { if (at === 0) { log.push('closed'); return; } at--; log.push('back'); windowObject.fire('popstate', { state: entries[at].state }); };
  return { windowObject, historyObject, back, log, at: () => at, entries };
}
const keyEvent = (key, code) => ({ key, code, defaultPrevented: false, preventDefault() { this.defaultPrevented = true; } });

test('armed: Back pops the guard entry and the guard puts it back; ten presses never close the tab', () => {
  const f = fakeWindow();
  const g = createBackGuard({ windowObject: f.windowObject }).arm();
  for (let i = 0; i < 10; i++) f.back();
  assert.deepEqual(f.log, ['push', ...Array(10).fill(['back', 'push']).flat()]);
  assert.equal(f.historyObject.state[GUARD_STATE], true);
  assert.deepEqual(g.evidence, { armed: true, onTop: true, absorbed: 10 });
});

test('unarmed (desktop, no controller): Back is the browser\'s', () => {
  const f = fakeWindow();
  installBackGuard({ windowObject: f.windowObject });
  f.back();
  assert.deepEqual(f.log, ['closed']);
});

test('a touch screen arms at load; a desktop arms when a controller connects', () => {
  const phone = fakeWindow({ coarse: true });
  installBackGuard({ windowObject: phone.windowObject });
  phone.back();
  assert.deepEqual(phone.log, ['push', 'back', 'push']);
  const desk = fakeWindow();
  installBackGuard({ windowObject: desk.windowObject });
  desk.windowObject.fire('gamepadconnected', {});
  desk.back();
  assert.deepEqual(desk.log, ['push', 'back', 'push']);
});

test('a trusted press re-pushes the guard when it is no longer on top; a synthetic one does not', () => {
  const f = fakeWindow();
  createBackGuard({ windowObject: f.windowObject }).arm();
  f.entries[1].state = null;   // the browser skipped or dropped our entry
  f.windowObject.fire('pointerup', { isTrusted: false });
  assert.deepEqual(f.log, ['push']);
  f.windowObject.fire('pointerup', { isTrusted: true });
  assert.deepEqual(f.log, ['push', 'push']);
  f.windowObject.fire('keydown', { isTrusted: true, key: 'a', code: 'KeyA' });
  assert.deepEqual(f.log, ['push', 'push'], 'on top already: nothing pushed');
});

test('a controller button reaching the page as an unidentified key, or the Back key, is consumed; game keys are not', () => {
  const f = fakeWindow();
  createBackGuard({ windowObject: f.windowObject });
  const cases = [['Unidentified', '', true], ['GoBack', '', true], ['BrowserBack', 'BrowserBack', true],
    ['Escape', 'Escape', false], ['Enter', 'Enter', false], ['ArrowUp', 'ArrowUp', false], ['Unidentified', 'KeyA', false], ['1', 'Digit1', false]];
  for (const [key, code, eaten] of cases) {
    const e = keyEvent(key, code);
    f.windowObject.fire('keydown', e);
    assert.equal(e.defaultPrevented, eaten, `${key}/${code}`);
    assert.equal(isBackKey(e), eaten);
  }
});

test('isBackKey: a soft keyboard or IME "Unidentified" keystroke is typing, not Back', () => {
  assert.equal(isBackKey({ key: 'Unidentified', code: '' }), true, 'a pad button with no key of its own');
  assert.equal(isBackKey({ key: 'Unidentified', code: '', keyCode: 229 }), false);
  assert.equal(isBackKey({ key: 'Unidentified', code: '', isComposing: true }), false);
  assert.equal(isBackKey({ key: 'Unidentified', code: '', target: { tagName: 'INPUT' } }), false);
  assert.equal(isBackKey({ key: 'Unidentified', code: '', target: { tagName: 'DIV', isContentEditable: true } }), false);
  assert.equal(isBackKey({ key: 'GoBack', code: '' }), true);
});
