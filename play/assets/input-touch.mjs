// Touch is an input layer: every gesture here ends as one of
// the SAME DOM pointer or key events a mouse and a keyboard produce, dispatched through
// input-common.mjs's proven helpers, so play/gameplay-shell.mjs's own handlers decide what happens
// -- nothing about the game is re-implemented here.
//
// TOUCH IS THE MOUSE'S BUTTONS, LIVE ("Control scheme"). The game is already playable with a mouse
// alone, so touch invents no vocabulary of its own on top of the mouse's gestures:
//
//   * ONE FINGER is the LEFT button, live. Its press is sent when the finger lands (aimed by the
//     belt, a panel grid or the character screen's "+", see below), every move while it is down is
//     sent as a move, and its lift is sent as the release. A held finger is therefore a held button:
//     the shell's own hold-to-repeat reads e.buttons, exactly as it does for a mouse.
//   * TWO FINGERS are the RIGHT button, held. The second finger's press sends a right button down at
//     the aiming finger's point (the point the player is pointing with), with the contextmenu the
//     browser sends for a real right click; the right button stays down until the SECOND finger
//     lifts. No hold timer: "press the right button" has no timing in it.
//   * A THIRD finger is swallowed: it sends nothing and reaches nothing.
//   * A SHIFT REST finger (row 2) is a finger that lands in the console gap of the bottom band, on
//     nothing the shell can act on (control-panel.mjs's isHudDeadZone). It sends nothing itself. While
//     it is down, every other finger's events carry shiftKey, which is the mouse's held shift: retail's
//     shift-click (0x63) is the only thing that puts an item into the belt. A phone has no shift key,
//     and a rest finger is the one thing a thumb can hold while the other thumb aims.
//   * PRESS/LIFT CANCEL: a pointercancel sends the release the lift would have sent, so the shell never
//     keeps a button down that the finger no longer holds.
//
// The canvas's touch events are held back from the shell (a capture-phase listener above the canvas,
// stopping the real event before the shell's own bubble-phase listener sees it) for the things a raw
// event cannot carry: the finger's AIM (M14, M15, M16), because the game's own 29x29 controls are 19.9
// CSS px of glass on a phone and a finger's own error is wider than they are -- the belt's boxes with
// their 2-px gutters, the open panels' grid cells, the character screen's "+" (see beltAim, gridAim and
// statAim below); the right button and the shift state, which ride the same replayed events. Everything
// this module dispatches goes through input-common's own pointer()/click()/tapKey() -- the exact calls
// a mouse and a keyboard already produce -- so a row comparing a touch path against a mouse or keyboard
// path is comparing two runs of the SAME code, not two implementations of the same idea.
//
// There is no on-screen chrome. The phone's controls are the game's own: the belt, the panels and the
// bottom band's buttons are retail's, and touch reaches them with the gestures above. The one element
// this module owns in the page is the hero-name input, shown only where retail shows its name box.

import { click, pointer, tapKey, toClient, toCanvas } from './input-common.mjs';
import { isHudDeadZone, hudAimPoint } from './control-panel.mjs';
import { hudOrigin } from './hud-origin.mjs';

// Overlay face: the game's serif, not the browser's monospace (no web font ships).
export const OVERLAY_FONT = 'Palatino Linotype, Palatino, Georgia, serif';

// The scripted seam's keys that the touch layer names by their key name (what a physical key sends).
const KEY_NAMES = { KeyI: '', KeyC: '', KeyT: '', Escape: 'Escape' };

function injectStyle(documentObject) {
  if (documentObject.getElementById('d2-touch-style')) return;
  const style = documentObject.createElement('style');
  style.id = 'd2-touch-style';
  style.textContent =
    '#touch-overlay{position:absolute;inset:0;pointer-events:none}' +
    '#touch-overlay .d2-touch-hit{pointer-events:none}' +
    '#touch-overlay.d2-touch-on .d2-touch-hit{pointer-events:auto}' +
    '#d2-touch-name{position:absolute;left:39.75%;top:82.2%;width:27.5%;height:5.2%;box-sizing:border-box;' +
      'font:16px ' + OVERLAY_FONT + ';padding:2px 6px;background:#1a170f;color:#e8d9ab;border:1px solid #493d27;touch-action:none}';
  documentObject.head.appendChild(style);
}

/**
 * Mount the touch input layer on the page: the canvas's live finger handling (one finger left, two
 * fingers right, a shift rest in the console gap) and the name-entry bridge. Safe to call once per
 * page; a page with none of the expected markup (#game, #touch-overlay) is a no-op.
 */
export function installTouchInput({ documentObject = globalThis.document, windowObject = globalThis.window } = {}) {
  const canvas = documentObject.getElementById('game');
  const container = documentObject.getElementById('touch-overlay');
  if (!canvas || !container) return null;

  injectStyle(documentObject);
  canvas.style.touchAction = 'none';   // M3: a touch drag on the canvas walks the hero, never the page

  let revealed = false;
  function reveal() { if (!revealed) { revealed = true; container.classList.add('d2-touch-on'); } }
  if (windowObject.matchMedia?.('(pointer: coarse)')?.matches) reveal();

  // Events THIS module redispatches at the canvas must not be read back as a new gesture.
  // element.dispatchEvent() is synchronous -- every listener runs before it returns -- so reserving
  // the pointerId only around that call is enough.
  const replaying = new Set();
  function guarded(id, dispatch) {
    replaying.add(id);
    try { dispatch(); } finally { replaying.delete(id); }
  }
  // A whole click (move, down, up, and the contextmenu for button 2): the scripted seam's gesture.
  function replay(x, y, opts) { guarded(opts.pointerId ?? 1, () => own(() => click(canvas, x, y, opts))); }
  // One live pointer event: a finger's press, move or release, as it happens.
  function send(type, x, y, opts) { guarded(opts.pointerId, () => pointer(canvas, type, x, y, opts)); }
  // The contextmenu a real right click's press produces. It is sent at the press, not after the release
  // (click() sends it after the release for a whole click): the right button is held, and the browser
  // sends its menu event on the press.
  function sendContextMenu(x, y, shiftKey) {
    const [clientX, clientY] = toClient(canvas, x, y);
    own(() => canvas.dispatchEvent(new MouseEvent('contextmenu', { clientX, clientY, button: 2, shiftKey, bubbles: true, cancelable: true })));
  }
  // The contextmenu events this module sends itself (its right clicks), told apart from the browser's.
  let owning = 0;
  function own(dispatch) { owning++; try { return dispatch(); } finally { owning--; } }

  // ---- M14: a finger is wider than the belt's own boxes ----------------------------------------
  // Retail's HUD is laid out for a mouse at 800x600 and the belt's boxes are Belts.txt's own
  // rectangles -- play/gameplay-shell.mjs hit-tests them through the engine's accessor, and this
  // module does not change them. What it does change is where a FINGER is allowed to land: measured
  // at the phone floor (915x412 CSS, dpr 2.625), the frame scales to 0.6867 CSS px per game px, so a
  // 29x29 box is 19.9x19.9 CSS px of glass (3.1 mm) against the 44 px (6.8 mm) this overlay holds its
  // own controls to -- and Belts.txt's boxes do not even tile, they leave a 2-game-px (1.37 CSS px)
  // GUTTER between neighbours. A real OS-level touch 10.3 CSS px off a full box's centre lands in
  // that gutter and the shell's click path falls straight through to the WORLD: measured on the
  // pre-fix build (s372), the finger drank NOTHING and the hero walked (5663,5813 -> 5668,5817).
  // So a touch gesture landing within half a cell of the belt row is delivered at the centre of the
  // nearest box instead -- the finger's aim, not a rewritten hit test:
  //   * the MOUSE is untouched (the shell's own hit test still gets the exact point: s372 clicks the
  //     same gutter with a real mouse and still gets the world click),
  //   * the SCRIPTED seam is untouched (input-touch's exported tap()/longPress(), which
  //     lib/player.mjs's S000_INPUT=touch and s315's 112-point mouse/touch identity sweep dispatch
  //     through, still delivers the raw point -- an instrument that knows where it is pointing).
  // Half a cell is the width of the rule, not a taste: it is the distance at which the neighbouring
  // box is as close as this one, so no point inside the band has a box "in front of" another.
  function beltAim(cx, cy) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen) return null;
    const boxes = shell.beltHudBoxes;
    if (!boxes || !boxes.length) return null;
    // The boxes are retail 800x600 coordinates on the bottom anchor, as the shell draws them; aim in that space.
    const o = hudOrigin(canvas.width, canvas.height, 'bottom'), x = (cx - o.x) / o.scale, y = (cy - o.y) / o.scale;
    let left = Infinity, right = -Infinity, top = Infinity, bottom = -Infinity;
    for (const b of boxes) {
      if (b.left < left) left = b.left;
      if (b.right > right) right = b.right;
      if (b.top < top) top = b.top;
      if (b.bottom > bottom) bottom = b.bottom;
    }
    const half = (bottom - top) / 2;
    if (x < left - half || x > right + half || y < top - half || y > bottom + half) return null;
    let best = boxes[0], bestDistance = Infinity;
    for (const b of boxes) {
      const distance = Math.abs((b.left + b.right) / 2 - x);
      if (distance < bestDistance) { bestDistance = distance; best = b; }
    }
    return [o.x + (best.left + best.right) / 2 * o.scale, o.y + (top + bottom) / 2 * o.scale];
  }

  // ---- M15: the same aim, one panel deeper -- the open grids --------------------------------------
  // The belt's finding generalises to the game's OTHER 29x29 controls, and the grid cells are exactly
  // that class: measured at the phone floor (915x412 CSS, dpr 2.625, 0.6867 CSS px per game px), the
  // inventory's pack grid is Inventory.txt's own `10x4 of 29x29 at 419,315` and the LoD stash is
  // `6x8 of 29x29 at 154,142` -- 19.9x19.9 CSS px (3.1 mm) cells, against the 44 px (6.8 mm) this
  // overlay holds its own controls to (s316) and a finger's own aim error of ~10 CSS px (15 game px).
  // What a mis-aim COSTS is different from the belt's, and measuring it was the whole point:
  //   * the grids TILE -- neighbouring cells share an edge, there is no gutter (the belt's boxes leave
  //     a 2-game-px one). A tap on the shared edge resolves to one of the two cells, and the mouse at
  //     the identical point resolves to the same one, so no tap in the grid's INTERIOR is ever lost.
  //   * the outer half-cell RING around the grid is where the aim error lands, and retail's own hit
  //     test finds nothing there: the panel's own `contains` swallows it, so the tap does NOTHING --
  //     silently. Measured pre-fix (s377): a finger 5, 10 or 13 game px (3.4, 6.9, 8.9 CSS px) left of
  //     the pack grid's own left edge, or 9 game px above its top row, with a full cell under the
  //     finger's own intent, left the cursor empty and the item where it was; with an item HELD the
  //     same tap left it on the cursor. The belt lost the drink and walked the hero; the grid loses the
  //     pickup, and the player is told nothing either way.
  // So the same rule is applied to the panel grids the shell's own hit test is handed:
  //   * OUTSIDE the grid only, within half a cell of it, from the shell's own rectangles
  //     (`inventoryLayout` / `stashGeometry`, the numbers the renderers' itemAt/cellAt get) -- half a
  //     cell again being the tie distance. A point already inside the grid is delivered RAW, because a
  //     grid's rectangle holds other controls than its cells (the stash's gold dialog draws its own
  //     up/down/ok/close boxes over the stash's grid, s314), and the shell's own hit test must keep
  //     deciding those exactly as before;
  //   * bounded by the PANEL's own rect (the shell's `inventoryPanelRect` / `stashPanelRect`, the
  //     rectangles their own `contains` tests): the finger's aim never reaches a point the panel does
  //     not own. The inventory's band is the reason that bound is stated -- the grid's right edge
  //     (709) is 11 game px from the panel's (720), so an unbounded half-cell band would reach
  //     3.5 game px into the world outside it;
  //   * and the MOUSE and the SCRIPTED seam are untouched, exactly as for the belt.
  function gridAim(x, y, grid, panel) {
    const halfW = grid.cellWidth / 2, halfH = grid.cellHeight / 2;
    const right = grid.left + grid.columns * grid.cellWidth, bottom = grid.top + grid.rows * grid.cellHeight;
    // INSIDE the grid the point is delivered exactly where it landed: the shell's own hit test decides
    // there, and it is not only the grid's cells that live inside a grid's rectangle -- the stash's gold
    // dialog draws its own up/down/ok/close controls over the stash's grid (s314 works them by finger),
    // so moving an interior point would take a control that is not the grid's. Only the ring moves.
    if (x >= grid.left && x < right && y >= grid.top && y < bottom) return null;
    if (x < grid.left - halfW || x > right + halfW || y < grid.top - halfH || y > bottom + halfH) return null;
    if (x < panel.left || x >= panel.left + panel.width || y < panel.top || y >= panel.top + panel.height) return null;
    const column = Math.min(grid.columns - 1, Math.max(0, Math.round((x - grid.left) / grid.cellWidth - 0.5)));
    const row = Math.min(grid.rows - 1, Math.max(0, Math.round((y - grid.top) / grid.cellHeight - 0.5)));
    return [grid.left + (column + 0.5) * grid.cellWidth, grid.top + (row + 0.5) * grid.cellHeight];
  }
  function panelAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen) return null;
    // The stash first: retail opens the inventory beside it, so both grids can be up at once.
    const stash = shell.stashGeometry, stashPanel = shell.stashPanelRect;
    if (stash && stashPanel) {
      const hit = gridAim(x, y, { left: stash.gridLeft, top: stash.gridTop, columns: stash.columns, rows: stash.rows,
        cellWidth: stash.boxW, cellHeight: stash.boxH }, stashPanel);
      if (hit) return hit;
    }
    const inv = shell.inventoryLayout, invPanel = shell.inventoryPanelRect;
    if (shell.inventoryVisible && inv?.grid && invPanel) {
      const hit = gridAim(x, y, inv.grid, invPanel);
      if (hit) return hit;
    }
    return null;
  }

  // ---- M16: the character screen's point-spending "+" buttons --------------------------------------
  // M14's rule, on the third class of retail control a finger has to hit, and measured before it was
  // applied (s426, real OS-level touches at the phone floor, 915x412 CSS / dpr 2.625 / 0.6867 CSS px
  // per game px): the four "+" are 30x30 game px = 20.6x20.6 CSS px = 3.2 mm of glass, and the defect
  // is the belt's own shape one control class over -- a finger 15 game px (10.3 CSS px) off the centre
  // spends 'world' instead of a point, and the hero WALKS (measured: 5.1 subtiles), so the miss costs
  // the point AND an unintended walk. The buttons do not exist at all until a point is waiting
  // (`characterPanelEvidence` carries none while statPoints is 0 and the shell's buttonAt() answers
  // null), which is why the row opens the screen on a character that has one.
  //   * OUTSIDE a button only, within HALF a button of it (15 game px, the tie distance M14 and M15
  //     both use), and from the shell's own drawn record -- `characterPanelEvidence.buttons`, the
  //     rectangles its own buttonAt() hit-tests, so nothing here re-derives the layout. A point
  //     already inside a button is delivered RAW: the shell's own hit test decides it, exactly as
  //     before, and the buttons are separated by 32 game px vertically (141/203/289/351), so no two
  //     rings can overlap and no ring reaches a neighbouring button.
  //   * The ring is bounded by the buttons themselves rather than by a panel rectangle, because the
  //     character screen's own rect is not a shell accessor and need not be: half a button reaches at
  //     most x 190..250, y 126..396, and the panel art runs 80..~400 x 61..~502 (the frames end 4 game
  //     px left of a button, the next control below is the mini-panel at y 531) -- the ring never
  //     leaves the panel it belongs to. s426 asserts that clearance, so a future layout that moved a
  //     button to the panel's edge fails the row instead of quietly walking the hero.
  //   * The MOUSE and the SCRIPTED seam are untouched, exactly as for the belt and the grids: the miss
  //     that spends the world is retail's own hit test, and the row measures it.
  function statAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen || !shell.characterVisible) return null;
    const buttons = shell.characterPanelEvidence?.buttons;
    if (!buttons) return null;
    for (const b of buttons) {
      const r = b.rect;
      if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return null;   // inside: raw
      if (x < r.x - r.w / 2 || x > r.x + r.w + r.w / 2 || y < r.y - r.h / 2 || y > r.y + r.h + r.h / 2) continue;
      return [r.x + r.w / 2, r.y + r.h / 2];
    }
    return null;
  }
  // ---- M17: the HUD's own small controls ---------------------------------------------------------
  // The run button (16x20 game px), the menu arrow and the mini-panel icons (18x18) are 11-14 CSS px of
  // glass at the phone floor, under a finger's own aim error, and a miss walks the hero. A finger that
  // lands NEAR one (control-panel.mjs's hudAimPoint: within HUD_FINGER_REACH, nearest wins) presses its
  // centre; a point already on a control, a belt box or an open panel is delivered raw. Off while the
  // skill popup is up (its icons sit over the band). The mouse and the scripted seam are untouched.
  function hudAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen || shell.skillSelectOpen) return null;
    const hud = shell.controlPanelEvidence?.hud;
    if (!hud) return null;
    const extra = [];
    if (shell.inventoryVisible && shell.inventoryPanelRect) extra.push(shell.inventoryPanelRect);
    if (shell.stashPanelRect) extra.push(shell.stashPanelRect);
    return hudAimPoint(x, y, { width: canvas.width, height: canvas.height, hud, belt: shell.beltHudBoxes || [], extra });
  }
  // The point a finger's aim snaps to, or null when the finger is on nothing the aim rules cover.
  function snapAim(x, y) { return beltAim(x, y) || panelAim(x, y) || statAim(x, y) || hudAim(x, y); }
  // A finger's aim: the belt's boxes, an open panel's grid cells, the character screen's "+", else
  // exactly where it landed.
  function aim(x, y) { return snapAim(x, y) || [x, y]; }

  // ---- row 2: the shift rest finger's ground ------------------------------------------------------
  // A finger that lands on nothing the aim rules cover, in the console gap of the bottom band, on no
  // control the frame last placed, is a rest finger (control-panel.mjs's isHudDeadZone owns the
  // geometry and the list of live boxes). The shell's own boxes are read here, not re-derived: the
  // inventory and stash rects are live while open (they overlap the band's top rows), and a panel that
  // covers the band itself -- the character screen, the skill tree, the skill popup -- leaves no rest
  // ground at all. Off the game screen there is no HUD to rest on.
  function restGround(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen) return false;
    const extra = [];
    if (shell.inventoryVisible && shell.inventoryPanelRect) extra.push(shell.inventoryPanelRect);
    if (shell.stashPanelRect) extra.push(shell.stashPanelRect);
    return isHudDeadZone(x, y, {
      width: canvas.width, height: canvas.height,
      hud: shell.controlPanelEvidence?.hud || {},
      belt: shell.beltHudBoxes || [],
      extra,
      panelOpen: Boolean(shell.characterVisible || shell.skillsVisible || shell.skillSelectOpen),
    });
  }

  // ---- the live finger table -----------------------------------------------------------------------
  // pointerId -> one of
  //   { role: 'aim', x, y }        the LEFT button, held: x, y is the finger's latest canvas point
  //                                (pending: true until its press is sent -- see land(); released: true
  //                                once a second finger took the press over: its left button is up, or
  //                                never went down, so its moves and its lift send nothing);
  //   { role: 'second', ax, ay }   the RIGHT button, held at (ax, ay), the aiming finger's point;
  //   { role: 'rest' }             a shift rest finger: sends nothing, makes every other event shift;
  //   { role: 'swallowed' }        a third finger while a pair is down: sends nothing, reaches nothing.
  const holds = new Map();
  const restDown = () => [...holds.values()].some((h) => h.role === 'rest');
  let lastTouch;   // when a finger last lifted: the browser's long-press menu guard reads it

  // The landing window. A mouse cannot press two buttons at the same instant, but two fingers land
  // tens of milliseconds apart (a natural two-finger tap on the emulator's Chrome: 40 ms), and a left
  // press sent at the first one has already walked the hero by the time the second says "right click".
  // So the first finger's press waits LAND_MS from its landing -- checked by flush() below, which the
  // shell calls at the top of every update() -- or until the finger lifts, or moves farther than
  // LAND_SLOP (a jitter of the landing finger is not a drag). A second finger inside the window makes
  // the gesture a pure right click: the left press is never sent. The press, when it goes, is a
  // mouse's: the cursor moves to where the finger landed, then the button goes down there. Without a
  // clock (a host with no performance.now) the window has no length: the first flush, move or lift
  // lands the press.
  const LAND_MS = 60;     // past the 40 ms between one tap's two fingers; a held walk still starts within two 40 ms frames
  const LAND_SLOP = 12;   // game px, ~8 CSS px on a phone: a platform's own touch slop
  const clock = () => windowObject.performance?.now?.();
  function inWindow(h) {
    const t = clock();
    return h.t !== undefined && t !== undefined && t - h.t < (h.item ? HOLD_MS : LAND_MS);
  }
  // ---- press-and-hold on an item is the RIGHT button ----------------------------------------------
  // A potion, a scroll or a tome is USED with a right click, and the two-finger right click needs one
  // finger on a 20 CSS px cell while a second lands -- awkward on a phone. So a finger that lands on
  // the belt or an open item grid (inventory, stash, cube) waits HOLD_MS before its left press: lifted
  // sooner it is a plain tap (pick up / put down), dragged past the slop it is a left drag, and held
  // still that long it is a right click at the aimed point (the button stays down until the lift).
  // In the world a held finger keeps walking: nothing changes there.
  const HOLD_MS = 400;
  function inRect(x, y, l, t, w, h) { return x >= l && x < l + w && y >= t && y < t + h; }
  function itemGround(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen) return false;
    const stash = shell.stashGeometry;
    if (stash && shell.stashPanelRect && inRect(x, y, stash.gridLeft, stash.gridTop, stash.columns * stash.boxW, stash.rows * stash.boxH)) return true;
    const g = shell.inventoryVisible && shell.inventoryLayout?.grid;
    if (g && inRect(x, y, g.left, g.top, g.columns * g.cellWidth, g.rows * g.cellHeight)) return true;
    const boxes = shell.beltHudBoxes;
    if (boxes?.length) {
      const o = hudOrigin(canvas.width, canvas.height, 'bottom'), bx = (x - o.x) / o.scale, by = (y - o.y) / o.scale;
      if (boxes.some((b) => bx >= b.left && bx < b.right && by >= b.top && by < b.bottom)) return true;
    }
    return Boolean(beltAim(x, y));
  }
  function holdRight(pointerId, h) {
    h.pending = false;
    h.released = true;
    h.right = true;
    const [ax, ay] = aim(h.x, h.y);
    const shiftKey = restDown();
    send('pointermove', ax, ay, { button: 0, pointerType: 'touch', pointerId, shiftKey });
    send('pointerdown', ax, ay, { button: 2, pointerType: 'touch', pointerId, shiftKey });
    sendContextMenu(ax, ay, shiftKey);
  }
  function land(pointerId, h) {
    if (!h.pending) return;
    h.pending = false;
    const [ax, ay] = aim(h.x, h.y);
    const opts = { button: 0, pointerType: 'touch', pointerId, shiftKey: restDown() };
    send('pointermove', ax, ay, opts);
    send('pointerdown', ax, ay, opts);
  }
  function flush() {
    for (const [id, h] of holds) {
      if (h.role !== 'aim' || h.released || inWindow(h)) continue;
      if (h.item && h.pending && h.t !== undefined) holdRight(id, h); else land(id, h);
    }
  }

  // A finger leaves (pointerup): the release its press opened. The browser takes it (pointercancel):
  // the press is called off, not completed -- an interrupted touch (the notification shade, an app
  // switch, a palm) must not activate the control under it, so the shell gets a cancel, never an up.
  // A press still in its landing window never went out, so a cancelled one sends nothing at all.
  function lift(pointerId, cancelled = false) {
    lastTouch = clock();
    const h = holds.get(pointerId);
    holds.delete(pointerId);
    if (!h) return;
    if (h.role === 'aim' && !h.released && cancelled && h.pending) return;
    if (h.role === 'aim' && h.right) {
      const [ax, ay] = aim(h.x, h.y);
      send('pointerup', ax, ay, { button: 2, pointerType: 'touch', pointerId, shiftKey: restDown() });
      return;
    }
    if (h.role === 'aim' && !h.released) land(pointerId, h);   // a tap inside the window: move, down, up
    const shiftKey = restDown();
    if (h.role === 'aim' && !h.released) {
      const [ax, ay] = aim(h.x, h.y);
      send(cancelled ? 'pointercancel' : 'pointerup', ax, ay, { button: 0, pointerType: 'touch', pointerId, shiftKey });
    } else if (h.role === 'second') {
      send('pointerup', h.ax, h.ay, { button: 2, pointerType: 'touch', pointerId, shiftKey });
    }
  }

  documentObject.addEventListener('pointerdown', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    reveal();   // "the first touch", wherever it lands
    // The primary pointer means no other finger is on the glass: any hold still recorded lost its
    // pointerup (a page that lost focus mid-touch, a browser that dropped the lift). Call each off, so
    // a stale aim cannot turn this finger into a right click, nor a stale rest hold Shift down forever.
    if (e.isPrimary === true) for (const id of [...holds.keys()]) lift(id, true);
    if (e.target !== canvas) return;
    e.stopPropagation();
    if (e.cancelable) e.preventDefault();
    const [x, y] = toCanvas(canvas, e.clientX, e.clientY);
    // A finger the aim rules claim is a click; otherwise a finger in the console gap is a rest.
    if (!snapAim(x, y) && restGround(x, y)) { holds.set(e.pointerId, { role: 'rest' }); return; }
    // A pair is already down: a third finger is not a third button. It is swallowed, so it can neither
    // press anything of its own nor turn the right click into anything else.
    // Only while the pair's right button is down: a finger left over from a lifted pair must not
    // swallow every finger after it.
    if ([...holds.values()].some((h) => h.role === 'second')) {
      holds.set(e.pointerId, { role: 'swallowed' });
      return;
    }
    const aiming = [...holds.entries()].find(([, h]) => h.role === 'aim');
    if (aiming?.[1].right) { holds.set(e.pointerId, { role: 'swallowed' }); return; }
    if (aiming) {
      // The second finger: the right button goes down where the aiming finger is pointing.
      const [ax, ay] = aim(aiming[1].x, aiming[1].y);
      const shiftKey = restDown();
      // A mouse has one pointer: the left button comes up before the right goes down, or the held
      // left keeps walking the hero through the whole right click.
      if (aiming[1].pending) {
        // Inside the landing window: the left press never went out, so nothing comes up; the cursor
        // moves to the aim point as a mouse's would before its right button goes down.
        send('pointermove', ax, ay, { button: 0, pointerType: 'touch', pointerId: aiming[0], shiftKey });
      } else if (!aiming[1].released) {
        send('pointerup', ax, ay, { button: 0, pointerType: 'touch', pointerId: aiming[0], shiftKey });
      }
      aiming[1].pending = false;
      aiming[1].released = true;
      send('pointerdown', ax, ay, { button: 2, pointerType: 'touch', pointerId: e.pointerId, shiftKey });
      sendContextMenu(ax, ay, shiftKey);
      holds.set(e.pointerId, { role: 'second', ax, ay });
      return;
    }
    // The first finger: the left button goes down, aimed, once the landing window closes (land()).
    holds.set(e.pointerId, { role: 'aim', x, y, pending: true, t: clock(), item: itemGround(x, y) });
  }, { capture: true });

  documentObject.addEventListener('pointermove', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    const h = holds.get(e.pointerId);
    if (!h) return;
    e.stopPropagation();   // the other fingers' moves reach nothing: they are not the shell's
    if (h.role !== 'aim') return;
    const [x, y] = toCanvas(canvas, e.clientX, e.clientY);
    if (h.released) {
      // The right button is held by a second finger: the aiming finger is still the mouse, and a mouse
      // moved with its right button held steers the held cast (the shell's own right-held retarget).
      // With no right button held (the second finger lifted) it sends nothing, but it still points:
      // the next second finger's right click goes where it is now, not where it was.
      h.x = x; h.y = y;
      const held = [...holds.entries()].find(([, o]) => o.role === 'second');
      if (!held) return;
      const [ax, ay] = aim(x, y);
      held[1].ax = ax; held[1].ay = ay;
      send('pointermove', ax, ay, { button: 2, pointerType: 'touch', pointerId: held[0], shiftKey: restDown() });
      return;
    }
    // A jitter inside the landing window keeps the press waiting, at the point the finger landed.
    if (h.pending && inWindow(h) && Math.hypot(x - h.x, y - h.y) <= LAND_SLOP) return;
    land(e.pointerId, h);   // the press goes where the finger landed, before the drag
    h.x = x; h.y = y;
    const [ax, ay] = aim(x, y);
    send('pointermove', ax, ay, { button: 0, pointerType: 'touch', pointerId: e.pointerId, shiftKey: restDown() });
  }, { capture: true });

  documentObject.addEventListener('pointerup', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    if (!holds.has(e.pointerId)) return;
    e.stopPropagation();
    lift(e.pointerId);
  }, { capture: true });

  documentObject.addEventListener('pointercancel', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    if (!holds.has(e.pointerId)) return;
    e.stopPropagation();   // the shell sees the cancel this layer sends, at the aimed point, once
    lift(e.pointerId, true);
  }, { capture: true });

  // The page lost focus or went to the background (a call, the app switcher, a notification shade):
  // a finger then on the glass never reports its lift. Every hold is called off now, as a cancel, so
  // nothing stale survives into the next touch -- no Shift left armed by a rest finger, no aim that
  // would make the next finger a right click. A held second finger ends with its button-2 up, the
  // shell's path that also ends a held cast.
  const callOff = () => { for (const id of [...holds.keys()]) lift(id, true); };
  windowObject.addEventListener?.('blur', callOff);
  documentObject.addEventListener('visibilitychange', () => { if (documentObject.visibilityState === 'hidden') callOff(); });

  // A finger held still is a long-press to the browser, and Chrome for Android answers it with a
  // contextmenu of its own (~500 ms in, while a hold-walk or a slow tap is still down). The shell reads
  // every contextmenu at the canvas as the right button -- in the inventory that uses the item under
  // the cursor -- so the browser's must never reach it: a touch's right button is the second finger,
  // and this layer sends that menu itself. A mouse's or pen's contextmenu passes untouched. One without
  // a pointerType (an older browser's MouseEvent) is a finger's while a finger is down, or within a
  // second of the last one lifting.
  function touchedRecently() {
    const t = clock();
    return lastTouch !== undefined && t !== undefined && t - lastTouch < 1000;
  }
  documentObject.addEventListener('contextmenu', (e) => {
    if (owning) return;
    const touch = e.pointerType ? e.pointerType === 'touch' : holds.size > 0 || touchedRecently();
    if (!touch) return;
    e.stopImmediatePropagation?.();
    e.stopPropagation?.();
    if (e.cancelable !== false) e.preventDefault?.();
  }, { capture: true });

  // ---- M9 (row 2): the shift that a finger has no way to hold -----------------------------------
  // Retail's shift-click (0x63) is the only thing that puts an item into the belt, and the shell reads
  // it from `e.shiftKey` alone (play/gameplay-shell.mjs's inventory branch). The rest finger is the
  // held shift: while it is down, every other finger's press, move and release carry shiftKey, exactly
  // as a held physical shift rides a mouse click. Nothing is latched: lift the rest finger and the next
  // tap is a plain tap.

  // ---- M2 (row 3): the keyboard keys a touch player reaches the game with -------------------------
  // touchKey is the scripted seam's key (lib/player.mjs's key()) and the name box's forwarding. A key a
  // physical keyboard would send is carried as the SAME event, through input-common's own tapKey(): the
  // touch layer produces the keyboard's event, exactly as it produces the mouse's click. A belt digit
  // has no key on a phone, so it is the retail HUD box it names, clicked where that box is drawn.
  function touchKey(code, keyName = '') {
    const digit = /^Digit([1-4])$/.exec(code);
    if (digit) {
      const boxes = windowObject.__d2Shell?.beltHudBoxes || [];
      const slot = Number(digit[1]) - 1;
      const box = boxes[slot];
      if (!box) throw new Error(`S000_INPUT=touch: belt slot ${digit[1]} has no retail HUD box`);
      const origin = windowObject.__d2Shell.beltEvidence?.origin || { x: 0, y: 0 };
      replay(origin.x + (box.left + box.right) / 2, origin.y + (box.top + box.bottom) / 2,
        { button: 0, pointerType: 'touch', pointerId: 9101 });
      return true;
    }
    // I, C, T and Escape keep their retail key names (Escape's is 'Escape').
    if (code in KEY_NAMES) { tapKey(code, KEY_NAMES[code]); return true; }
    // A letter typed into the hero's name (s000a step 3), a Backspace correcting it, or any other key a
    // physical keyboard would send is carried as the same event, through tapKey().
    tapKey(code, keyName);
    return true;
  }

  // ------------- issue 8: a focused input that opens the phone keyboard, forwarding to typeKey ----
  const nameInput = documentObject.createElement('input');
  nameInput.type = 'text'; nameInput.id = 'd2-touch-name'; nameInput.className = 'd2-touch-hit';
  nameInput.autocomplete = 'off'; nameInput.autocapitalize = 'off'; nameInput.spellcheck = false;
  nameInput.maxLength = 15; nameInput.style.display = 'none';
  container.appendChild(nameInput);

  // A phone keyboard's own keydown is unreliable (predictive text and IME composition report
  // "Unidentified" on many devices), so what is forwarded is the INPUT's value, diffed against what
  // it held a moment ago and replayed a character at a time through the SAME tapKey a physical key
  // uses -- charcreate.mjs's typeKey does the validating and the 15-character cap, once, either way.
  let prevValue = '';
  function forwardNameDiff(oldValue, newValue) {
    if (newValue.startsWith(oldValue)) { for (const ch of newValue.slice(oldValue.length)) tapKey('', ch); }
    else if (oldValue.startsWith(newValue)) { for (let i = oldValue.length; i > newValue.length; i--) tapKey('Backspace', 'Backspace'); }
    else { for (let i = 0; i < oldValue.length; i++) tapKey('Backspace', 'Backspace'); for (const ch of newValue) tapKey('', ch); }
  }
  // The box then shows what the hero's name actually took, never what was typed: a character the game
  // refuses (an emoji, a letter outside its font) would otherwise stay in the box, and the next
  // Backspace there would delete a real letter from the name. A real keydown the shell already took
  // (a hardware keyboard, iOS) leaves the box empty, so keyup re-reads it too. Never mid-composition:
  // rewriting the value would cancel the keyboard's word.
  let composing = false;
  function showTakenName() {
    if (composing) return;
    const taken = windowObject.__d2Shell?.charCreateEvidence?.name;
    if (typeof taken === 'string' && taken !== nameInput.value) nameInput.value = taken;
    prevValue = nameInput.value;
  }
  nameInput.addEventListener('compositionstart', () => { composing = true; });
  nameInput.addEventListener('compositionend', () => { composing = false; showTakenName(); });
  nameInput.addEventListener('input', () => { const value = nameInput.value; forwardNameDiff(prevValue, value); prevValue = value; showTakenName(); });
  nameInput.addEventListener('keyup', showTakenName);
  // Enter is forwarded once and goes no further: the forwarded one creates the hero and starts the
  // game, so the real one reaching the window a moment later would open the chat bar.
  nameInput.addEventListener('keydown', (e) => { if (e.key === 'Enter') { tapKey('Enter', 'Enter'); e.preventDefault(); e.stopPropagation(); } });

  // The shell already exposes exactly the signal retail's own name box uses to appear (a class
  // picked): polled, so this needs no new hook into the hot shared shell file.
  setInterval(() => {
    const shell = windowObject.__d2Shell;
    const show = revealed && shell?.screen === 'create' && Boolean(shell?.charCreateEvidence?.chosen);
    if (show) placeNameBox(shell);
    if (show && nameInput.style.display === 'none') { nameInput.style.display = 'block'; prevValue = nameInput.value = ''; showTakenName(); nameInput.focus(); }
    else if (!show && nameInput.style.display !== 'none') { nameInput.style.display = 'none'; nameInput.blur(); }
  }, 200);
  // Over the box the screen actually draws. On a phone the canvas takes the screen's shape, not 4:3,
  // and the 800x600 scene sits centred in it, so the stylesheet's 4:3 percentages would miss the box;
  // they stay only as the fallback when the screen reports no rect.
  function placeNameBox(shell) {
    const rect = shell.uiTargets?.()?.create?.name, w = canvas.width, h = canvas.height;
    if (!Array.isArray(rect) || !(w > 0) || !(h > 0)) return;
    const pct = (n, d) => `${(n / d * 100).toFixed(3)}%`;
    Object.assign(nameInput.style, { left: pct(rect[0], w), top: pct(rect[1], h), width: pct(rect[2], w), height: pct(rect[3], h) });
  }

  // A scripted player (wasm-build/scenarios/lib/player.mjs, S000_INPUT=touch) names the button it
  // wants directly -- unlike a real finger, it never has to put a second one down to say "right".
  // These call the exact same replay() a real gesture produces (touch-tagged pointer events through
  // input-common's click(), never a mouse event), so a row comparing the two input modes' resulting
  // engine commands is still comparing two runs of the SAME dispatch code. `longPress` keeps its
  // name only because lib/player.mjs (not this module) calls it; it is the RIGHT BUTTON.
  // `shiftKey` follows a held rest finger, as a real finger's events do, and an explicit opt still
  // wins -- lib/player.mjs's fillBelt passes `{shiftKey: true}` for the click it knows is a shift-click.
  return { canvas, container, nameInput, key: touchKey,
    flush,   // the shell's update() calls this first: a finger whose landing window has closed presses now
    isShiftArmed: restDown,   // "a rest finger is down": the shift every other finger's event carries
    tap: (x, y, opts = {}) => replay(x, y, { button: 0, pointerType: 'touch', pointerId: 9001, shiftKey: restDown(), ...opts }),
    longPress: (x, y, opts = {}) => replay(x, y, { button: 2, pointerType: 'touch', pointerId: 9001, ...opts }),
  };
}

if (typeof window !== 'undefined' && typeof document !== 'undefined' && document.addEventListener) window.__d2TouchInput = installTouchInput();
