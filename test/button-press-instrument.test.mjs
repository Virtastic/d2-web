// Instrument: every momentary button draws retail's down frame (same DC6, up+1) only while armed and held over,
// and acts only on a release over the same button. Fixed event sequences, frame records compared to the rule.
import test from 'node:test';
import assert from 'node:assert/strict';
import { BUTTON_PRESS_RULES as R, simulateButton, pressedFrameOf } from '../play/assets/button-press-rules.mjs';
import { HUD_LAYOUT, levelButtonFrame } from '../play/assets/control-panel.mjs';
import { STAT_PLUS_ART, STAT_PLUS_FRAME, STAT_PLUS_PRESSED_FRAME, STAT_PLUS_DISABLED_FRAME } from '../play/assets/character-panel.mjs';
import { panelCloseFrame } from '../play/assets/panel-close.mjs';
import { characterPanelCloseFrame } from '../play/assets/character-panel-close.mjs';
import { createWaypointMenuInput } from '../play/assets/waypoint-menu-input.mjs';

const SEQ = [ { type: 'move', over: 'b' }, { type: 'down', over: 'b' }, { type: 'move', over: 'b' }, { type: 'move', over: null },
  { type: 'move', over: 'b' }, { type: 'up', over: 'b' }, { type: 'move', over: 'b' } ];
const OFF = [ { type: 'down', over: 'b' }, { type: 'move', over: null }, { type: 'up', over: null } ];

for (const [id, rule] of Object.entries(R)) test(`${id}: ${rule.cel} up ${rule.up} -> down ${rule.down}`, () => {
  assert.equal(rule.down, rule.up + 1 === rule.down ? rule.down : -1, 'down is the frame after up');
  const r = simulateButton(rule, 'b', SEQ);
  assert.deepEqual(r.frames, [rule.up, rule.down, rule.down, rule.up, rule.down, rule.up, rule.up]);
  assert.deepEqual(r.activated, [5]);
  const off = simulateButton(rule, 'b', OFF);
  assert.deepEqual(off.frames, [rule.down, rule.up, rule.up]);
  assert.deepEqual(off.activated, [], 'release off the button does not activate');
  assert.equal(pressedFrameOf(rule, null, 'b'), rule.up);
  assert.equal(pressedFrameOf(rule, 'other', 'b'), rule.up);
});

test('rules agree with the renderers that draw them', () => {
  assert.equal(R['stat-plus'].cel.toLowerCase(), STAT_PLUS_ART.toLowerCase());
  assert.deepEqual([R['stat-plus'].up, R['stat-plus'].down, R['stat-plus'].disabled], [STAT_PLUS_FRAME, STAT_PLUS_PRESSED_FRAME, STAT_PLUS_DISABLED_FRAME]);
  const lv = HUD_LAYOUT.levelButtons;
  assert.equal(lv.path.toLowerCase(), R['level-hud'].cel.toLowerCase());
  assert.deepEqual([levelButtonFrame(0, -1), levelButtonFrame(0, 0), lv.disabledFrame], [R['level-hud'].up, R['level-hud'].down, R['level-hud'].disabled]);
  assert.equal(HUD_LAYOUT.runButton.frame, R['run-walk'].up);
  assert.equal(HUD_LAYOUT.runButton.runFrame, R['run-run'].up);
  assert.equal(HUD_LAYOUT.menuButton.openFrame, R['menu-open'].up);
  assert.equal(HUD_LAYOUT.menuButton.closedFrame, R['menu-closed'].up);
  assert.equal(HUD_LAYOUT.miniButtons.path.toLowerCase(), 'data\\global\\ui\\panel\\minipanelbtn.dc6');
  assert.deepEqual([panelCloseFrame(false), panelCloseFrame(true)], [R['panel-close'].up, R['panel-close'].down]);
  assert.deepEqual([characterPanelCloseFrame(false), characterPanelCloseFrame(true)], [R['panel-close'].up, R['panel-close'].down]);
});

test('waypoint close: only the 36x34 box is momentary; its edges are retail\'s', () => {
  const w = createWaypointMenuInput();
  for (const [x, y, inside] of [[353, 447, true], [388, 480, true], [353, 446, false], [352, 460, false], [389, 460, false], [370, 481, false]])
    assert.equal(w.inCloseRect(x, y, 800, 600), inside, `${x},${y}`);
});

// ---- Demo defect 18's remaining controls: skill-tree tabs, waypoint tabs, hire rows, message-log arrows ----------------
// Each item runs one fixed sequence (down inside, draw, move out, draw, move in, up inside) and records
// [frame, fired] at every step. RETAIL is what 1.13c D2Client does. Skill tabs come from 0x796F0 and waypoint tabs
// from 0xAA4E0: both pick the tab on the down and draw no held cel. The message-log arrows and the hire rows come
// from the list widget: down 0x679F0, up 0x68670, draw 0x67FD0. BEFORE replays the code these fixes replaced, so its
// count is the "before" figure. The ported modules must read 0.
import { skillTabAt } from '../play/assets/skill-panel.mjs';
import { createListPress, listGeometry } from '../play/assets/list-press.mjs';
import { MSG_LIST } from '../play/assets/quest-log.mjs';
import { createRealmNpcScreen } from '../play/assets/realm-npc-screen.mjs';

const STEPS = ['down', 'draw', 'out', 'draw', 'in', 'up'];
const diffs = (a, b) => a.reduce((n, r, i) => n + (JSON.stringify(r) === JSON.stringify(b[i]) ? 0 : 1), 0);
// 0x796F0 written out from the screen size W x H and the panel offsets a0 = 80, a4 = -60 at 800x600.
const retailSkillTab = (x, y, W = 800, H = 600, a0 = 80, a4 = -60) => {
  if (y > H - 0x30 || x < W - a0 - 0x58 || x > W - a0) return 0;
  if (y > H + a4 - 0x174 && y < H + a4 - 0x109) return 3;
  if (y > H + a4 - 0x108 && y < H + a4 - 0x9d) return 2;
  if (y > H + a4 - 0x9c && y < H - 0x31) return 1;
  return 0;
};
const oldSkillTab = (x, y) => { if (x >= 630 && x < 718) for (const [t, top] of [[1, 388], [2, 280], [3, 172]]) if (y >= top && y < top + 100) return t; return 0; };
const tabRun = (tabAt, inP, outP) => { let tab = 1; return STEPS.map((s) => { let fired = 0;
  if (s === 'down') { const t = tabAt(...inP); if (t) { tab = t; fired = 1; } } return [tab, fired]; }); };
const SKILL_PROBES = [];
for (const x of [630, 631, 632, 676, 717, 718, 720, 721]) for (const y of [168, 169, 170, 274, 275, 276, 277, 382, 383, 384, 385, 487, 488, 550, 551, 552])
  SKILL_PROBES.push([x, y]);

// The message log: the arrows' frames and the scroll per step, from a scroll of 3 in a 40-line log.
const msgRun = (press, inP, outP, arrow) => { let at = inP, scroll = 3; const frame = () => press.frames()[arrow];
  return STEPS.map((s) => { const before = scroll;
    if (s === 'down') press.down(...at);
    if (s === 'out') at = outP;
    if (s === 'in') at = inP;
    if (s === 'draw') { const f = frame(), r = press.tick(...at); if (r) scroll += r.scroll; return [f, scroll === before ? 0 : 1]; }
    if (s === 'up') { const r = press.up(...at); if (r?.scroll) scroll += r.scroll; }
    return [frame(), scroll === before ? 0 : 1]; }); };
// The code it replaced: the 12x13 boxes at (766,81) and (766,453), scrolled on the down, never a held frame.
const oldMsgPress = () => { let fired = 0; const box = (x, y) => (x >= 766 && x < 778 ? (y >= 81 && y < 94 ? -1 : y >= 453 && y < 466 ? 1 : 0) : 0);
  return { frames: () => ({ up: 9, down: 8 }), down(x, y) { fired = box(x, y); }, tick: () => null,
    up() { return null; }, get pending() { const f = fired; fired = 0; return f; } }; };
const oldMsgRun = (inP, outP, arrow) => { const p = oldMsgPress(); let scroll = 3;
  return STEPS.map((s) => { const before = scroll; if (s === 'down') { p.down(...inP); scroll += p.pending; } return [p.frames()[arrow], scroll === before ? 0 : 1]; }); };
const MSG_PROBES = [];
for (const x of [766, 767, 768, 777, 778, 782, 783]) for (const y of [69, 70, 80, 81, 93, 94, 441, 442, 452, 453, 465, 466]) MSG_PROBES.push([x, y]);
const retailArrow = (x, y) => (x > 767 && x < 783 ? (y > 69 && y < 81 ? 'up' : y > 441 && y < 453 ? 'down' : null) : null);
const oldArrow = (x, y) => (x >= 766 && x < 778 ? (y >= 81 && y < 94 ? 'up' : y >= 453 && y < 466 ? 'down' : null) : null);

// The hire window: the real NPC screen over a menu whose offer row 0 sits at x 50 (70 is off every row).
const hireScreen = () => { const hires = [];
  const menu = { open: false, hireOpen: false, showHire() { this.hireOpen = true; }, closeHire() { this.hireOpen = false; }, drawHire: () => ({}),
    hireAt: (x) => (x === 50 ? { row: 0 } : x === 60 ? { cancel: true } : null), close() {}, show() {}, hover() {}, outside: () => false };
  const ui = { hire: (m) => { hires.push(m); return true; }, end: () => true };
  const view = { guid: 0x55, classId: 154, mode: 'hire', entries: [], stock: [], speech: null, offers: [{ mercName: 5, name: 'A', level: 3, life: 20, def: 7, cost: 300, skill: '' }] };
  const screen = createRealmNpcScreen({ ui, view: () => view, menu, buyConfirm: null, vendor: {}, speech: {}, itemTable: {}, text: () => '', now: () => 0,
    nameOf: () => '', anchor: () => ({ x: 0, y: 0 }), gold: () => 0 });
  screen.draw({}, 800, 600);
  return { screen, hires }; };
const hireRun = (upX) => { const { screen, hires } = hireScreen(); let x = 50;
  return STEPS.map((s) => { const n = hires.length;
    if (s === 'down') screen.pointerDown(x, 10, 800, 600, 0);
    if (s === 'out') x = 70;
    if (s === 'in') x = upX;
    if (s === 'up') screen.pointerUp(x, 10, 800, 600);
    return [-1, hires.length - n]; }); };
const oldHireRun = () => STEPS.map((s) => [-1, s === 'down' ? 1 : 0]);   // the old down hired at once

const RETAIL = {
  skillTab: [[2, 1], [2, 0], [2, 0], [2, 0], [2, 0], [2, 0]],
  waypointTab: [[2, 1], [2, 0], [2, 0], [2, 0], [2, 0], [2, 0]],
  msgUp: [[11, 0], [11, 1], [11, 0], [11, 0], [11, 0], [9, 1]],
  msgDown: [[10, 0], [10, 1], [10, 0], [10, 0], [10, 0], [8, 1]],
  hireSame: [[-1, 0], [-1, 0], [-1, 0], [-1, 0], [-1, 0], [-1, 1]],
  hireOff: [[-1, 0], [-1, 0], [-1, 0], [-1, 0], [-1, 0], [-1, 0]],
};
const wp = createWaypointMenuInput();
const wpTab = (x, y) => wp.hitTest(x, y, 800, 600, [], () => null, [true, true, true, true])?.act ?? 0;
const ours = () => ({
  skillTab: tabRun(skillTabAt, [676, 330], [500, 330]),
  waypointTab: tabRun(wpTab, [176, 70], [300, 300]),
  msgUp: msgRun(createListPress(MSG_LIST), [772, 75], [772, 120], 'up'),
  msgDown: msgRun(createListPress(MSG_LIST), [772, 447], [772, 400], 'down'),
  hireSame: hireRun(50), hireOff: hireRun(70),
  skillZones: SKILL_PROBES.map(([x, y]) => skillTabAt(x, y)),
  msgZones: MSG_PROBES.map(([x, y]) => listArrowOf(x, y)),
});
const listArrowOf = (x, y) => { const p = createListPress(MSG_LIST); p.down(x, y); return p.arrow; };
const before = () => ({
  skillTab: tabRun(oldSkillTab, [676, 330], [500, 330]), waypointTab: tabRun(wpTab, [176, 70], [300, 300]),
  msgUp: oldMsgRun([772, 75], [772, 120], 'up'), msgDown: oldMsgRun([772, 447], [772, 400], 'down'),
  hireSame: oldHireRun(), hireOff: oldHireRun(),
  skillZones: SKILL_PROBES.map(([x, y]) => oldSkillTab(x, y)), msgZones: MSG_PROBES.map(([x, y]) => oldArrow(x, y)),
});
const retail = { ...RETAIL, skillZones: SKILL_PROBES.map(([x, y]) => retailSkillTab(x, y)), msgZones: MSG_PROBES.map(([x, y]) => retailArrow(x, y)) };
const count = (o) => Object.fromEntries(Object.keys(retail).map((k) => [k, diffs(retail[k], o[k])]));

test('defect 18 items: records differing from retail, before and after', () => {
  assert.deepEqual(listGeometry(5, 0x41, 780, 400), { x: 5, y: 65, w: 780, h: 400, ax: 767, ay: 81, cels: 31 });
  const b = count(before()), a = count(ours());
  console.log('defect-18 before', JSON.stringify(b), 'after', JSON.stringify(a));
  assert.deepEqual(a, Object.fromEntries(Object.keys(retail).map((k) => [k, 0])));
});
