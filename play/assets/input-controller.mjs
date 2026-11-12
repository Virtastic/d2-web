// Gamepad support. A controller is an INPUT LAYER: it produces the same DOM pointer and key events a
// mouse and a keyboard produce (input-common.mjs), so the shell's own handlers issue the same engine
// commands. Nothing about the game is re-implemented here. Polled on a timer, never requestAnimationFrame.
//
// The scheme follows Diablo II: Resurrected's, over the original 1.13c panels (no new UI):
//   * A/B act on the picked world target; the d-pad snaps a highlight across the points the shell
//     reports for each open panel or menu;
//   * left stick walks; L3 toggles run/walk (one press); R3 swaps weapons; LB held shows items; the d-pad is the four belt slots;
//   * X, Y, RB (world) and RT are skill hotkeys 2, 4, 5, 6, and HOLDING LT banks them to 1, 3, 7, 8 (D2R's developers put the
//     second bank on the left trigger; LT does nothing on its own);
//   * HOLDING LT also banks the d-pad to the four panels (Up Inventory, Left Character, Right Skill Tree, Down Quest Log)
//     and Select to the Skill Speed Bar (LT_BANK): one index finger holds LT while the left thumb stays on the d-pad,
//     which a handheld (Retroid Pocket) needs -- its Select sits under the same thumb as the d-pad, so a Select chord
//     with the d-pad cannot be pressed. LT is the least-used modifier D2R has: alone it does nothing, and D2R gives the
//     d-pad no second bank, so nothing D2R binds is lost (the plain d-pad stays the belt);
//   * the right stick moves retail's own pointer in the game (world, panels, the game menu) and A/B click where it is,
//     so every on-screen button retail draws (the mini-panel, the skill icons, the run button) is reachable; the
//     left stick or the d-pad in a panel hands control back to the target picker / the snap highlight;
//   * HOLDING Select (Back) banks the d-pad and face buttons to the panel and utility functions (MENU_BANK) -- kept
//     as a backup: alone, Select is still the automap on release.
// Every key is pressed by the function the player's own Configure Controls table binds it to
// (shell.controlCodeFor), falling back to retail's default only when the shell cannot answer.
//
// Standard mapping (https://w3c.github.io/gamepad/#remapping); an unmapped pad (Android's built-in pads) is read the same way (pad-pick.mjs):
//   left stick (0,1) walk | right stick (2,3) the pointer in the game; a free cursor on the other screens nothing snaps on
//   A (0) left click | B (1) right click | X (2) Y (3) RB (5, world) RT (7): Skill 2, 4, 5, 6; LT held: 1, 3, 7, 8
//   LB (4) Show Items, held | RB (5) Shift, held | LT (6) skill + panel bank, held | Back (8, Select) Automap on tap, backup bank held
//   Start (9) Escape | L3 (10) run/walk on a press | R3 (11) swap weapons | d-pad (12-15) belt 1-4 clockwise from Up
// The client's two typed fields (the create screen's name box and the stash's gold amount box) are
// worked with a grid: the d-pad moves the highlight, A types the highlighted cell.

import { OVERLAY_FONT } from './input-touch.mjs';
import { click, pointer, key, tapKey, playerOnCanvas, stickTarget } from './input-common.mjs';
import { rankTargets, pickTarget, goldInReach } from './pad-targeting.mjs';
import { enhancePanelPoints, makeRepeater, nearestIndex, quickMovePlan, beltFillPlan, makeHold, equipHeld } from './pad-panels.mjs';
import { makeTargetHold, STAT_HOLD_FIRST_MS, STAT_HOLD_EVERY_MS } from './pad-hold.mjs';
import { modalPointerOwner } from './modal-pointer-priority.mjs';
import { adjustedPad, createRunToggle, createSettings, createSettingsPanel } from './pad-settings.mjs';
import { pickPad } from './pad-pick.mjs';
import { createReportReader } from './pad-report.mjs';
import { backAction, CHAT_ROWS, chatCellCode, padLayout, hintLine } from './pad-menus.mjs';
import { makeSkillMapper } from './pad-skill-mapper.mjs';
import { makeSkillPlay } from './pad-skill-play.mjs';
import { createSkillOverlay } from './pad-skill-overlay.mjs';

const POLL_MS = 50;          // setInterval, never requestAnimationFrame -- see header
const MENU_REPEAT_FIRST_MS = 400; // menus and typed-field grids; item-grid panels keep makeRepeater's 300
const STICK_WALK_MS = 250;   // a mouse user's own repeat rate for a held click-to-walk
const CURSOR_SPEED = 14;     // canvas px per poll at full right-stick deflection
// How far this D2R-style "no cursor" targeting model reaches from the player. It is a stand-in for
// D2R's own screen-space cone (PS Blog: "a large cone", per-class weighted) until its numbers are
// recovered. It is a controller policy; F14 pointer hover uses a separate hit-candidate comparator.

const BUTTON = { A: 0, B: 1, X: 2, Y: 3, LB: 4, RB: 5, LT: 6, RT: 7, BACK: 8, START: 9, L3: 10, R3: 11, DPAD_UP: 12, DPAD_DOWN: 13, DPAD_LEFT: 14, DPAD_RIGHT: 15 };
// A pad key is "the retail function, else its default code": [function, default code, key name].
const CODE_KEY = { Space: ' ', Enter: 'Enter', Tab: 'Tab', Backquote: '`', Minus: '-', Equal: '=', Backspace: 'Backspace', Escape: 'Escape' };
export const keyNameForCode = (code) => (/^Key[A-Z]$/.test(code) ? code[3].toLowerCase() : /^Digit\d$/.test(code) ? code[5] : /^F\d{1,2}$/.test(code) ? code : CODE_KEY[code] || '');
const FN = {
  belt1: ['Use Belt 1', 'Digit1', '1'], belt2: ['Use Belt 2', 'Digit2', '2'], belt3: ['Use Belt 3', 'Digit3', '3'], belt4: ['Use Belt 4', 'Digit4', '4'],
  inventory: ['Inventory Screen', 'KeyI', 'i'], character: ['Character Screen', 'KeyC', 'c'], skillTree: ['Skill Tree', 'KeyT', 't'],
  quest: ['Quest Log', 'KeyQ', 'q'], message: ['Message Log', 'KeyM', 'm'], hireling: ['Hireling Screen', 'KeyO', 'o'], help: ['Help Screen', 'KeyH', 'h'],
  swap: ['Swap Weapons', 'KeyW', 'w'], rightSkill: ['Skill Speed Bar', 'KeyS', 's'], showBelt: ['Show Belt', 'Backquote', '`'],
  chat: ['Chat', 'Enter', 'Enter'], clear: ['Clear Screen', 'Space', ' '], automap: ['Automap', 'Tab', 'Tab'], run: ['Toggle Run/Walk', 'KeyR', 'r'],
  showItems: ['Show Items', 'AltLeft', 'Alt'], party: ['Party Screen', 'KeyP', 'p'],
};
for (let n = 1; n <= 8; n++) FN[`skill${n}`] = [`Skill ${n}`, `F${n}`, `F${n}`];
// D-pad direction to belt slot, clockwise from Up.
const DPAD_BELT = { [BUTTON.DPAD_UP]: 'belt1', [BUTTON.DPAD_RIGHT]: 'belt2', [BUTTON.DPAD_DOWN]: 'belt3', [BUTTON.DPAD_LEFT]: 'belt4' };
// Skill hotkeys: three buttons, six slots with LT held.
// D2R's Help screen numbers the buttons A=1 X=2 B=3 Y=4 RB=5 RT=6; A and B stay the left and right click here, so
// X, Y, RB (in the world only: over panels RB stays the shift-click modifier) and RT press Skill 2, 4, 5, 6.
// LT held banks the same four to Skill 1, 3, 7, 8 so every F1-F8 hotkey stays reachable.
const SKILL_BUTTONS = [BUTTON.X, BUTTON.Y, BUTTON.RB, BUTTON.RT];
const SKILL_NORMAL = [2, 4, 5, 6], SKILL_LT = [1, 3, 7, 8];
// LT held, in the game: the d-pad opens the four panels and Select the Skill Speed Bar.
const LT_BANK = {
  [BUTTON.DPAD_UP]: 'inventory', [BUTTON.DPAD_LEFT]: 'character', [BUTTON.DPAD_RIGHT]: 'skillTree', [BUTTON.DPAD_DOWN]: 'quest',
  [BUTTON.BACK]: 'rightSkill',
};
const DPAD_BANK = [BUTTON.DPAD_UP, BUTTON.DPAD_LEFT, BUTTON.DPAD_RIGHT, BUTTON.DPAD_DOWN];
// Back (Select) held, the backup: panels and the keys that have no click road in the world.
const MENU_BANK = {
  [BUTTON.DPAD_UP]: 'inventory', [BUTTON.DPAD_LEFT]: 'character', [BUTTON.DPAD_RIGHT]: 'skillTree', [BUTTON.DPAD_DOWN]: 'quest',
  [BUTTON.X]: 'hireling', [BUTTON.Y]: 'message', [BUTTON.A]: 'swap', [BUTTON.B]: 'rightSkill', [BUTTON.RT]: 'chat',
  [BUTTON.L3]: 'showBelt', [BUTTON.R3]: 'help', [BUTTON.LB]: 'clear',
};

// ------------- D2R targeting: no free cursor in the world -------------
//
// D2R's own devs (PlayStation Blog, 2021-08-13): "we are always scanning the playfield with a large
// cone, and prioritizing targets at many levels -- monsters, items, objects you can interact with,
// other players, your corpse... the priorities are tuned per class." Per-class tuning is not built
// (no source gives the exact weights yet). The monster tier reuses the browser's selectPrimaryTarget
// nearest-world helper; its mouse path calls that helper with a pointer-derived point and hit-test
// filter, while this controller model searches around the player. The ordered controller tiers and
// their player-centered TARGET_RADIUS/OBJECT_RADIUS are design choices, not F14 hover rules or retail
// measurements. The caller projects the selected target and dispatches a normal canvas click through
// the shell's existing click path; that wiring does not make the controller choice equivalent to the
// mouse-hover winner.
export function selectWorldTarget(shell, aim = null, cycle = 0) {
  if (!shell?.snapshot?.player || !shell.project) return null;
  const t = pickTarget(rankTargets(shell.snapshot, { aim, project: shell.project }), cycle);
  return t && { kind: t.kind, id: t.id, x: t.x, y: t.y, screen: t.screen };
}

// ------------- D2R snap-to-slot navigation -------------
//
// Replaces the pointer cursor on the six panels the task names (inventory, stash, vendor, character,
// skill tree -- the cube has no client panel yet, a gap, not a road to build against) AND, since
// G16, on the title, character-select, escape (game) and NPC menus -- the screens G15 left as an
// "open gap, not a regression" because no D2R source named a road for them. Every point comes from
// `__d2Shell.uiTargets()`, the SAME canvas points the shell already computes by probing its OWN hit
// tests (gameplay-shell.mjs, title-screen.mjs, charselect.mjs, game-menu.mjs, npc-menu.mjs) -- so a
// slot the pad lands on is a slot the shell's click handler will resolve to the identical thing,
// never a second, gamepad-only geometry.
export function panelPoints(targets) {
  if (!targets) return null;
  // `at(v)` may hand back a non-point value (charselect's `targets.select` flattens `characters` and
  // `rows` sub-maps alongside its own [x,y] entries) -- skipped, not destructured, so one generic
  // walk serves every target map this function reads without a per-screen filter list.
  const entries = (obj, prefix, at = (v) => v) => Object.entries(obj || {})
    .map(([k, v]) => { const p = at(v); return Array.isArray(p) ? { key: `${prefix}:${k}`, x: p[0], y: p[1] } : null; })
    .filter(Boolean);
  // Every grid the shell shows AT ONCE is one navigable set, not a priority pick: a vendor screen
  // shows the shop's stock beside the player's own carried-item grid (retail's own SELL side), and a
  // pad-only player has to reach both without a second button to switch between them, exactly as the
  // free cursor could roam either. Character and skill-tree stay the fallback -- retail never shows
  // either beside a grid, so they never need to merge with one.
  // The waypoint menu is modal over the world: its tabs, rows and close box are the whole panel.
  if (targets.waypoint && Object.keys(targets.waypoint).length) return entries(targets.waypoint, 'waypoint');
  const pts = [
    ...entries(targets.shop, 'shop', (v) => v.at),
    ...entries(targets.stashGrid, 'stash'),
    ...entries(targets.stashGold, 'stashgold'),
    ...entries(targets.inventoryGrid, 'invg'),
    ...entries(targets.inventorySlots, 'invs'),
    ...entries(targets.shopTabs, 'shoptab'),
    ...entries(targets.shopButtons, 'shopbtn'),
  ];
  // Retail's skill popup (left or right hand) over the world or a panel: its icons are the whole panel, so the d-pad
  // reaches every skill, A picks it and X/Y/RB/RT over it assign that skill to the hotkey (the shell's F-key rule).
  if (targets.skillSelect?.skills && Object.keys(targets.skillSelect.skills).length) return entries(targets.skillSelect.skills, 'skillsel');
  // The single-item Repair confirm is modal over everything: Yes / No only.
  if (targets.repairConfirm) return entries(targets.repairConfirm, 'repair');
  if (pts.length) return pts;
  // Character creation exposes the class hit boxes and EXIT target. Once a class is chosen,
  // nameGridOpen() takes over d-pad/A for text entry.
  if (targets.create) {
    const classes = entries(targets.create.classes, 'create');
    if (classes.length) return [...classes, ...entries({ exit: targets.create.exit }, 'create')];
  }
  if (targets.character && Object.keys(targets.character).length) return entries(targets.character, 'char');
  if (targets.skills && Object.keys(targets.skills).length) return entries(targets.skills, 'skill');
  if (targets.quest && Object.keys(targets.quest).length) return entries(targets.quest, 'quest');
  if (targets.titleGateways && Object.keys(targets.titleGateways).length) return entries(targets.titleGateways, 'titleGateways');
  for (const k of ['cinematics', 'otherMultiplayer', 'credits']) if (targets[k] && Object.keys(targets[k]).length) return entries(targets[k], k);
  // G16: the screens that were still the pre-D2R free cursor. Each is its own screen (never shown
  // together), so the order below is only which one wins if `uiTargets()` ever reported two at once.
  if (targets.gameMenu && Object.keys(targets.gameMenu).length) return entries(targets.gameMenu, 'gamemenu');
  if (targets.npcMenu && Object.keys(targets.npcMenu).length) return entries(targets.npcMenu, 'npcmenu');
  if (targets.title && Object.keys(targets.title).length) return entries(targets.title, 'title');
  if (targets.select && Object.keys(targets.select).length) return entries(targets.select, 'select');
  // Character creation's class list already exposes the centre of each class's own hit box.
  // The name grid takes over once a class is chosen; before that, use those same click points
  // for D2R-style d-pad navigation instead of falling back to a free cursor (G17).
  const classes = entries(targets.create?.classes, 'create');
  if (classes.length) return classes;
  return null;
}

// One d-pad press moves the highlight to whichever OTHER point lies most nearly in the pressed
// direction -- the "snap" in snap-to-slot: a real 2-D grid (dx,dy from cellAt) and an irregular list
// (character's four +buttons) are the same problem to this function, so one implementation serves
// every panel `panelPoints` names. Direction is a unit-ish vector (-1/0/1 per axis, from the d-pad).
export function snapMove(points, index, dx, dy) {
  if (!points.length) return 0;
  const cur = points[Math.max(0, Math.min(points.length - 1, index))];
  let best = -1, bestScore = Infinity;
  points.forEach((p, i) => {
    if (p === cur) return;
    const vx = p.x - cur.x, vy = p.y - cur.y, along = vx * dx + vy * dy;
    if (along <= 0) return;   // behind the pressed direction
    const perp = Math.abs(vx * dy - vy * dx);
    const score = along + perp * 3;   // prefer the closest thing roughly ON the pressed axis
    if (score < bestScore) { bestScore = score; best = i; }
  });
  return best >= 0 ? best : index;
}

// ------------- The pad's two text fields: one grid, two layouts (G7, G13) -------------
//
// A gamepad has no letters and no digits, so a pad-only player cannot fill a text field the mouse road
// fills by typing. The client has exactly two such fields, and both are typed with the keyboard alone:
//
//   * the create screen's NAME box (G7). s000's flow types the hero's name there (s000a step 3:
//     `__s000.key('KeyE','E')` ...), which is the first step of the retail flow a controller-only
//     player cannot take -- a gamepad has no letters. charcreate.mjs's own typeKey accepts A-Z, '-'
//     and '_', 15 characters long.
//   * the stash's GOLD AMOUNT box (G13). Its road is retail's: digits `0`-`9` typed at it, Enter to
//     submit, Backspace to correct (gameplay-shell.mjs's modal branch). The dialog also draws up and
//     down arrows, and those step ONE gold per activation -- OpenDiablo2's move_gold_panel.go has
//     `increase()`/`decrease()` as `currentValue ± 1`, so that step is retail's and changing it to
//     make the pad's life easier would be a parity defect. The dialog opens at the whole balance, so
//     the arrows reach "all of it" and nothing in between: before this, a pad-only player could not
//     deposit or withdraw a chosen amount at all. Measured, not assumed -- s428's first arm parks the
//     cursor on the coin and the amount box, and its own log shows the amount stuck at `available`.
//
// Both fields are the same problem, so they get the same mechanism: a grid of exactly the characters
// the field's own handler accepts, plus a DEL and an OK cell, shown while that field is up (both open
// signals are the shell's own polled evidence -- `charCreateEvidence.chosen`, which input-touch.mjs's
// issue 8 input already reads, and `stashEvidence.gold.drawn.modal` -- so the hot shared gameplay-shell.mjs
// needs no hook for either), the d-pad moving the highlight and A committing the highlighted cell.
// Y hides the grid so the cursor can reach whatever is under it again (another class, EXIT, the
// dialog's own close box). Nothing about either field is re-implemented: a committed cell dispatches
// exactly the keydown/keyup pair the matching physical key produces (input-common.mjs's tapKey), which
// play/gameplay-shell.mjs routes into charcreate.mjs's own typeKey or into its amount-box branch like
// any other key. A third such field would cost a layout and a code map, not a mechanism.
const NAME_ROWS = ['ABCDEFG', 'HIJKLMN', 'OPQRSTU', 'VWXYZ-_', ['DEL', 'OK']];
// The code a physical key carries for the cells that are not letters ('-' is Minus, '_' is Shift+Minus).
const NAME_CELL_CODE = { DEL: ['Backspace', 'Backspace'], OK: ['Enter', 'Enter'], '-': ['Minus', '-'], _: ['Minus', '_'] };
const nameCellCode = (label) => NAME_CELL_CODE[label] || [`Key${label}`, label];
// The gold amount box: retail's ten digits and the two editing keys the shell's modal branch reads.
// `0` carries no letter, so the cell maps straight onto the digit key the physical row produces.
const GOLD_ROWS = ['123', '456', '789', ['DEL', '0', 'OK']];
const GOLD_CELL_CODE = { DEL: ['Backspace', 'Backspace'], OK: ['Enter', 'Enter'] };
const goldCellCode = (label) => GOLD_CELL_CODE[label] || [`Digit${label}`, label];
// Where each grid sits, as fractions of the canvas's OWN box -- so it scales with the page and stays
// over the game. The name grid keeps the lower left, where the name box, the class list and OK/EXIT
// stay visible while it is up. The gold grid sits BELOW the amount dialog (whose own controls are
// around three quarters of the width and half the height, in the 800x600 space) so the coin, the
// arrows, OK and the close box all stay clear of it, and the modal has no other clickable control
// anyway: while it is up the shell consumes every other press as its own background.
const NAME_GRID_BOX = { x: 0.005, y: 0.55, w: 0.36, h: 0.44 };
const NAME_COLS = 7;
const GOLD_GRID_BOX = { x: 0.005, y: 0.62, w: 0.26, h: 0.30 };
const GOLD_COLS = 3;
const CHAT_GRID_BOX = { x: 0.005, y: 0.60, w: 0.45, h: 0.38 };   // under the chat bar, clear of the world's centre

function makeCursorEl(viewport) {
  const el = document.createElement('div');
  el.id = 'controller-cursor';
  Object.assign(el.style, {
    position: 'absolute', width: '14px', height: '14px', transform: 'translate(-50%,-50%)',
    borderRadius: '50%', border: '2px solid #fff', boxShadow: '0 0 3px #000, 0 0 1px #000 inset',
    pointerEvents: 'none', display: 'none', zIndex: 5,
  });
  viewport.appendChild(el);
  return el;
}

// A grid's cells, built once; only their highlight and the container's geometry change. `cells` and
// `labels` are the same length and the same order, so one index addresses both.
function makeGrid(viewport, id, rows, cols, box) {
  const el = document.createElement('div');
  el.id = id;
  Object.assign(el.style, { position: 'absolute', display: 'none', pointerEvents: 'none', zIndex: 6 });
  const cells = [], labels = [];
  rows.forEach((row, r) => {
    const line = document.createElement('div');
    Object.assign(line.style, { display: 'flex', width: '100%' });
    Array.from(row).forEach((label, c) => {
      const cell = document.createElement('div');
      cell.textContent = label;
      Object.assign(cell.style, {
        width: `${100 / cols}%`, boxSizing: 'border-box', textAlign: 'center', lineHeight: '1.35',
        fontFamily: OVERLAY_FONT, color: '#d8d8d8', background: 'rgba(0,0,0,0.65)',
        border: '1px solid #4a4a4a', borderRadius: '2px', overflow: 'hidden',
      });
      line.appendChild(cell);
      labels.push({ label, row: r, col: c });
      cells.push(cell);
    });
    el.appendChild(line);
  });
  const hintEl = document.createElement('div');
  Object.assign(hintEl.style, { fontFamily: OVERLAY_FONT, color: '#c8c8c8', background: 'rgba(0,0,0,0.65)', textAlign: 'center', fontSize: '11px' });
  el.appendChild(hintEl);
  viewport.appendChild(el);
  return { el, cells, labels, rows, index: 0, hidden: false, box, hintEl };
}

function start() {
  const canvas = document.getElementById('game') || document.querySelector('canvas');
  if (!canvas) { setTimeout(start, POLL_MS); return; }   // defensive only: module scripts run after the DOM parses
  const viewport = canvas.parentElement || document.body;
  if (getComputedStyle(viewport).position === 'static') viewport.style.position = 'relative';
  const cursorEl = makeCursorEl(viewport);
  const nameGrid = makeGrid(viewport, 'controller-name-keys', NAME_ROWS, NAME_COLS, NAME_GRID_BOX);
  const goldGrid = makeGrid(viewport, 'controller-gold-keys', GOLD_ROWS, GOLD_COLS, GOLD_GRID_BOX);
  const chatGrid = makeGrid(viewport, 'controller-chat-keys', CHAT_ROWS, 10, CHAT_GRID_BOX);

  const holds = { a: makeTargetHold(), b: makeTargetHold() }, statRepeat = makeRepeater(STAT_HOLD_FIRST_MS, STAT_HOLD_EVERY_MS);
  const applyHold = (acts, button) => { for (const a of acts) pointer(canvas, `pointer${a.type}`, a.x, a.y, { button, shiftKey: true, buttons: a.type === 'up' ? 0 : button === 2 ? 2 : 1 }); };
  const reader = createReportReader();
  const runToggle = createRunToggle();
  const settings = createSettings(globalThis.localStorage);
  const settingsPanel = createSettingsPanel(viewport, settings, OVERLAY_FONT);
  window.__d2PadSettings = { get: settings.get, set: settings.set, panel: settingsPanel };
  const repeat = Object.fromEntries([BUTTON.DPAD_UP, BUTTON.DPAD_DOWN, BUTTON.DPAD_LEFT, BUTTON.DPAD_RIGHT].map((b) => [b, makeRepeater()]));
  const state = { connected: false, index: -1, cursor: { x: canvas.width / 2, y: canvas.height / 2 }, lastStickWalk: 0, wasPressed: {}, shift: false, showItems: false, aim: null, cycle: 0, cycleSig: '', cycleDir: 0,
    mode: 'cursor', worldTarget: null, panelIndex: 0, panelSig: null, free: false };

  const placeCursor = () => {
    cursorEl.style.left = `${state.cursor.x / canvas.width * 100}%`;
    cursorEl.style.top = `${state.cursor.y / canvas.height * 100}%`;
  };
  placeCursor();

  // What a row asserts against: connect state and the cursor's own
  // canvas position, read back rather than the module offering a second, faked way to move it.
  //
  // press() is S000_INPUT=pad's seam (wasm-build/scenarios/lib/player.mjs's click()/rclick(), mirroring
  // S000_INPUT=touch's tap()/longPress() on window.__d2TouchInput): a scripted player already knows the
  // exact point and button it wants, unlike a real thumbstick-plus-button sequence. It parks the cursor
  // at the point (as if the right stick had already put it there over earlier polls -- no pointermove
  // of its own, so it adds no event the poll loop's own A/B edge below would not also produce) and then
  // calls the exact SAME click(canvas, x, y, {button}) the poll loop's A/B branch calls -- never a
  // second, mouse-shaped stand-in.
  window.__d2Controller = {
    get evidence() {
      // `mode` is which of the four roads this poll took ("Controller: D2R's scheme,
      // specified"): 'world' (D2R's no-cursor targeting), 'panel' (snap-to-slot on one of the six
      // named panels), 'field' (the name/gold typed-field grid) or 'cursor' (the pre-D2R free cursor,
      // still the road for every screen the D2R research has not specified). `worldTarget` is exactly
      // what selectWorldTarget picked this poll (or null); it records the controller model's choice
      // and does not claim that the mouse-hover resolver would choose the same unit.
      return { connected: state.connected, index: state.index, cursor: { x: state.cursor.x, y: state.cursor.y }, shift: state.shift,
        mode: state.mode, worldTarget: state.worldTarget && { ...state.worldTarget }, panelIndex: state.panelIndex, panelSig: state.panelSig };
    },
    // The name grid's own state, for a row to aim at without knowing the layout (G7): where the
    // highlight is, the rows it can be in, and the name the shell's own evidence already reports.
    get nameEntry() {
      const cell = nameGrid.labels[nameGrid.index];
      return {
        visible: nameGrid.el.style.display !== 'none',
        index: nameGrid.index, row: cell.row, col: cell.col, label: cell.label,
        rows: NAME_ROWS.map((r) => Array.from(r)),
        name: globalThis.__d2Shell?.charCreateEvidence?.name ?? null,
      };
    },
    // The gold grid's state, s306's shape for the amount box (G13). `amount`/`available`/`mode` are the
    // shell's own reading of the dialog, so a row compares what the pad typed into retail's field with
    // what the keyboard types into the same field -- never a second copy of the number kept here.
    get goldEntry() {
      const cell = goldGrid.labels[goldGrid.index];
      const gold = globalThis.__d2Shell?.stashEvidence?.gold?.drawn ?? null;
      return {
        visible: goldGrid.el.style.display !== 'none',
        index: goldGrid.index, row: cell.row, col: cell.col, label: cell.label,
        rows: GOLD_ROWS.map((r) => Array.from(r)),
        open: Boolean(gold?.modal), mode: gold?.modal?.mode ?? null,
        amount: gold?.modal?.amount ?? null, available: gold?.modal?.available ?? null,
      };
    },
    // shiftKey is the second thing the seam carries, and it is a REAL pad road, not a scripted-only
    // capability: the poll loop reads it from L3 (held) and passes it to the identical click() below.
    // Without it a pad-only player cannot shift-click at all, and retail's shift-click is the ONLY way
    // to move an item from the pack to the belt (gameplay-shell.mjs's gridToBelt, retail 0x63) -- so
    // the belt's columns (d-pad -> Digit1-4) could never be filled from the pad. Measured end to end:
    // with the modifier dropped, s000a driven by the pad stalls inside step 19 (the Den of Evil walk
    // never converges, walkEvidence.hops stuck at 16) because lib/player.mjs's belt refill shift-clicks
    // potions in and the click arrived unmodified; with it carried, the same run's steps.json is
    // identical to the mouse run's for all 34 shared steps and stops at the same blocker (step 35).
    press(x, y, button = 0, shiftKey = false) {
      state.cursor.x = x; state.cursor.y = y;
      placeCursor();
      click(canvas, x, y, { button, shiftKey });
    },
    // key() is press()'s counterpart for the KEYBOARD half of the pad's mapping (G10): the poll loop
    // above reaches the belt (d-pad), Escape (Start), KeyI/KeyC/KeyT (Back-bank/RB/RT) and F1-F3 (X/Y/LT)
    // only through its own edge detection on a real pad's buttons, which a scripted s000 player cannot
    // hold -- so under S000_INPUT=pad every one of those actions was dispatched by the scenario player
    // itself, as a raw KeyboardEvent that never passed through this module at all. That made the pad
    // run "mouse-shaped" in exactly the half of the mapping the keyboard owns, and it is what G6's row
    // left open ("`__s000.key()` has no pad branch ... which is what 'driven by the pad alone'
    // overstates"). key() calls the exact SAME tapKey() the poll loop's own key branches call -- never
    // a second, keyboard-shaped stand-in -- so a pad-driven run exercises one dispatch road for every
    // input it makes.
    key(code, key) {
      tapKey(code, key);
    },
  };

  function rawEdge(pad, i) {
    const now = Boolean(pad.buttons[i]?.pressed), was = Boolean(state.wasPressed[i]);
    state.wasPressed[i] = now;
    return now && !was;
  }
  // A held bank (Back or LT) takes a button's press for itself: the plain meaning of that press is
  // swallowed, so one press is one action. `masked` is the set of buttons the bank owns this poll.
  let masked = new Set();
  const holdA = makeHold();
  const edge = (pad, i) => rawEdge(pad, i) && !masked.has(i);
  // Press the key the player's own Configure Controls table binds to a retail function.
  const fnKey = (name) => {
    const [label, def, keyName] = FN[name];
    const code = globalThis.__d2Shell?.controlCodeFor?.(label) || def;
    // A rebound code carries its own key name; one with none is not pressed rather than sent as an empty key.
    const shown = code === def ? keyName : keyNameForCode(code);
    if (shown) tapKey(code, shown);
  };
  // The skill buttons (pad-skill-*.mjs) own the world: they reach the game only through these events.
  const skillMapper = makeSkillMapper(), skillOverlay = createSkillOverlay(viewport, OVERLAY_FONT);
  const skillPlay = makeSkillPlay({ io: {
    selectSkill: (id, side) => globalThis.__d2Shell?.selectSkill?.(id, side),
    pointer: (type, x, y, o) => pointer(canvas, type, x, y, { button: o.button, shiftKey: o.shiftKey }),
    fnKey: (name) => (name === 'escape' ? tapKey('Escape', 'Escape') : fnKey(name)),
    keyHold: (name, down) => { const [label, def, keyName] = FN[name]; const code = globalThis.__d2Shell?.controlCodeFor?.(label) ?? def; key(code, code === def ? keyName : keyNameForCode(code), down); },
  } });
  window.__d2PadSkills = skillPlay;
  // One poll of the skill buttons: who is in reach, the character's skills, then the mapped actions.
  function runSkills(shell, act, origin, field, pad) {
    const lx = pad.axes[0] || 0, ly = pad.axes[1] || 0;
    if (field && Math.hypot(lx, ly) > 0) state.aim = [lx, ly];
    const snap = shell?.snapshot, open = skillPlay.uiMode !== 'none';
    const ranked = field && snap?.player && shell.project ? rankTargets(snap, { aim: state.aim, project: shell.project }) : [];
    const known = field || open ? shell?.padSkills || {} : {};
    // With retail's pointer out (the right stick), A and B click where it is, so they cast nothing.
    if (state.free) act = { ...act, slot: act.slot > 1 ? act.slot : -1, slotDown: act.slotDown.map((d, i) => d && i > 1) };
    const [ax, ay] = state.aim || [0, 1], len = Math.hypot(ax, ay) || 1;
    const host = { ...known, selected: shell?.selectedSkills || { left: -1, right: -1 }, dead: snap?.mode === 0 || snap?.mode === 17,
      ranked, hero: origin, ahead: origin && stickTarget(origin, ax / len, ay / len) };
    skillPlay.step(host, act, { field });
    skillOverlay.render(skillPlay, skillPlay.wheelIds(host), known.skillName || (() => ''));
    return ranked;
  }

  // The name grid is up exactly while retail's own name box is: the create screen with a class chosen.
  // Everything about it is polled, so the shared shell file needs no hook (input-touch.mjs's issue 8 input
  // reads the same signal the same way). The gold grid is up exactly while the amount dialog is, read
  // from the shell's own drawing of it (`stashEvidence.gold.drawn.modal`, non-null only while open) --
  // so the pad appears when the field does and no button has to summon it.
  const nameGridOpen = () => globalThis.__d2Shell?.screen === 'create' && Boolean(globalThis.__d2Shell?.charCreateEvidence?.chosen);
  const chatGridOpen = () => globalThis.__d2Shell?.chatOpen === true;
  const goldGridOpen = () => Boolean(globalThis.__d2Shell?.stashEvidence?.gold?.drawn?.modal);

  // One d-pad press: a vertical move keeps the column it was in where the new row is long enough and
  // clamps to that row's end where it is not (the DEL/OK row is two cells wide), a horizontal move
  // stays inside the row it is on. The highlight therefore never leaves the grid.
  function moveIndex(g, dr, dc) {
    const cur = g.labels[g.index];
    const row = Math.max(0, Math.min(g.rows.length - 1, cur.row + dr));
    const len = Array.from(g.rows[row]).length;
    const col = dr !== 0 ? Math.min(cur.col, len - 1) : Math.max(0, Math.min(len - 1, cur.col + dc));
    g.index = g.labels.findIndex((c) => c.row === row && c.col === col);
  }

  // In a menu, a grid or a typed field the left stick steps like the d-pad: past half travel on an axis is a press.
  function navPressed(pad, button) {
    const ax = pad.axes?.[0] || 0, ay = pad.axes?.[1] || 0;
    const stick = button === BUTTON.DPAD_LEFT ? ax < -0.5 : button === BUTTON.DPAD_RIGHT ? ax > 0.5 : button === BUTTON.DPAD_UP ? ay < -0.5 : ay > 0.5;
    return (Boolean(pad.buttons[button]?.pressed) && !masked.has(button)) || stick;
  }

  function dir(pad, button) {
    rawEdge(pad, button);
    return repeat[button](navPressed(pad, button), performance.now(), MENU_REPEAT_FIRST_MS);
  }

  function renderGrid(g, visible) {
    const { el, cells } = g;
    if (!visible) { el.style.display = 'none'; return; }
    const box = { x: canvas.offsetLeft + g.box.x * canvas.offsetWidth, y: canvas.offsetTop + g.box.y * canvas.offsetHeight,
      w: g.box.w * canvas.offsetWidth, h: g.box.h * canvas.offsetHeight };
    Object.assign(el.style, { display: 'block', left: `${Math.round(box.x)}px`, top: `${Math.round(box.y)}px`,
      width: `${Math.round(box.w)}px`, height: `${Math.round(box.h)}px`, fontSize: `${Math.max(9, Math.round(box.h / g.rows.length * 0.3))}px` });
    g.hintEl.textContent = hintLine(state.layout, [[BUTTON.A, 'Select'], [BUTTON.B, 'Delete'], [BUTTON.Y, 'Hide']]);
    cells.forEach((cell, i) => {
      const on = i === g.index;
      cell.style.background = on ? '#8a6a1a' : 'rgba(0,0,0,0.65)';
      cell.style.color = on ? '#fff' : '#d8d8d8';
      cell.style.borderColor = on ? '#ffd24a' : '#4a4a4a';
    });
  }

  // One committed cell: the identical keydown/keyup pair the matching physical key produces.
  function commitGrid(g, cellCode) {
    const [code, keyName] = cellCode(g.labels[g.index].label);
    tapKey(code, keyName);
  }

  // Hold A on a monster / B on any target or the ground ahead: after HOLD_MS the press becomes a held
  // Shift-click (stands still, repeats), re-aimed as the target moves, released with the button.
  function holdWorld(ranked, aEdge, bEdge, picked, ahead) {
    const pad = navigator.getGamepads()[state.index];
    const held = (b) => Boolean(pad?.buttons[b]?.pressed) && !masked.has(b), now = performance.now();
    const find = (key) => { const t = ranked.find((r) => `${r.kind}${r.id}` === key); return t ? { screen: t.screen } : null; };
    const key = picked && `${picked.kind}${picked.id}`;
    applyHold(holds.a.step({ edge: aEdge, pressed: held(BUTTON.A), now, key: picked?.kind === 'unit' ? key : null, locate: find }), 0);
    applyHold(holds.b.step({ edge: bEdge, pressed: held(BUTTON.B), now, key: key || (ahead ? 'ahead' : null),
      locate: (k) => (k === 'ahead' ? (ahead ? { screen: ahead } : null) : find(k)) }), 2);
  }

  // The OS mouse cursor hides once the pad does something, and returns on the first real (trusted) mouse or
  // touch event; the pad's own synthetic pointer events are untrusted and never bring it back.
  const showMouse = (e) => { if (e.isTrusted) canvas.style.cursor = ''; };
  for (const type of ['pointermove', 'pointerdown']) globalThis.addEventListener?.(type, showMouse, true);

  function poll() {
    const pads = navigator.getGamepads ? navigator.getGamepads() : [];
    // pad-report.mjs: an unmapped pad's d-pad hat and analog triggers become the standard buttons; a standard pad passes through.
    const rawPad = reader.read(pickPad(pads, state.connected ? state.index : -1));
    if (!rawPad) {
      if (state.connected) { applyHold(holds.a.release(), 0); applyHold(holds.b.release(), 2); state.connected = false; state.wasPressed = {}; cursorEl.style.display = 'none'; for (const g of [nameGrid, goldGrid, chatGrid]) g.el.style.display = 'none'; }
      return;
    }
    if (!state.connected || state.index !== rawPad.index) { state.connected = true; state.index = rawPad.index; }
    // The settings overlay owns the pad while it is up; edges are kept current so a button released into the
    // game does not arrive as a fresh press.
    if (settingsPanel.update(rawPad.buttons)) { for (let i = 0; i < 16; i++) rawEdge(rawPad, i); return; }
    const pad = adjustedPad(rawPad, settings.get());
    if (rawPad.buttons.some((x) => x?.pressed) || [0, 1, 2, 3].some((i) => Math.abs(rawPad.axes[i] || 0) > 0.5)) canvas.style.cursor = 'none';

    const shell = globalThis.__d2Shell;
    const screenName = shell?.screen;

    // The two text fields (G7 name, G13 gold amount): whichever field is up shows its own grid, and a
    // field that has gone starts fresh, so its grid comes back on its own next time it appears. Read
    // here, before everything below, because a typed field owns the d-pad and the A button too (and
    // its evidence getter is a deep copy -- once a poll, not twice).
    const nameUp = nameGridOpen(), goldUp = goldGridOpen(), chatUp = chatGridOpen();
    if (!nameUp) nameGrid.hidden = false;
    if (!goldUp) goldGrid.hidden = false;
    if (!chatUp) chatGrid.hidden = false;
    // A hidden grid gives navigation back to the screen below it. Toggle before
    // choosing the input owner so the same poll can return to class snap-nav.
    const activeField = goldUp ? goldGrid : chatUp ? chatGrid : nameUp ? nameGrid : null;
    const cellCode = activeField === goldGrid ? goldCellCode : activeField === chatGrid ? chatCellCode : nameCellCode;
    if (activeField && edge(pad, BUTTON.Y)) activeField.hidden = !activeField.hidden;
    const typedFieldUp = Boolean(activeField) && !activeField.hidden;
    const fieldOpen = typedFieldUp;
    const modal = modalPointerOwner({
      gameMenuOpen: screenName === 'game' && Boolean(shell?.gameMenuEvidence?.open),
      stashGoldDialogOpen: goldUp,
    });

    // D2R snap-to-slot panels (G4, G16): inventory, stash, vendor, character, skill tree, and now the
    // title, character-select, escape and NPC menus -- read from the shell's own `uiTargets()`, the
    // SAME canvas points its click handler hit-tests against, so a slot the pad lands on is a slot the
    // shell resolves identically (never a second geometry). The cube has no client panel yet (a gap,
    // ), so it is not in `panelPoints`' list. Not gated on `modal` any more: the
    // escape menu IS one of the panels now (G16), so `uiTargets()` is asked on every screen and
    // `panelPoints` picks whichever one, if any, is actually open this poll.
    const targets = !fieldOpen && shell?.uiTargets ? shell.uiTargets() : null;
    const basePanel = panelPoints(targets);
    const panelAll = enhancePanelPoints(basePanel, targets, { items: shell?.snapshot?.items, width: canvas.width, height: canvas.height,
      held: shell?.cursorItem ? (shell.snapshot?.items?.find((i) => i.id === shell.cursorItem) || { id: shell.cursorItem }) : null,
      layout: targets && shell?.touchPanelLayout ? shell.touchPanelLayout() : null });
    // An empty point list is no panel: every use below indexes into it.
    const panel = panelAll?.length ? panelAll : null;
    // D2R's own world mode: the game screen with nothing modal, no snap panel and no typed field up --
    // the ONE state with no free cursor at all.
    const worldMode = screenName === 'game' && !modal && !panel && !fieldOpen;
    // What is left over keeps the pre-D2R free cursor only for screens `uiTargets()` reports nothing
    // for. Character creation's class list is now a panel too (G17); title, select, escape and NPC
    // menus were already covered by G16.
    const cursorMode = !fieldOpen && !panel && !worldMode;
    // The skill buttons own the world, and their wheel and panel list wherever they are open.
    const skillUi = skillPlay.uiMode, skillOwns = worldMode || skillUi !== 'none';
    const skillAct = skillMapper(pad, { ui: skillUi, field: worldMode && skillUi === 'none', seconds: POLL_MS / 1000 });

    // Right stick: the pre-D2R free cursor, now only live where D2R has not specified a road yet (G1
    // as built before 2026-09-28 -- see ). Off in the world (D2R has no free cursor
    // there) and off over a snap panel (the d-pad drives the highlight instead, below).
    if (cursorMode) {
      cursorEl.style.display = 'block';
      const rx = pad.axes[2] || 0, ry = pad.axes[3] || 0;
      if (Math.hypot(rx, ry) > 0) {
        state.cursor.x = Math.max(0, Math.min(canvas.width, state.cursor.x + rx * CURSOR_SPEED));
        state.cursor.y = Math.max(0, Math.min(canvas.height, state.cursor.y + ry * CURSOR_SPEED));
        placeCursor();
        pointer(canvas, 'pointermove', state.cursor.x, state.cursor.y);
      }
    }

    // Left stick: walk -- a click at stickTarget from the hero, repeated ~every 250ms while held,
    // exactly what a mouse user does ahead of the hero (G2), and D2R's own move (PS Blog: no
    // pathfinding, run/walk a toggle not analog -- retail's move command is exactly that discrete
    // state) -- but NOT while a modal owns the pointer (G14).
    const origin = playerOnCanvas();
    const target = !modal && origin && stickTarget(origin, pad.axes[0] || 0, pad.axes[1] || 0, { deadzone: 0 });
    const now = performance.now();
    if (target && now - state.lastStickWalk >= STICK_WALK_MS) { click(canvas, target[0], target[1]); state.lastStickWalk = now; }

    if (typedFieldUp) {
      masked = new Set();
      state.mode = 'field';
      // The d-pad moves the grid's highlight. Neither the belt (there is no session on the create
      // screen for it to mean anything to, and the amount box is a modal the shell consumes every
      // digit of) nor the face skill keys are reachable from here, so the two do not both fire on
      // one press. Y above is the grid's own toggle, which is why it is not the game's F2 here.
      if (dir(pad, BUTTON.DPAD_LEFT)) moveIndex(activeField, 0, -1);
      if (dir(pad, BUTTON.DPAD_RIGHT)) moveIndex(activeField, 0, 1);
      if (dir(pad, BUTTON.DPAD_UP)) moveIndex(activeField, -1, 0);
      if (dir(pad, BUTTON.DPAD_DOWN)) moveIndex(activeField, 1, 0);
      if (edge(pad, BUTTON.A)) commitGrid(activeField, cellCode);
      // B is the field's Backspace (every grid has a DEL cell; B saves walking to it).
      if (edge(pad, BUTTON.B)) tapKey('Backspace', 'Backspace');
    } else {
      // RB is the keyboard's Shift, read as a HELD modifier: the shell reads event.shiftKey per click.
      // In the skill popup RB is skill 5 (assigning it), not Shift: nothing in the popup takes a shift-click.
      const skillPopup = Boolean(panel?.[0]?.key.startsWith('skillsel:'));
      state.shift = !worldMode && !skillPopup && Boolean(pad.buttons[BUTTON.RB]?.pressed);
      // Back held: the backup bank owns the d-pad and the face buttons. LT held: X, Y, RB and RT are skills 1, 3, 7, 8,
      // and in the game the d-pad and Select open the panels (LT_BANK).
      masked = new Set();
      const inGame = screenName === 'game' && !modal;
      const ltHeld = Boolean(pad.buttons[BUTTON.LT]?.pressed);
      // Select pressed while LT is held is the Skill Speed Bar; that press is then neither an automap tap nor the
      // backup bank, for as long as Select stays down.
      if (rawEdge(pad, BUTTON.BACK) && ltHeld && inGame && !skillOwns) { fnKey(LT_BANK[BUTTON.BACK]); state.backUsed = true; state.ltBankBack = true; }
      const backHeld = !skillOwns && Boolean(pad.buttons[BUTTON.BACK]?.pressed) && !state.ltBankBack;
      if (backHeld) {
        for (const button of Object.keys(MENU_BANK).map(Number)) {
          masked.add(button);
          if (rawEdge(pad, button)) { fnKey(MENU_BANK[button]); state.backUsed = true; }
        }
      } else if (ltHeld && !skillOwns) {
        SKILL_BUTTONS.forEach((button) => masked.add(button));
        if (inGame) for (const button of DPAD_BANK) {
          masked.add(button);
          if (rawEdge(pad, button)) fnKey(LT_BANK[button]);
        }
      }

      // The right stick moves retail's own pointer anywhere in the game; A and B then click where it is. The left
      // stick (walking, or stepping a panel) or a d-pad step in a panel gives the pad back to the picker / the highlight.
      const pointerStick = skillUi === 'none' && Math.hypot(pad.axes[2] || 0, pad.axes[3] || 0) > 0;
      const leftStick = Math.hypot(pad.axes[0] || 0, pad.axes[1] || 0) > 0;
      const dpadStep = Boolean(panel) && [BUTTON.DPAD_UP, BUTTON.DPAD_DOWN, BUTTON.DPAD_LEFT, BUTTON.DPAD_RIGHT].some((b) => pad.buttons[b]?.pressed && !masked.has(b));
      if (screenName !== 'game' || cursorMode || skillUi !== 'none') state.free = false;
      else if (pointerStick) state.free = true;
      else if (state.free && (leftStick || dpadStep)) {
        state.free = false;
        if (panel) state.panelIndex = nearestIndex(panel, state.cursor.x, state.cursor.y);
      }

      const skillRanked = skillOwns ? runSkills(shell, skillAct, origin, worldMode && skillUi === 'none', pad) : (skillPlay.releaseAll(), skillOverlay.hide(), []);
      if (state.free) {
        state.mode = 'pointer';
        state.worldTarget = null;
        // Retail draws its own pointer at the last pointer position, so no ring is shown for it.
        cursorEl.style.display = 'none';
        if (panel) state.panelSig = panel.map((p) => p.key).join('|');
        if (pointerStick) {
          state.cursor.x = Math.max(0, Math.min(canvas.width, state.cursor.x + (pad.axes[2] || 0) * CURSOR_SPEED));
          state.cursor.y = Math.max(0, Math.min(canvas.height, state.cursor.y + (pad.axes[3] || 0) * CURSOR_SPEED));
          placeCursor();
          pointer(canvas, 'pointermove', state.cursor.x, state.cursor.y);
        }
        if (edge(pad, BUTTON.A)) click(canvas, state.cursor.x, state.cursor.y, { button: 0, shiftKey: state.shift });
        if (edge(pad, BUTTON.B)) click(canvas, state.cursor.x, state.cursor.y, { button: 2, shiftKey: state.shift });
      } else if (panel) {
        // D2R's snap-to-slot (G4): the d-pad snaps the highlight to whichever OTHER point in the
        // panel lies most nearly in the pressed direction (`snapMove`); A commits it -- a left click
        // there, exactly what a mouse click on that slot does -- and B a right click. A new panel, or
        // the same panel with a different point COUNT (an item picked up, a page turned), starts the
        // highlight over rather than aiming a stale index at a point that used to be something else.
        state.mode = 'panel';
        const sig = panel.map((p) => p.key).join('|');
        const panelChanged = sig !== state.panelSig;
        // An item lifted or put down changes the point set: stay on the nearest point, not the first.
        if (panelChanged) { state.panelIndex = state.panelSig && state.panelSig.split(':')[0] === sig.split(':')[0] ? nearestIndex(panel, state.cursor.x, state.cursor.y) : 0; state.panelSig = sig; }
        const panelHold = /^(select|create|gamemenu|npcmenu|cinematics|credits|otherMultiplayer|titleGateways|char|waypoint|quest)$/.test(panel[0]?.key.split(':')[0]) ? MENU_REPEAT_FIRST_MS : undefined;
        const t = performance.now(), held = (b) => { rawEdge(pad, b); return navPressed(pad, b); };
        if (repeat[BUTTON.DPAD_LEFT](held(BUTTON.DPAD_LEFT), t, panelHold)) state.panelIndex = snapMove(panel, state.panelIndex, -1, 0);
        if (repeat[BUTTON.DPAD_RIGHT](held(BUTTON.DPAD_RIGHT), t, panelHold)) state.panelIndex = snapMove(panel, state.panelIndex, 1, 0);
        if (repeat[BUTTON.DPAD_UP](held(BUTTON.DPAD_UP), t, panelHold)) state.panelIndex = snapMove(panel, state.panelIndex, 0, -1);
        if (repeat[BUTTON.DPAD_DOWN](held(BUTTON.DPAD_DOWN), t, panelHold)) state.panelIndex = snapMove(panel, state.panelIndex, 0, 1);
        const cur = panel[Math.max(0, Math.min(panel.length - 1, state.panelIndex))];
        const pointChanged = state.cursor.x !== cur.x || state.cursor.y !== cur.y;
        state.cursor.x = cur.x; state.cursor.y = cur.y; placeCursor(); cursorEl.style.display = 'block';
        // F295: the escape menu keeps its own selected row for the pentagram highlight; move the same
        // DOM pointer the mouse path uses whenever snap-nav focuses one of its targets.
        // The skill popup reads its hover the same way: that hover is what an F-key assigns.
        if ((cur.key.startsWith('create:') || cur.key.startsWith('gamemenu:') || cur.key.startsWith('skillsel:')) && (panelChanged || pointChanged)) {
          pointer(canvas, 'pointermove', cur.x, cur.y);
        }
        // Inventory shortcuts (pad-panels.mjs): LT+A quick-moves, R3 fills the belt, holding A equips.
        const heldItem = shell?.cursorItem || 0, ctx = { items: shell?.snapshot?.items, width: canvas.width, height: canvas.height, layout: targets && shell?.touchPanelLayout ? shell.touchPanelLayout() : null };
        const r3Now = Boolean(pad.buttons[BUTTON.R3]?.pressed), r3Edge = r3Now && !state.r3Panel; state.r3Panel = r3Now;
        const holdKey = holdA(Boolean(pad.buttons[BUTTON.A]?.pressed), t, cur.key);
        const aEdge = edge(pad, BUTTON.A), statHeld = cur.key.startsWith('char:') && Boolean(pad.buttons[BUTTON.A]?.pressed) && !masked.has(BUTTON.A);
        // Character panel: holding A on a stat's + keeps adding points (a normal + click each time).
        const statAgain = statRepeat(statHeld, t, STAT_HOLD_FIRST_MS);
        const plan = (aEdge && pad.buttons[BUTTON.LT]?.pressed && quickMovePlan(basePanel, targets, ctx, cur, heldItem))
          || (r3Edge && beltFillPlan(panel, targets, ctx.items, heldItem));
        if (plan) for (const c of plan) click(canvas, c.x, c.y, { button: 0, shiftKey: state.shift || Boolean(c.shift), ctrlKey: Boolean(c.ctrl) });
        else if (aEdge || (statHeld && statAgain)) click(canvas, cur.x, cur.y, { button: 0, shiftKey: state.shift });
        if (holdKey && /^invg:i/.test(holdKey) && shell?.cursorItem === Number(holdKey.slice(6))) equipHeld(shell.cursorItem, panel, ctx.items, (x, y) => click(canvas, x, y), () => shell.cursorItem);
        if (edge(pad, BUTTON.B)) {
          // B is "back" on menu screens: the screen's EXIT button or Escape. Over item grids it stays the right click.
          // In the skill popup B closes it the mouse's way: a click on the hand's own skill button.
          const popupButton = skillPopup && shell?.skillSelectOpen ? targets?.skillSelect?.[shell.skillSelectOpen] : null;
          const back = popupButton ? { click: popupButton } : backAction(targets, cur.key);
          if (back?.key) tapKey(back.key);
          else if (back?.click) click(canvas, back.click[0], back.click[1]);
          else click(canvas, cur.x, cur.y, { button: 2, shiftKey: state.shift });
        }
      } else if (worldMode) {
        // D2R-style no-cursor targeting: the skill buttons above cast at the picked target through the
        // shell's normal click path; this records the pick and keeps the opt-in gold pickup.
        state.mode = 'world';
        state.panelSig = null;
        const picked = pickTarget(skillRanked, 0);
        const worldTarget = picked && { kind: picked.kind, id: picked.id, x: picked.x, y: picked.y, screen: picked.screen };
        state.worldTarget = worldTarget;
        // Opt-in: click a gold pile within reach, at most once per pile every 600 ms, never with an item held.
        if (settings.get().autoGold && !shell.cursorItem) {
          const g = goldInReach(shell.snapshot);
          if (g && (state.goldId !== g.id || now - (state.goldAt || 0) > 600)) {
            state.goldId = g.id; state.goldAt = now;
            const [gx, gy] = shell.project(g.x, g.y);
            click(canvas, gx, gy, { button: 0 });
          }
        }
        if (worldTarget) { state.cursor.x = worldTarget.screen[0]; state.cursor.y = worldTarget.screen[1]; placeCursor(); }
        cursorEl.style.display = 'none';   // no white ring over the world target
      } else {
        // The pre-D2R free cursor, for every screen the D2R research has not specified yet (title,
        // character select, the escape menu, the NPC menu).
        state.mode = 'cursor';
        state.panelSig = null;
        if (edge(pad, BUTTON.A)) click(canvas, state.cursor.x, state.cursor.y, { button: 0, shiftKey: state.shift });
        if (edge(pad, BUTTON.B)) click(canvas, state.cursor.x, state.cursor.y, { button: 2, shiftKey: state.shift });
      }

      if (state.mode !== 'world') { applyHold(holds.a.release(), 0); applyHold(holds.b.release(), 2); }

      // D-pad: the four belt slots, only while no snap panel has claimed the d-pad for navigation above.
      if (!panel && skillUi === 'none') {
        for (const [button, name] of Object.entries(DPAD_BELT)) if (edge(pad, Number(button))) fnKey(name);
      }
      // X / Y / RT: skills 1-3; with LT held, 4-6 (the masked press is read raw below).
      const lt = Boolean(pad.buttons[BUTTON.LT]?.pressed) && !backHeld;
      SKILL_BUTTONS.forEach((button, i) => {
        if (skillOwns) { rawEdge(pad, button); return; }
        if (button === BUTTON.RB && !worldMode && !skillPopup) return;
        if (button === BUTTON.RB && backHeld) return;
        if (lt ? rawEdge(pad, button) : edge(pad, button)) fnKey(`skill${(lt ? SKILL_LT : SKILL_NORMAL)[i]}`);
      });
      // L3: run/walk. R3: swap weapons (D2R's default). LB held: Show Items (down for as long as it is held).
      rawEdge(pad, BUTTON.L3);
      if (runToggle.fire(screenName === 'game' && Boolean(pad.buttons[BUTTON.L3]?.pressed) && !masked.has(BUTTON.L3), performance.now())) fnKey('run');
      const lbDown = !skillOwns && Boolean(pad.buttons[BUTTON.LB]?.pressed) && !backHeld;
      const latch = settings.get().latchItems;
      if (rawEdge(pad, BUTTON.LB) && lbDown && latch) state.itemsLatched = !state.itemsLatched;
      if (!latch) state.itemsLatched = false;
      const r3 = latch ? state.itemsLatched : lbDown;
      if (screenName === 'game' && r3 !== state.showItems) {
        state.showItems = r3;
        const [label, def, keyName] = FN.showItems;
        const code = globalThis.__d2Shell?.controlCodeFor?.(label) ?? def;
        key(code, code === def ? keyName : '', r3);
      }
      // In a snap panel R3 is the belt fill above; the edge is still consumed so closing the panel cannot swap.
      if (edge(pad, BUTTON.R3) && !panel && !skillOwns) fnKey('swap');
    }
    if (edge(pad, BUTTON.START)) { if (pad.buttons[BUTTON.BACK]?.pressed) { state.backUsed = true; settingsPanel.toggle(true); } else if (!skillOwns) tapKey('Escape'); }
    // Back is the automap when tapped; held, it is the panel bank's chord key, and releasing it then does nothing.
    const backNow = Boolean(pad.buttons[BUTTON.BACK]?.pressed);
    if (backNow && !state.backWas && !state.ltBankBack) state.backUsed = false;
    if (backNow && skillOwns) state.backUsed = true;   // in the world Back is the skill wheel, never the automap
    if (!backNow && state.backWas && !state.backUsed) fnKey('automap');
    if (!backNow) state.ltBankBack = false;
    state.backWas = backNow;
    rawEdge(pad, BUTTON.BACK);

    state.layout = padLayout(pad.id);
    renderGrid(nameGrid, nameUp && !nameGrid.hidden);
    renderGrid(goldGrid, goldUp && !goldGrid.hidden);
    renderGrid(chatGrid, chatUp && !chatGrid.hidden);
  }
  setInterval(poll, POLL_MS);
}

if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', start, { once: true });
else start();
