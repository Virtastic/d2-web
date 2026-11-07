import { test } from 'node:test';
import assert from 'node:assert/strict';
import { B, install, tap, down, up, poll, drain } from './pad-fixture.mjs';
import { gridPoints, makeRepeater, enhancePanelPoints } from '../play/assets/pad-panels.mjs';

let now = 1000;
performance.now = () => now;
const cells = (prefix, cols, rows, x0 = 100, y0 = 100, step = 30) => {
  const o = {};
  for (let y = 0; y < rows; y++) for (let x = 0; x < cols; x++) o[`${x},${y}`] = [x0 + x * step, y0 + y * step];
  return o;
};
const item = (id, gridX, gridY, invWidth, invHeight, extra = {}) => ({ id, gridX, gridY, invWidth, invHeight, invPage: 0, mode: 0, ...extra });
const shell = { screen: 'game', cursorItem: 0, snapshot: { items: [], player: { x: 0, y: 0, hpMax: 1 } }, controlCodeFor: () => null, project: () => [0, 0],
  inventoryVisible: true, uiTargets: () => ({ inventoryGrid: cells('invg', 4, 3) }) };
await install(shell);
const clicks = () => drain().filter((e) => e.startsWith('pointerdown'));
// pointerdown:x,y -- the fixture adds the +0.5 centre convention
const pt = (x, y) => `pointerdown:${x + 0.5},${y + 0.5}`;

test('a 2x2 item is one stop for the d-pad, not four', () => {
  shell.snapshot.items = [item(7, 1, 0, 2, 2)];
  drain();
  tap(B.A); assert.deepEqual(clicks(), [pt(100, 100)]);            // first stop: empty cell 0,0
  tap(B.RIGHT); tap(B.A); assert.deepEqual(clicks(), [pt(145, 115)]); // the item's footprint middle
  tap(B.RIGHT); tap(B.A); assert.deepEqual(clicks(), [pt(190, 100)]); // past it: cell 3,0 in one press
});

test('holding an item, the d-pad steps anchors of its footprint', () => {
  shell.snapshot.items = [item(7, 1, 0, 2, 2), item(9, 0, 2, 1, 1, { mode: 4, invPage: 255 })];
  shell.snapshot.items[1].invWidth = 2; shell.snapshot.items[1].invHeight = 1;
  shell.cursorItem = 9;
  drain();
  poll(); tap(B.A); assert.deepEqual(clicks(), [pt(175, 100)]);     // lifted from cell 3,0: nearest anchor, 2,0 (last that fits)
  tap(B.LEFT); tap(B.LEFT); tap(B.A); assert.deepEqual(clicks(), [pt(115, 100)]);   // anchor 0,0: middle of the 2x1 footprint
  tap(B.RIGHT); tap(B.RIGHT); tap(B.RIGHT); tap(B.A); assert.deepEqual(clicks(), [pt(360, 300)]);   // past the grid: the ground-drop strip
  shell.cursorItem = 0;
});

test('the highlight stays near where it was when the point set changes', () => {
  shell.snapshot.items = [item(7, 2, 1, 2, 2)];
  poll(); drain();
  shell.snapshot.items = [];       // item lifted: its stop becomes cells again
  poll(); tap(B.A);
  const [c] = clicks();
  assert.ok(c && !c.startsWith(pt(100, 100)), c);   // not back at the first cell
});

test('a held d-pad repeats every 150 ms after a 300 ms hold', () => {
  const r = makeRepeater();
  const log = [];
  for (let t = 0; t <= 800; t += 50) log.push(r(true, t));
  const fired = log.map((f, i) => (f ? i * 50 : null)).filter((v) => v !== null);
  assert.deepEqual(fired, [0, 300, 450, 600, 750]);
  assert.equal(r(false, 900), false);
  assert.equal(r(true, 950), true);
});

test('vendor tabs, buttons and the repair confirm are reachable; the confirm is exclusive', () => {
  const base = { shop: { 5: { at: [60, 60] } }, shopTabs: { 0: [50, 20] }, shopButtons: { buy: [70, 300], repair: [130, 300] } };
  shell.snapshot.items = [];
  shell.uiTargets = () => base;
  poll(); drain();
  tap(B.DOWN); tap(B.A);
  assert.equal(clicks().length, 1);
  shell.uiTargets = () => ({ ...base, repairConfirm: { yes: [300, 280], no: [340, 280] } });
  poll(); tap(B.RIGHT); tap(B.A);
  assert.deepEqual(clicks(), [pt(340, 280)]);
});

test('cube cells and Transmute, PlugY page buttons and the drop strip come from the layout', () => {
  const layout = { cube: { geometry: { gridLeft: 100, gridTop: 100, columns: 3, rows: 4, boxW: 29, boxH: 29 } }, stash: { plugy: true, gold: false } };
  const items = [];
  const base = [{ key: 'invg:0,0', x: 450, y: 300 }];
  const keys = (held) => enhancePanelPoints(base, {}, { items, held, layout, width: 800, height: 600 }).map((p) => p.key);
  const plain = keys(null);
  assert.ok(plain.includes('cube:transmute') && plain.includes('stashbtn:next') && plain.includes('cube:2,3'));
  assert.ok(!plain.includes('drop'));
  assert.ok(keys({ id: 1, invWidth: 1, invHeight: 1 }).includes('drop'));
});

test('belt boxes join the inventory stops', () => {
  const out = enhancePanelPoints([{ key: 'invg:0,0', x: 1, y: 1 }], { belt: [[10, 560], [40, 560]] }, { items: [], held: null, layout: null, width: 800, height: 600 });
  assert.deepEqual(out.map((p) => p.key), ['invg:0,0', 'belt:0', 'belt:1']);
});

test('gridPoints leaves empty cells alone', () => {
  const c = Object.entries(cells('invg', 2, 1)).map(([k, p]) => ({ key: `invg:${k}`, x: p[0], y: p[1] }));
  assert.deepEqual(gridPoints(c, 'invg', [], null), c);
});

test('holding the d-pad walks the grid: one step at once, then every 150 ms after 300 ms', () => {
  shell.snapshot.items = [];
  shell.uiTargets = () => ({ inventoryGrid: cells('invg', 4, 3) });
  shell.cursorItem = 0;
  poll(); for (let i = 0; i < 4; i++) tap(B.LEFT); for (let i = 0; i < 3; i++) tap(B.UP); drain();
  const idx = () => window.__d2Controller.evidence.panelIndex;
  assert.equal(idx(), 0);
  now = 5000; down(B.RIGHT);
  assert.equal(idx(), 1);                       // the press itself
  now = 5250; poll(); assert.equal(idx(), 1);   // still inside the 300 ms hold
  now = 5300; poll(); assert.equal(idx(), 2);   // first repeat
  now = 5400; poll(); assert.equal(idx(), 2);   // under 150 ms since
  now = 5450; poll(); assert.equal(idx(), 3);
  up(B.RIGHT);
});
