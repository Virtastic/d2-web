// Gamepad support. A controller
// is an INPUT LAYER: it produces the SAME DOM pointer and key events a mouse and a keyboard produce,
// through input-common.mjs (whose dispatch mirrors wasm-build/scenarios/lib/player.mjs's click()), so
// the shell's own handlers issue the same engine commands. Nothing about the game is re-implemented
// here.
//
// Polled on a timer (setInterval), never requestAnimationFrame: rAF is throttled to nothing in a
// headless or background tab, which is exactly where the browser rows fake a
// pad with a page-installed navigator.getGamepads() stub -- a controller that only wakes on rAF would
// never see it.
//
// Rework: the pad plays LIKE D2R and is easy to use -- D2R is the model, not a
// byte-exact spec.
// D2R has NO free cursor in the world -- see `selectWorldTarget` below -- and its panels, menus and
// character-class list use snap-to-slot navigation instead of a pointer -- see `panelPoints`/`snapMove`.
//
// G16 (this pass) applied the researched three-guide consensus table ("Controller:
// D2R's scheme, specified", `bc9f458c`) to BUTTON/DPAD_BELT/FACE_SKILL_KEYS/SHIFT_BANK_KEYS and found
// every row it could safely change was ALREADY what is built: D-pad = belt (3/3 agree), left stick =
// move, A = basic attack -- all unchanged. The rows the table adds that this engine has no 1.13c
// action behind (skill slots 5/6, a second skill-tray bank -- only F1-F3/Attack,Throw,Kick exist here,
// no F4+; the run/walk TOGGLE -- `KeyR` is dropped by retail-key-bindings.mjs for the keyboard too, so
// there is nothing live to bind a button to yet) are LEFT OUT per 's own rule for a
// D2R feature with no retail action behind it, not built as a no-op gesture. L3 therefore keeps its
// proven G10 role (a HELD shift modifier the pack->belt move needs) rather than moving to the table's
// run/walk toggle, which would cost that proven road for a button with nothing yet to press. Weapon
// swap (already on L3's own bank, G12) and show-items (no engine road at all, an existing gap) are
// unaffected. G16 gave the title, character-select, escape (game) and NPC menus the SAME snap-nav as
// the six panels. G17 extends it to character creation's class list, using `uiTargets().create.classes`
// from the same hit boxes the shell click handler uses; once a class is chosen, the existing name grid
// continues to own the d-pad and A button.
//
// Standard mapping (https://w3c.github.io/gamepad/#remapping), recorded in (G5):
//   left stick (axes 0,1)  -- walk: the same click ahead of the hero a mouse user repeats (G2), and
//                             nothing while a modal owns the pointer -- a held stick must not press
//                             into the escape menu or the amount box (G14, see the walk branch)
//   right stick (axes 2,3) -- moves a visible cursor; the move is a pointermove, as a mouse (G1)
//   A (button 0)  -- left click at the cursor       B (button 1) -- right click at the cursor
//   D-pad (12-15) -- belt columns 1-4 (Digit1-Digit4), clockwise from Up: Up=1, Right=2, Down=3, Left=4
//   Start (button 9) -- Escape (the game menu)
//   LB (button 4) -- KeyI (inventory)                RB (button 5) -- KeyC (character)
//   RT (button 7) -- KeyT (skill tree)                                        (G3, G4)
//   X (button 2) -- F1, Y (button 3) -- F2, LT (button 6) -- F3: the right-hand skill, the keyboard's
//                   own skill hotkeys (Attack / Throw / Kick)                       (G8)
//   R3 (button 11) -- Space: retail's CfgClearScreen, the HUD hides and the same button restores it
//                   (G12; G9 called this "attack the nearest target" and that is withdrawn -- see
//                   CURSOR_STICK_CLICK_KEY below)
//   Back (button 8) -- Tab: the automap, the last live retail key left once the command keys were
//                   covered (G11) -- it is not a CommandInput key, so G9's "last command key" did
//                   not reach it, and `s301` proves the toggle from both roads
//   L3 (button 10) HELD -- SHIFT (G10), and a bank (G12): the d-pad's Up/Down become KeyH, KeyW
//                   (retail's Swap Weapons), the two live keys no button of the standard mapping
//                   reached (G12 -- see SHIFT_BANK_KEYS below)
//   L3 (button 10) -- SHIFT, held: while it is down, A/B carry shiftKey, which is how a pad-only
//                   player reaches retail's 0x63 shift-click item move (pack -> belt) at all (G10)
// The client's two typed fields -- the create screen's name box (G7) and the stash's gold amount box
// (G13) -- are worked with the same buttons and the same grid mechanism: while one is up, the d-pad
// moves the grid's highlight instead of the belt and A commits the highlighted cell, Y hides the grid
// so the cursor reaches whatever is under it. See "The pad's two text fields" below.
import { OVERLAY_FONT } from './input-touch.mjs';
import { click, pointer, tapKey, playerOnCanvas, stickTarget } from './input-common.mjs';
import { selectPrimaryTarget } from '../gameplay-model.mjs';
import { modalPointerOwner } from './modal-pointer-priority.mjs';

const POLL_MS = 50;          // setInterval, never requestAnimationFrame -- see header
const STICK_WALK_MS = 250;   // a mouse user's own repeat rate for a held click-to-walk
const CURSOR_SPEED = 14;     // canvas px per poll at full right-stick deflection
const DEADZONE = 0.25;
// How far this D2R-style "no cursor" targeting model reaches from the player. It is a stand-in for
// D2R's own screen-space cone (PS Blog: "a large cone", per-class weighted) until its numbers are
// recovered. It is a controller policy; F14 pointer hover uses a separate hit-candidate comparator.
const TARGET_RADIUS = 12;
const OBJECT_RADIUS = 7;   // gameplay-shell.mjs's own COMMAND.INTERACT nearest-object distance

const BUTTON = { A: 0, B: 1, X: 2, Y: 3, LB: 4, RB: 5, LT: 6, RT: 7, BACK: 8, START: 9, L3: 10, R3: 11, DPAD_UP: 12, DPAD_DOWN: 13, DPAD_LEFT: 14, DPAD_RIGHT: 15 };
// D-pad direction -> belt column. Retail's belt has no directional meaning, so this is simply the
// one table recorded for it; clockwise from Up.
const DPAD_BELT = { [BUTTON.DPAD_UP]: 'Digit1', [BUTTON.DPAD_RIGHT]: 'Digit2', [BUTTON.DPAD_DOWN]: 'Digit3', [BUTTON.DPAD_LEFT]: 'Digit4' };
// The right-hand skill: the keyboard's own hotkeys are F1-F3 (gameplay-model.mjs's CommandInput maps
// them to Attack/Throw/Kick -- retail's skill selection, F3), and the pad had no
// button for any of them, which left a pad-only player unable to reach a control the keyboard has.
// The control panel's skill button is the pad's other road there and it is NOT equivalent: its popup
// lists only skills whose level is above zero (skill-select.mjs's skillSelectRows), while F3's Kick is
// selectable at level 0 -- so the popup on its own cannot offer every skill the keys do. X and Y are
// the pad's face buttons A/B do not use and LT was the documented unassigned one (
// "Not yet built"); all three are free in game, and each taps the identical key its F-key produces
// (input-common.mjs's tapKey), never a gamepad-only path into the engine.
const FACE_SKILL_KEYS = { [BUTTON.X]: 'F1', [BUTTON.Y]: 'F2', [BUTTON.LT]: 'F3' };
// G9: R3 (button 11), the cursor stick's own click -- Space. G9 read Space as "the last LIVE command
// key the pad had no button for" and mapped it to attack-the-nearest-target, and **that reading is
// withdrawn (G12, measured)**: an earlier change's F3c gave Space retail's own binding, `CfgClearScreen`
// (`gameplay-shell.mjs`'s keydown handler toggles `clearScreen` and RETURNS, well above its
// `input.key(...)` call), so a Space no longer reaches CommandInput at all and R3 toggles the HUD.
// Retail's binding is the one to keep -- the Configure Controls table has
// `Clear Screen | Space`, and OpenDiablo2's key_map.go agrees -- so R3 stays on Space and now means
// what the keyboard's Space means. The pad's attack road is A at the cursor (G3), exactly the mouse's;
// `COMMAND.PRIMARY` reachable by key is now dead for the keyboard too, which is the shell's matter,
// not this module's. s308 re-proves the mapping at identity (R3 == a raw Space == one clearScreen
// toggle) and carries the measurement that the old claim is gone.
//
// The other command keys are all dead or already mapped, which is why this is the last one:
//   * KeyE -> COMMAND.INTERACT is unreachable FROM THE KEYBOARD TOO: the shell's own capture filter
//     (assets/retail-key-bindings.mjs, retail F3 -- "KeyE no longer interacts", asserted by s297)
//     stops the keydown before gameplay-shell's listener runs, so CommandInput never sees it. s308
//     measures that directly (the event is dispatched; __d2Shell.input.key is never called for the
//     keydown), and the engine's own d2_session_command_nearest_npc accepts the command it would
//     carry -- so it is the binding that is dead, not the engine.
//   * KeyT -> COMMAND.TRAVEL never reaches CommandInput either: gameplay-shell's keydown handler
//     claims KeyT for the skill tree first (also retail F3).
//   * every other key the model carries is already mapped above: WASD/arrows -> the left stick,
//     Digit1-4 -> the d-pad, F1-F3 -> X/Y/LT, Escape -> Start, and KeyI/KeyC/KeyT -> LB/RB/RT.
// So L3 (button 10) is not mapped to any of them -- a pad control that exists is not a control that
// does the retail thing. It carries SHIFT instead (G10): a modifier is
// not a key, and the shift-click it enables is a thing a pad-only player otherwise cannot do at all.
const CURSOR_STICK_CLICK_KEY = ['Space', ' '];

// G12: the two live retail keys no button of the standard mapping reaches. G11 audited the mapping
// against what the shell actually listens for and found `Tab`; the shell has since grown more keys,
// and this is that audit run again. `H` is retail's Help screen (`CfgHelp`; the shell's `helpVisible`)
// and `W` is retail's Swap Weapons (`CfgSwapWeapons`; the engine's own
// `d2_session_command_swap_weapons` -- the client swaps nothing itself). Neither has a click road, so
// on the mouse side they are keys and nothing else, and a pad-only player could not reach them at all.
//
// The standard mapping is EXHAUSTED: buttons 0-16 all carry something already (16 is Guide, which
// rejects -- the OS claims it and it cannot be read reliably), so the two remaining
// actions can only come from a second bank, and the only button that can be a modifier without also
// doing something is the one that is ALREADY one: L3, held for SHIFT (G10). Every other candidate is
// a TAP button (RT is KeyT, Back is Tab, Start is Escape), so holding it to bank would fire its own
// action on the way in -- a Help press that also opened the skill tree. L3 has no tap action at all.
// While it is held, all four d-pad directions are these four keys instead of the belt's four columns;
// the belt is what L3 WITHOUT the bank still gives. Each entry is the identical `tapKey` the physical
// key produces -- never a gamepad-only path into the engine.
//
// G12 filled Up/Down (KeyH Help, KeyW Swap Weapons). Audited again for F691: `Q` (retail's
// CfgQuestLog) and `M` (CfgMessageLog) are live in the shell -- `gameplay-shell.mjs`'s keydown handler
// toggles `questVisible`/`messageVisible` for them ABOVE its dropped-key check -- and neither has a
// pad road at all: no button of the standard mapping is free, and their only click road (the control
// panel's mini-panel icons) is unreachable because the game screen has NO free cursor under the D2R
// scheme (worldMode, G15). So a pad-only player could not open the Quest Log or the Message Log.
// Left/Right on the same bank is the fix; it costs the belt columns only while L3 is held, which is
// the modifier's own trade (Up/Down already cost columns 1 and 3).
const SHIFT_BANK_KEYS = { [BUTTON.DPAD_UP]: 'KeyH', [BUTTON.DPAD_DOWN]: 'KeyW', [BUTTON.DPAD_LEFT]: 'KeyQ', [BUTTON.DPAD_RIGHT]: 'KeyM' };

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
export function selectWorldTarget(shell) {
  const snapshot = shell.snapshot;
  if (!snapshot || !snapshot.player) return null;
  const p = snapshot.player;
  // Monsters: use the browser's nearest-world helper around the player. The mouse path supplies a
  // pointer-derived point and hit-test filter, so this player-centered choice is not its hover result.
  const unitId = selectPrimaryTarget(snapshot, p, TARGET_RADIUS);
  if (unitId) {
    const unit = (snapshot.entities || []).find((e) => e.id === unitId);
    if (unit) return { kind: 'unit', id: unitId, x: unit.x, y: unit.y };
  }
  // Ground items (mode 3, on the floor -- invPage 255 is gameplay-shell.mjs's own floor marker).
  let item = null, itemDist = Infinity;
  for (const it of snapshot.items || []) {
    if (it.mode !== 3 || it.invPage !== 255) continue;
    const d = (it.x - p.x) ** 2 + (it.y - p.y) ** 2;
    if (d <= TARGET_RADIUS * TARGET_RADIUS && d < itemDist) { itemDist = d; item = it; }
  }
  if (item) return { kind: 'item', id: item.id, x: item.x, y: item.y };
  // Interactable objects -- the same chebyshev-distance nearest-object scan COMMAND.INTERACT's own
  // handler runs (gameplay-shell.mjs), so a pad target and KeyE's own fallback agree on "nearest".
  let object = null, objectDist = OBJECT_RADIUS;
  for (const o of snapshot.objects || []) {
    const d = Math.max(Math.abs(o.x - p.x), Math.abs(o.y - p.y));
    if (d < objectDist || (d === objectDist && object && o.id < object.id)) { objectDist = d; object = o; }
  }
  if (object) return { kind: 'object', id: object.id, x: object.x, y: object.y };
  // NPCs.
  let npc = null, npcDist = Infinity;
  for (const e of snapshot.entities || []) {
    if (!e.npc) continue;
    const d = (e.x - p.x) ** 2 + (e.y - p.y) ** 2;
    if (d <= TARGET_RADIUS * TARGET_RADIUS && d < npcDist) { npcDist = d; npc = e; }
  }
  if (npc) return { kind: 'npc', id: npc.id, x: npc.x, y: npc.y };
  return null;
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
  const pts = [
    ...entries(targets.shop, 'shop', (v) => v.at),
    ...entries(targets.stashGrid, 'stash'),
    ...entries(targets.stashGold, 'stashgold'),
    ...entries(targets.inventoryGrid, 'invg'),
    ...entries(targets.inventorySlots, 'invs'),
  ];
  if (pts.length) return pts;
  // Character creation exposes the class hit boxes and EXIT target. Once a class is chosen,
  // nameGridOpen() takes over d-pad/A for text entry.
  if (targets.create) {
    const classes = entries(targets.create.classes, 'create');
    if (classes.length) return [...classes, ...entries({ exit: targets.create.exit }, 'create')];
  }
  if (targets.character && Object.keys(targets.character).length) return entries(targets.character, 'char');
  if (targets.skills && Object.keys(targets.skills).length) return entries(targets.skills, 'skill');
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
  viewport.appendChild(el);
  return { el, cells, labels, rows, index: 0, hidden: false, box };
}

function start() {
  const canvas = document.getElementById('game') || document.querySelector('canvas');
  if (!canvas) { setTimeout(start, POLL_MS); return; }   // defensive only: module scripts run after the DOM parses
  const viewport = canvas.parentElement || document.body;
  if (getComputedStyle(viewport).position === 'static') viewport.style.position = 'relative';
  const cursorEl = makeCursorEl(viewport);
  const nameGrid = makeGrid(viewport, 'controller-name-keys', NAME_ROWS, NAME_COLS, NAME_GRID_BOX);
  const goldGrid = makeGrid(viewport, 'controller-gold-keys', GOLD_ROWS, GOLD_COLS, GOLD_GRID_BOX);

  const state = { connected: false, index: -1, cursor: { x: canvas.width / 2, y: canvas.height / 2 }, lastStickWalk: 0, wasPressed: {}, shift: false,
    mode: 'cursor', worldTarget: null, panelIndex: 0, panelSig: null };

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
    // above reaches the belt (d-pad), Escape (Start), KeyI/KeyC/KeyT (LB/RB/RT) and F1-F3 (X/Y/LT)
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

  function edge(pad, i) {
    const now = Boolean(pad.buttons[i]?.pressed), was = Boolean(state.wasPressed[i]);
    state.wasPressed[i] = now;
    return now && !was;
  }

  // The name grid is up exactly while retail's own name box is: the create screen with a class chosen.
  // Everything about it is polled, so the shared shell file needs no hook (input-touch.mjs's issue 8 input
  // reads the same signal the same way). The gold grid is up exactly while the amount dialog is, read
  // from the shell's own drawing of it (`stashEvidence.gold.drawn.modal`, non-null only while open) --
  // so the pad appears when the field does and no button has to summon it.
  const nameGridOpen = () => globalThis.__d2Shell?.screen === 'create' && Boolean(globalThis.__d2Shell?.charCreateEvidence?.chosen);
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

  function renderGrid(g, visible) {
    const { el, cells } = g;
    if (!visible) { el.style.display = 'none'; return; }
    const box = { x: canvas.offsetLeft + g.box.x * canvas.offsetWidth, y: canvas.offsetTop + g.box.y * canvas.offsetHeight,
      w: g.box.w * canvas.offsetWidth, h: g.box.h * canvas.offsetHeight };
    Object.assign(el.style, { display: 'block', left: `${Math.round(box.x)}px`, top: `${Math.round(box.y)}px`,
      width: `${Math.round(box.w)}px`, height: `${Math.round(box.h)}px`, fontSize: `${Math.max(9, Math.round(box.h / g.rows.length * 0.3))}px` });
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

  function poll() {
    const pads = navigator.getGamepads ? navigator.getGamepads() : [];
    const pad = pads && Array.prototype.find.call(pads, (p) => p && p.mapping === 'standard');
    if (!pad) {
      if (state.connected) { state.connected = false; state.wasPressed = {}; cursorEl.style.display = 'none'; nameGrid.el.style.display = 'none'; }
      return;
    }
    if (!state.connected) { state.connected = true; state.index = pad.index; }

    const shell = globalThis.__d2Shell;
    const screenName = shell?.screen;

    // The two text fields (G7 name, G13 gold amount): whichever field is up shows its own grid, and a
    // field that has gone starts fresh, so its grid comes back on its own next time it appears. Read
    // here, before everything below, because a typed field owns the d-pad and the A button too (and
    // its evidence getter is a deep copy -- once a poll, not twice).
    const nameUp = nameGridOpen(), goldUp = goldGridOpen();
    if (!nameUp) nameGrid.hidden = false;
    if (!goldUp) goldGrid.hidden = false;
    // A hidden grid gives navigation back to the screen below it. Toggle before
    // choosing the input owner so the same poll can return to class snap-nav.
    const activeField = goldUp ? goldGrid : nameUp ? nameGrid : null;
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
    const panel = panelPoints(targets);
    // D2R's own world mode: the game screen with nothing modal, no snap panel and no typed field up --
    // the ONE state with no free cursor at all.
    const worldMode = screenName === 'game' && !modal && !panel && !fieldOpen;
    // What is left over keeps the pre-D2R free cursor only for screens `uiTargets()` reports nothing
    // for. Character creation's class list is now a panel too (G17); title, select, escape and NPC
    // menus were already covered by G16.
    const cursorMode = !fieldOpen && !panel && !worldMode;

    // Right stick: the pre-D2R free cursor, now only live where D2R has not specified a road yet (G1
    // as built before 2026-09-28 -- see ). Off in the world (D2R has no free cursor
    // there) and off over a snap panel (the d-pad drives the highlight instead, below).
    if (cursorMode) {
      cursorEl.style.display = 'block';
      const rx = pad.axes[2] || 0, ry = pad.axes[3] || 0;
      if (Math.hypot(rx, ry) > DEADZONE) {
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
    const target = !modal && origin && stickTarget(origin, pad.axes[0] || 0, pad.axes[1] || 0);
    const now = performance.now();
    if (target && now - state.lastStickWalk >= STICK_WALK_MS) { click(canvas, target[0], target[1]); state.lastStickWalk = now; }

    if (typedFieldUp) {
      state.mode = 'field';
      // The d-pad moves the grid's highlight. Neither the belt (there is no session on the create
      // screen for it to mean anything to, and the amount box is a modal the shell consumes every
      // digit of) nor the face skill keys are reachable from here, so the two do not both fire on
      // one press. Y above is the grid's own toggle, which is why it is not the game's F2 here.
      if (edge(pad, BUTTON.DPAD_LEFT)) moveIndex(activeField, 0, -1);
      if (edge(pad, BUTTON.DPAD_RIGHT)) moveIndex(activeField, 0, 1);
      if (edge(pad, BUTTON.DPAD_UP)) moveIndex(activeField, -1, 0);
      if (edge(pad, BUTTON.DPAD_DOWN)) moveIndex(activeField, 1, 0);
      if (edge(pad, BUTTON.A)) commitGrid(activeField, activeField === goldGrid ? goldCellCode : nameCellCode);
    } else {
      // L3 is read as a HELD modifier, not an edge: it changes what A/B mean for as long as it is
      // down, exactly as the keyboard's shift does, and the shell reads event.shiftKey per click. It
      // is also the d-pad's bank below (G12), for the same reason -- a held modifier is the only kind
      // of button that can mean a second thing without firing its own action on the way in.
      state.shift = Boolean(pad.buttons[BUTTON.L3]?.pressed);

      if (panel) {
        // D2R's snap-to-slot (G4): the d-pad snaps the highlight to whichever OTHER point in the
        // panel lies most nearly in the pressed direction (`snapMove`); A commits it -- a left click
        // there, exactly what a mouse click on that slot does -- and B a right click. A new panel, or
        // the same panel with a different point COUNT (an item picked up, a page turned), starts the
        // highlight over rather than aiming a stale index at a point that used to be something else.
        state.mode = 'panel';
        const sig = panel.map((p) => p.key).join('|');
        const panelChanged = sig !== state.panelSig;
        if (panelChanged) { state.panelSig = sig; state.panelIndex = 0; }
        if (edge(pad, BUTTON.DPAD_LEFT)) state.panelIndex = snapMove(panel, state.panelIndex, -1, 0);
        if (edge(pad, BUTTON.DPAD_RIGHT)) state.panelIndex = snapMove(panel, state.panelIndex, 1, 0);
        if (edge(pad, BUTTON.DPAD_UP)) state.panelIndex = snapMove(panel, state.panelIndex, 0, -1);
        if (edge(pad, BUTTON.DPAD_DOWN)) state.panelIndex = snapMove(panel, state.panelIndex, 0, 1);
        const cur = panel[Math.max(0, Math.min(panel.length - 1, state.panelIndex))];
        const pointChanged = state.cursor.x !== cur.x || state.cursor.y !== cur.y;
        state.cursor.x = cur.x; state.cursor.y = cur.y; placeCursor(); cursorEl.style.display = 'block';
        // F295: the escape menu keeps its own selected row for the pentagram highlight; move the same
        // DOM pointer the mouse path uses whenever snap-nav focuses one of its targets.
        if ((cur.key.startsWith('create:') || cur.key.startsWith('gamemenu:')) && (panelChanged || pointChanged)) {
          pointer(canvas, 'pointermove', cur.x, cur.y);
        }
        if (edge(pad, BUTTON.A)) click(canvas, cur.x, cur.y, { button: 0, shiftKey: state.shift });
        if (edge(pad, BUTTON.B)) click(canvas, cur.x, cur.y, { button: 2, shiftKey: state.shift });
      } else if (worldMode) {
        // D2R-style no-cursor targeting (G3): A/B dispatch a canvas click at the selected target's
        // projected point through the shell's normal click path. This proves event routing only; it
        // does not equate the controller's player-centered choice with F14 pointer-hover arbitration.
        state.mode = 'world';
        state.panelSig = null;
        const worldTarget = selectWorldTarget(shell);
        state.worldTarget = worldTarget;
        if (worldTarget && shell.project) {
          const [tx, ty] = shell.project(worldTarget.x, worldTarget.y);
          state.cursor.x = tx; state.cursor.y = ty; placeCursor(); cursorEl.style.display = 'block';
          if (edge(pad, BUTTON.A)) click(canvas, tx, ty, { button: 0, shiftKey: state.shift });
          if (edge(pad, BUTTON.B)) click(canvas, tx, ty, { button: 2, shiftKey: state.shift });
        } else {
          // Nothing in range: D2R shows no target, and there is nowhere for A/B to click -- consume
          // the edge so a target that appears on the NEXT poll does not fire on a stale press.
          cursorEl.style.display = 'none';
          edge(pad, BUTTON.A); edge(pad, BUTTON.B);
        }
      } else {
        // The pre-D2R free cursor, for every screen the D2R research has not specified yet (title,
        // character select, the escape menu, the NPC menu).
        state.mode = 'cursor';
        state.panelSig = null;
        if (edge(pad, BUTTON.A)) click(canvas, state.cursor.x, state.cursor.y, { button: 0, shiftKey: state.shift });
        if (edge(pad, BUTTON.B)) click(canvas, state.cursor.x, state.cursor.y, { button: 2, shiftKey: state.shift });
      }

      // D-pad: the belt's columns (G3) -- or, while L3 is held, the two keys the mapping has no
      // button left for (G12) -- only while no snap panel has claimed the d-pad for navigation above.
      if (!panel) {
        for (const button of [BUTTON.DPAD_UP, BUTTON.DPAD_RIGHT, BUTTON.DPAD_DOWN, BUTTON.DPAD_LEFT]) {
          const code = (state.shift && SHIFT_BANK_KEYS[button]) || DPAD_BELT[button];
          if (edge(pad, button)) tapKey(code);
        }
      }
      // X / Y / LT: the right-hand skill, the keyboard's F1-F3 (G8) -- unaffected by the D2R rework,
      // the button table is not yet settled.
      for (const [button, code] of Object.entries(FACE_SKILL_KEYS)) if (edge(pad, Number(button))) tapKey(code);
      // R3: Space, retail's CfgClearScreen (G12). Gated on the game screen like the shell's own
      // binding, which returns early outside one.
      if (screenName === 'game' && edge(pad, BUTTON.R3)) tapKey(...CURSOR_STICK_CLICK_KEY);
    }
    // Start / Back / shoulders / trigger -- unaffected by the D2R rework.
    if (edge(pad, BUTTON.START)) tapKey('Escape');
    if (edge(pad, BUTTON.BACK)) tapKey('Tab', 'Tab');
    if (edge(pad, BUTTON.LB)) tapKey('KeyI');
    if (edge(pad, BUTTON.RB)) tapKey('KeyC');
    if (edge(pad, BUTTON.RT)) tapKey('KeyT');

    renderGrid(nameGrid, nameUp && !nameGrid.hidden);
    renderGrid(goldGrid, goldUp && !goldGrid.hidden);
  }
  setInterval(poll, POLL_MS);
}

if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', start, { once: true });
else start();
