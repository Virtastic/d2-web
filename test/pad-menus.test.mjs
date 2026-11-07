import { test } from 'node:test';
import assert from 'node:assert/strict';
import { B, pad, install, tap, down, up, poll, drain, keyTaps } from './pad-fixture.mjs';
import { createRepeater, backAction, padLayout, glyph, hintLine, chatCellCode } from '../play/assets/pad-menus.mjs';

let t = 1000;
performance.now = () => t;
const wait = (ms) => { t += ms; poll(); };
const shell = { screen: 'select', uiTargets: () => targets, charCreateEvidence: {} };
let targets = {};
await install(shell);
const events = () => drain().filter((e) => !e.startsWith('pointermove'));

test('repeater: one step on press, then 400 ms later, then every 150 ms', () => {
  const r = createRepeater();
  const fired = [];
  for (let ms = 0; ms <= 1000; ms += 50) fired.push(r.fire('d', true, ms));
  assert.deepEqual(fired.flatMap((f, i) => (f ? [i * 50] : [])), [0, 400, 550, 700, 850, 1000]);
  assert.equal(r.fire('d', false, 1050), false);
  assert.equal(r.fire('d', true, 1100), true, 'a release resets the first-press delay');
});

test('a held d-pad walks the character-select list at the repeat rate; a tap is one step', () => {
  targets = { select: { characters: {}, rows: {}, play: [100, 500], create: [300, 500], exit: [500, 500] } };
  const at = () => globalThis.__d2Controller.evidence.cursor.x;
  poll(); drain();
  down(B.RIGHT);
  assert.equal(at(), 300, 'the press moves one step');
  for (let i = 0; i < 7; i++) wait(50);
  assert.equal(at(), 300, 'nothing more before 400 ms');
  wait(50);
  assert.equal(at(), 500, 'the first repeat lands at 400 ms');
  up(B.RIGHT);
  t += 1000;
  tap(B.LEFT);
  assert.equal(at(), 300, 'a tap is one step');
});

test('B on character select is EXIT; with the delete dialog up it is NO', () => {
  targets = { select: { play: [100, 500], create: [300, 500], exit: [500, 500], DELETE_NO: [250, 300] } };
  poll(); drain();
  tap(B.B);
  assert.deepEqual(events(), ['pointerdown:250.5,300.5', 'pointerup:250.5,300.5']);
  targets = { select: { play: [100, 500], create: [300, 500], exit: [500, 500] } };
  poll(); drain();
  tap(B.B);
  assert.deepEqual(events(), ['pointerdown:500.5,500.5', 'pointerup:500.5,500.5']);
});

test('backAction chooses the route per screen', () => {
  const tg = { select: { exit: [5, 6], DELETE_NO: [7, 8] }, create: { exit: [9, 10] } };
  assert.deepEqual(backAction(tg, 'select:exit'), { click: [7, 8] });
  assert.deepEqual(backAction({ select: { exit: [5, 6] } }, 'select:play'), { click: [5, 6] });
  assert.deepEqual(backAction(tg, 'create:amazon'), { click: [9, 10] });
  for (const k of ['gamemenu:RETURN', 'npcmenu:cancel', 'waypoint:row0', 'quest:close', 'char:str', 'credits:x', 'titleGateways:a']) assert.deepEqual(backAction(tg, k), { key: 'Escape' }, k);
  assert.equal(backAction(tg, 'invg:0,0'), null, 'item grids keep B as the right click');
  assert.equal(backAction(tg, 'title:SINGLE PLAYER'), null);
});

test('glyphs follow the controller family', () => {
  assert.equal(padLayout('054c-0ce6-Wireless Controller (STANDARD GAMEPAD Vendor: 054c Product: 0ce6)'), 'playstation');
  assert.equal(padLayout('Xbox 360 Controller (STANDARD GAMEPAD Vendor: 045e Product: 028e)'), 'xbox');
  assert.equal(padLayout('Pro Controller (STANDARD GAMEPAD Vendor: 057e Product: 2009)'), 'switch');
  assert.equal(padLayout('Standard Gamepad'), 'xbox');
  assert.equal(hintLine('playstation', [[0, 'Select'], [1, 'Delete'], [3, 'Hide']]), 'Cross Select   Circle Delete   Triangle Hide');
  assert.equal(hintLine('xbox', [[0, 'Select'], [1, 'Delete']]), 'A Select   B Delete');
  assert.equal(glyph('switch', 0), 'B');
});

test('chat cells are the keys the shell chat handler takes', () => {
  assert.deepEqual(chatCellCode('h'), ['KeyH', 'h']);
  assert.deepEqual(chatCellCode('7'), ['Digit7', '7']);
  assert.deepEqual(chatCellCode(' '), ['Space', ' ']);
  assert.deepEqual(chatCellCode('OK'), ['Enter', 'Enter']);
});

test('chat bar open: A types the highlighted letter, B is Backspace, Start closes it', () => {
  shell.screen = 'game'; shell.chatOpen = true; targets = {};
  poll(); drain();
  tap(B.A); tap(B.RIGHT); tap(B.A); tap(B.B); tap(B.DOWN); tap(B.A);
  assert.deepEqual(events(), [...keyTaps('KeyA'), ...keyTaps('KeyB'), ...keyTaps('Backspace'), ...keyTaps('KeyL')]);
  tap(B.START);
  assert.deepEqual(events(), keyTaps('Escape'));
  shell.chatOpen = false;
});

test('waypoint menu: d-pad snaps over its rows and tabs, A presses, B closes with Escape', () => {
  shell.screen = 'game'; shell.chatOpen = false;
  targets = { waypoint: { tab1: [100, 80], tab2: [140, 80], row0: [200, 150], row1: [200, 200], close: [400, 500] } };
  poll(); drain();
  tap(B.DOWN); tap(B.A);
  assert.deepEqual(events(), ['pointerdown:200.5,150.5', 'pointerup:200.5,150.5'], 'down from tab 1 lands on row 0');
  tap(B.DOWN); tap(B.A);
  assert.deepEqual(events(), ['pointerdown:200.5,200.5', 'pointerup:200.5,200.5'], 'down again lands on row 1');
  tap(B.B);
  assert.deepEqual(events(), keyTaps('Escape'));
});

test('quest log: B closes with Escape', () => {
  targets = { quest: { tab1: [100, 80], quest1: [150, 200], close: [300, 480] } };
  poll(); drain();
  tap(B.B);
  assert.deepEqual(events(), keyTaps('Escape'));
});

test('the left stick steps the character-select list like the d-pad: one step per push, repeating while held', () => {
  targets = { select: { characters: {}, rows: {}, play: [100, 500], create: [300, 500], exit: [500, 500] } };
  const at = () => globalThis.__d2Controller.evidence.cursor.x;
  const stick = (x, y = 0) => { pad.axes[0] = x; pad.axes[1] = y; poll(); };
  t += 1000; stick(0); drain();
  const start = at();
  stick(1);
  assert.notEqual(at(), start, 'pushing right moves one step');
  const first = at();
  wait(50);
  assert.equal(at(), first, 'nothing more before the repeat delay');
  stick(0);
  t += 1000;
  stick(0.3);
  assert.equal(at(), first, 'a light push under half travel is not a step');
  stick(0);
});
