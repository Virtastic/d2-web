import test from 'node:test';
import assert from 'node:assert/strict';
import { createTouchTextEntry } from './touch-text-entry.mjs';

// A document with one <input>: listeners, value, focus and blur, enough to drive the events a phone sends.
function rig({ limit = 8, refuse = '' } = {}) {
  const listeners = {}; let active = null, text = '';
  const input = { style: {}, value: '', setAttribute() {},
    addEventListener(t, f) { (listeners[t] ??= []).push(f); },
    focus() { active = input; }, blur() { if (active === input) { active = null; fire('blur'); } } };
  const fire = (type, init = {}) => { const e = { type, preventDefault() { this.prevented = true; }, ...init }; for (const f of listeners[type] ?? []) f(e); return e; };
  const doc = { createElement: () => input, body: { appendChild() {} }, get activeElement() { return active; } };
  const calls = []; let blurs = 0, enters = 0;
  const entry = createTouchTextEntry({ documentObject: doc,
    typeKey: (code, key) => {
      calls.push(`${code}|${key}`);
      if (code === 'Enter') { enters++; text = ''; }
      else if (code === 'Backspace') text = text.slice(0, -1);
      else if (key.length === 1 && !refuse.includes(key) && text.length < limit) text += key;
    },
    getValue: () => text, onBlur: () => blurs++ });
  const type = (v, ev = 'input') => { input.value = v; return fire(ev); };
  return { entry, input, fire, type, calls, get text() { return text; }, set text(v) { text = v; }, get blurs() { return blurs; }, get enters() { return enters; }, doc };
}

test('plain typing: each added character and each deletion is one key call', () => {
  const r = rig(); r.entry.open();
  r.type('h'); r.type('hi'); r.type('h');
  assert.deepEqual(r.calls, ['|h', '|i', 'Backspace|Backspace']); assert.equal(r.text, 'h');
});

test('composition: nothing is sent until compositionend, then the composed text arrives once', () => {
  const r = rig(); r.entry.open();
  r.fire('compositionstart'); r.type('h'); r.type('he'); r.type('hel');
  assert.deepEqual(r.calls, []);
  r.type('hello'); r.fire('compositionend');
  assert.deepEqual(r.calls, ['|h', '|e', '|l', '|l', '|o']);
  r.type('hello'); assert.equal(r.calls.length, 5, 'a trailing input event with the same text adds nothing');
});

test('autocorrect replaces a word: Backspace for what went, then the new characters', () => {
  const r = rig(); r.entry.open(); r.type('teh');
  r.calls.length = 0; r.type('the');
  assert.deepEqual(r.calls, ['Backspace|Backspace', 'Backspace|Backspace', '|h', '|e']); assert.equal(r.text, 'the');
});

test('the screen has the last word: a refused character or a limit leaves the input showing the screen text', () => {
  const r = rig({ limit: 3, refuse: 'z' }); r.entry.open();
  r.type('az'); assert.equal(r.input.value, 'a');
  r.type('abcd'); assert.equal(r.input.value, 'abc');
});

test('Enter key: one Enter call, the input cleared from the screen text', () => {
  const r = rig(); r.entry.open(); r.type('hi');
  const e = r.fire('keydown', { key: 'Enter', code: 'Enter', timeStamp: 10 });
  assert.equal(e.prevented, true); assert.equal(r.enters, 1); assert.equal(r.input.value, ''); assert.equal(r.calls.at(-1), 'Enter|Enter');
});

test('Enter as a beforeinput line break (a keyboard sending key Unidentified) is an Enter', () => {
  const r = rig(); r.entry.open(); r.type('hi');
  r.fire('keydown', { key: 'Unidentified', code: '', timeStamp: 5 });
  const e = r.fire('beforeinput', { inputType: 'insertLineBreak', timeStamp: 6 });
  assert.equal(e.prevented, true); assert.equal(r.enters, 1);
});

test('Enter reported both ways within one press counts once', () => {
  const r = rig(); r.entry.open(); r.type('hi');
  r.fire('keydown', { key: 'Enter', code: 'Enter', timeStamp: 100 });
  r.fire('beforeinput', { inputType: 'insertLineBreak', timeStamp: 101 });
  assert.equal(r.enters, 1);
  r.fire('beforeinput', { inputType: 'insertLineBreak', timeStamp: 900 });
  assert.equal(r.enters, 2, 'a later press is another Enter');
});

test('Enter during a composition commits the composed text first', () => {
  const r = rig(); r.entry.open();
  r.fire('compositionstart'); r.type('hey');
  r.fire('keydown', { key: 'Enter', code: 'Enter', timeStamp: 1 });
  assert.deepEqual(r.calls, ['|h', '|e', '|y', 'Enter|Enter']);
});

test('blur mid-composition flushes the composed text, then reports the blur', () => {
  const r = rig(); r.entry.open();
  r.fire('compositionstart'); r.type('abc');
  r.entry.close();
  assert.deepEqual(r.calls, ['|a', '|b', '|c']); assert.equal(r.blurs, 1);
  r.type('abcd'); assert.equal(r.calls.at(-1), '|d', 'composition state is reset, typing works after refocus');
});

test('open() seeds the input from the screen text and refuses when nothing takes text', () => {
  const r = rig(); r.text = 'old'; r.entry.open(); assert.equal(r.input.value, 'old');
  r.type('olds'); assert.deepEqual(r.calls, ['|s']);
  const none = createTouchTextEntry({ documentObject: r.doc, typeKey() {}, getValue: () => null });
  assert.equal(none.open(), false);
});

test('a non-BMP character is one unit: removing it is one Backspace', () => {
  const r = rig({ refuse: '' }); r.entry.open(); r.text = 'a\u{1F600}'; r.entry.open();
  r.calls.length = 0; r.type('a');
  assert.deepEqual(r.calls, ['Backspace|Backspace']);
});
