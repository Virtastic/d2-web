// Touch is an input layer: every gesture here ends as one of
// the SAME DOM pointer or key events a mouse and a keyboard produce, dispatched through
// input-common.mjs's proven helpers, so play/gameplay-shell.mjs's own handlers decide what happens
// -- nothing about the game is re-implemented here.
//
// TOUCH IS TWO GESTURES, AND THEY ARE THE MOUSE'S TWO BUTTONS (
// "Control scheme"). The whole game is already playable with a mouse alone -- left click and right
// click carry every action retail needs -- so touch invents no vocabulary of its own on top:
//
//   * ONE FINGER is the LEFT button. A raw PointerEvent already reaches the shell's canvas listener
//     unchanged (Pointer Events carry pointerType through and that listener never reads it), so a
//     tap is already a click once touch-action:none keeps the browser from turning the gesture into
//     a scroll or a zoom first (M3). Nothing here translates it.
//   * TWO FINGERS are the RIGHT button. A second finger landing on the canvas while the first is
//     still down REPLAYS exactly the right click input-common's click() gives a mouse (button 2,
//     plus the contextmenu retail's own inventory right-click reads), at the point the AIMING
//     finger -- the one that was already down, the one the player is pointing with -- is on. No
//     hold timer, no long press: a right click that waits 500 ms is a right click a player cannot
//     use in a fight, and "press the right button" has no timing in it.
//
// The canvas's touch events are still held back from the shell and replayed (a capture-phase
// listener above the canvas, stopping the real event before the shell's own bubble-phase listener
// sees it) for the three things a raw event cannot carry: the right button above; M9's shift
// latch, which has to ride the replay the way a held physical shift rides a mouse click; and the
// finger's AIM (M14, M15), because the game's own 29x29 controls are 19.9 CSS px of glass on a phone
// and a finger's own error is wider than they are -- the belt's boxes with their 2-px gutters, and
// the open panels' grid cells (see beltAim and gridAim below). Everything
// this module dispatches goes through input-common's own click()/tapKey() -- the exact calls a mouse
// and a keyboard already produce -- so a row comparing a touch path against a mouse or keyboard path
// is comparing two runs of the SAME code, not two implementations of the same idea.
//
// The virtual stick and the button bar (M1/M2/M4) are new DOM elements this module creates and owns
// in #touch-overlay; they are controls retail has no on-screen equivalent of, and each dispatches
// the same primitives.

import { click, tapKey, toCanvas, playerOnCanvas, stickTarget } from './input-common.mjs';

export const STICK_TICK_MS = 150;      // how often a held stick re-aims the walk, as a mouse re-clicked
export const STICK_RADIUS = 160;       // input-common.stickTarget's own default, named for the CSS geometry

// M13 (text legibility at phone sizes): not pursued -- touch is one finger = left
// click, two fingers = right click, nothing else. A press-and-hold loupe was built and rejected as an
// unrequested gesture; M13 stays a GAP in .

const BAR_KEYS = [
  { code: 'KeyI', label: 'I' },
  { code: 'KeyC', label: 'C' },
  { code: 'KeyT', label: 'T' },
  { code: 'Escape', label: 'Esc' },
];

function injectStyle(documentObject) {
  if (documentObject.getElementById('d2-touch-style')) return;
  const style = documentObject.createElement('style');
  style.id = 'd2-touch-style';
  // The overlay is a transparent pass-through layer (pointer-events:none) so a mouse player's clicks
  // reach the canvas beneath it untouched; only .d2-touch-hit children opt in, and only once revealed
  // (coarse pointer, or the first real touch -- "The overlay appears only on touch
  // devices... never over a mouse player's screen").
  style.textContent =
    '#touch-overlay{position:absolute;inset:0;opacity:0;pointer-events:none;transition:opacity .15s;font:14px monospace}' +
    '#touch-overlay.d2-touch-on{opacity:1}' +
    '#touch-overlay .d2-touch-hit{pointer-events:none}' +
    '#touch-overlay.d2-touch-on .d2-touch-hit{pointer-events:auto}' +
    '.d2-touch-stick{position:absolute;left:5%;bottom:7%;width:26%;max-width:150px;aspect-ratio:1;' +
      'border-radius:50%;background:rgba(201,173,104,.12);border:2px solid rgba(201,173,104,.45);touch-action:none}' +
    '.d2-touch-knob{position:absolute;left:50%;top:50%;width:42%;height:42%;border-radius:50%;' +
      'background:rgba(201,173,104,.55);transform:translate(-50%,-50%);pointer-events:none}' +
    '.d2-touch-bar{position:absolute;right:4%;bottom:7%;display:flex;gap:10px}' +
    '.d2-touch-btn{width:44px;height:44px;border-radius:8px;background:rgba(20,18,12,.65);' +
      'border:2px solid rgba(201,173,104,.45);color:#c9ad68;font:15px monospace;touch-action:none;' +
      'display:flex;align-items:center;justify-content:center;user-select:none}' +
    // The shift latch, armed: it must LOOK armed from across the screen -- a modifier the player
    // cannot see is one they leave on.
    '#touch-overlay .d2-touch-armed{background:rgba(201,173,104,.8);color:#1a170f;border-color:#e8d9ab}' +
    '#d2-touch-name{position:absolute;left:39.75%;top:82.2%;width:27.5%;height:5.2%;box-sizing:border-box;' +
      'font:16px monospace;padding:2px 6px;background:#1a170f;color:#e8d9ab;border:1px solid #493d27;touch-action:none}' +
    // A wide phone held sideways letterboxes the 4:3 game with black bars at both sides: the stick and
    // the key bar go there, off the orbs, belt and skill buttons they covered when drawn over the game.
    // (A 4:3 tablet has no bars; they stay over the game there.) Last, so it overrides the rules above.
    '@media (pointer: coarse) and (min-aspect-ratio: 16/9){' +
      '.d2-touch-stick{position:fixed;left:calc((100vw - 100dvh * 4 / 3) / 4);bottom:12%;transform:translateX(-50%);' +
        'width:min(150px, calc((100vw - 100dvh * 4 / 3) / 2 - 16px))}' +
      '.d2-touch-bar{position:fixed;right:calc((100vw - 100dvh * 4 / 3) / 4);bottom:12%;transform:translateX(50%);flex-direction:column}' +
    '}';
  documentObject.head.appendChild(style);
}

/**
 * Mount the touch input layer on the page: the canvas long-press/tap handling, the virtual stick,
 * the button bar, and the name-entry bridge. Safe to call once per page; a page with none of
 * the expected markup (#game, #touch-overlay) is a no-op, so this can be the page's whole script.
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

  // Events THIS module redispatches at the canvas (a confirmed tap, a matured long press, a stick
  // tick) must not be read back as a new gesture. element.dispatchEvent() is synchronous -- every
  // listener runs before it returns -- so reserving the pointerId only around that call is enough.
  const replaying = new Set();
  function replay(x, y, opts) {
    const id = opts.pointerId ?? 1;
    replaying.add(id);
    try { click(canvas, x, y, opts); } finally { replaying.delete(id); }
  }

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
  //     through, still delivers the raw point -- an instrument that knows where it is pointing),
  //   * the STICK is untouched: its tick aims at `stickTarget`, a point in the world the hero walks
  //     to, and a walk that happens to aim into the belt band must stay a walk.
  // Half a cell is the width of the rule, not a taste: it is the distance at which the neighbouring
  // box is as close as this one, so no point inside the band has a box "in front of" another.
  function beltAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen) return null;
    const boxes = shell.beltHudBoxes;
    if (!boxes || !boxes.length) return null;
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
    return [(best.left + best.right) / 2, (top + bottom) / 2];
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
  //   * and the MOUSE, the SCRIPTED seam and the stick are untouched, exactly as for the belt.
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
  //   * The MOUSE, the SCRIPTED seam and the stick are untouched, exactly as for the belt and the
  //     grids: the miss that spends the world is retail's own hit test, and the row measures it.
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
  // A finger's aim: the belt's boxes, an open panel's grid cells, the character screen's "+", else
  // exactly where it landed.
  function aim(x, y) { return beltAim(x, y) || panelAim(x, y) || statAim(x, y) || [x, y]; }

  // ---- M9: the keyboard's shift, which a finger has no way to hold ---------------------------------
  // Retail's shift-click (0x63) is the only thing that puts an item into the belt, and the shell reads
  // it from `e.shiftKey` alone (play/gameplay-shell.mjs's inventory branch). A phone has no shift key,
  // so this is a LATCH: one tap arms it, every tap while it is armed carries the same `shiftKey` a held
  // physical shift gives, and a second tap disarms it -- the desktop's "hold shift and click each
  // potion", spelled as a toggle because a thumb cannot hold one key and tap with the other hand's
  // finger reliably at the same time. It is sticky rather than one-shot because retail's shift is held
  // across many clicks, and it is drawn armed so a player never shifts a click they did not mean to.
  const shift = { armed: false };

  // ---- M3 / M5: one finger is a click, two fingers are a right click, per pointerId (M4) --------
  // pointerId -> { x, y, fired, consumed }. `fired` is a finger whose release must make no
  // left click (its gesture was spent as the aiming finger of a right click); `consumed` is a finger
  // that never was a click at all (the second of the pair, or a stray third).
  const holds = new Map();

  documentObject.addEventListener('pointerdown', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    reveal();   // "the first touch", wherever it lands
    if (e.target !== canvas) return;
    e.stopPropagation();
    if (e.cancelable) e.preventDefault();
    const [x, y] = toCanvas(canvas, e.clientX, e.clientY);
    // Already a pair down: a third finger is not a third button. It is swallowed, so it can neither
    // fire a left click of its own nor turn the right click into anything else.
    if ([...holds.values()].some((h) => h.consumed)) { holds.set(e.pointerId, { consumed: true }); return; }
    const live = [...holds.entries()].filter(([, h]) => !h.fired);
    if (live.length === 1) {
      // M5: the second finger. The right click happens AT THE AIMING FINGER'S POINT -- the finger
      // already down, the one the player is pointing with -- which is where a mouse user would have
      // had the pointer when they pressed the right button.
      const [aimId, aiming] = live[0];   // named so it cannot shadow the aim() step below
      aiming.fired = true;
      holds.set(e.pointerId, { consumed: true });
      const [ax, ay] = aim(aiming.x, aiming.y);
      replay(ax, ay, { button: 2, pointerType: 'touch', pointerId: aimId });
      return;
    }
    holds.set(e.pointerId, { x, y, fired: false, consumed: false });
  }, { capture: true });

  documentObject.addEventListener('pointermove', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    const h = holds.get(e.pointerId);
    if (!h) return;
    e.stopPropagation();   // the pair's second finger moves nothing, but it does not reach the shell either
    if (h.consumed) return;
    const [x, y] = toCanvas(canvas, e.clientX, e.clientY);
    h.x = x; h.y = y;   // the left click lands where the finger is RELEASED, as the shell reads a mouse
  }, { capture: true });

  documentObject.addEventListener('pointerup', (e) => {
    if (e.pointerType !== 'touch' || replaying.has(e.pointerId)) return;
    const h = holds.get(e.pointerId);
    if (!h) return;
    e.stopPropagation();
    if (!h.consumed && !h.fired) {   // M14/M15: a finger's own aim, at the belt's boxes or a panel grid
      const [ax, ay] = aim(h.x, h.y);
      replay(ax, ay, { button: 0, pointerType: 'touch', pointerId: e.pointerId, shiftKey: shift.armed });
    }
    holds.delete(e.pointerId);
  }, { capture: true });

  documentObject.addEventListener('pointercancel', (e) => {
    if (e.pointerType !== 'touch') return;
    holds.delete(e.pointerId);
  }, { capture: true });

  // ---------------------------- M1 / M4: the virtual stick -----------------------------------
  const stickBase = documentObject.createElement('div');
  stickBase.id = 'd2-touch-stick'; stickBase.className = 'd2-touch-stick d2-touch-hit';
  const knob = documentObject.createElement('div'); knob.className = 'd2-touch-knob';
  stickBase.appendChild(knob); container.appendChild(stickBase);

  let stick = null;   // { pointerId, ax, ay, timer }
  function stickAt(clientX, clientY) {
    const r = stickBase.getBoundingClientRect();
    const cx = r.left + r.width / 2, cy = r.top + r.height / 2, radius = Math.max(1, r.width / 2);
    const dx = clientX - cx, dy = clientY - cy, mag = Math.hypot(dx, dy) || 1;
    const clamp = Math.min(1, mag / radius);
    knob.style.transform = `translate(${-50 + (dx / mag) * clamp * 50}%, ${-50 + (dy / mag) * clamp * 50}%)`;
    return { ax: dx / radius, ay: dy / radius };
  }
  function stickTick() {
    if (!stick) return;
    const target = stickTarget(playerOnCanvas(), stick.ax, stick.ay, { radius: STICK_RADIUS });
    if (target) replay(target[0], target[1], { button: 0, pointerType: 'touch', pointerId: stick.pointerId });
  }
  stickBase.addEventListener('pointerdown', (e) => {
    if (stick) return;   // one thumb on the stick at a time
    // Pointer capture keeps the drag reporting to this element even if the finger slides off it; a
    // synthetic PointerEvent (a scenario's dispatch, not a real OS-tracked pointer) has no "active
    // pointer" to capture and throws NotFoundError here, which must not stop the gesture itself.
    try { stickBase.setPointerCapture?.(e.pointerId); } catch {}
    const aim = stickAt(e.clientX, e.clientY);
    stick = { pointerId: e.pointerId, ax: aim.ax, ay: aim.ay, timer: setInterval(stickTick, STICK_TICK_MS) };
    stickTick();
    if (e.cancelable) e.preventDefault();
  });
  stickBase.addEventListener('pointermove', (e) => {
    if (!stick || e.pointerId !== stick.pointerId) return;
    const aim = stickAt(e.clientX, e.clientY);
    stick.ax = aim.ax; stick.ay = aim.ay;
  });
  const releaseStick = (e) => {
    if (!stick || e.pointerId !== stick.pointerId) return;
    clearInterval(stick.timer); stick = null;
    knob.style.transform = 'translate(-50%,-50%)';
  };
  stickBase.addEventListener('pointerup', releaseStick);
  stickBase.addEventListener('pointercancel', releaseStick);

  // -------------------------------- M2: the button bar ---------------------------------------
  const bar = documentObject.createElement('div'); bar.id = 'd2-touch-bar'; bar.className = 'd2-touch-bar';
  for (const { code, label } of BAR_KEYS) {
    const btn = documentObject.createElement('div');
    btn.className = 'd2-touch-btn d2-touch-hit'; btn.dataset.touchKey = code; btn.textContent = label;
    // The same primitive the keyboard's own keydown/keyup produce (input-common.tapKey): retail's
    // handler cannot tell this apart from a physical key, because it is not.
    btn.addEventListener('pointerdown', (e) => { tapKey(code, code === 'Escape' ? 'Escape' : ''); if (e.cancelable) e.preventDefault(); });
    bar.appendChild(btn);
  }
  // M9: the shift latch, at the end of the bar (the four keys keep their measured order and size).
  // `data-touch-shift` is the handle a scenario reaches it by; the class is what a player sees.
  const shiftButton = documentObject.createElement('div');
  shiftButton.className = 'd2-touch-btn d2-touch-hit'; shiftButton.dataset.touchShift = '';
  shiftButton.textContent = '⇧'; shiftButton.setAttribute('aria-label', 'Shift');
  shiftButton.addEventListener('pointerdown', (e) => {
    setShift(!shift.armed);
    if (e.cancelable) e.preventDefault();
  });
  bar.appendChild(shiftButton);
  container.appendChild(bar);

  function setShift(armed) {
    shift.armed = Boolean(armed);
    shiftButton.classList.toggle('d2-touch-armed', shift.armed);
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
  nameInput.addEventListener('input', () => { const value = nameInput.value; forwardNameDiff(prevValue, value); prevValue = value; });
  nameInput.addEventListener('keydown', (e) => { if (e.key === 'Enter') { tapKey('Enter', 'Enter'); e.preventDefault(); } });

  // The shell already exposes exactly the signal retail's own name box uses to appear (a class
  // picked): polled, so this needs no new hook into the hot shared shell file.
  setInterval(() => {
    const shell = windowObject.__d2Shell;
    const show = revealed && shell?.screen === 'create' && Boolean(shell?.charCreateEvidence?.chosen);
    // The stick and the key bar are for play: on the title, select and create screens they sat over
    // retail's own buttons (CREDITS, CINEMATICS, EXIT on a landscape phone) and took their taps.
    const inGame = shell?.screen === 'game' ? '' : 'none';
    if (stickBase.style.display !== inGame) { stickBase.style.display = inGame; bar.style.display = inGame; }
    if (show && nameInput.style.display === 'none') { nameInput.style.display = 'block'; prevValue = nameInput.value = ''; nameInput.focus(); }
    else if (!show && nameInput.style.display !== 'none') { nameInput.style.display = 'none'; nameInput.blur(); }
  }, 200);

  // A scripted player (wasm-build/scenarios/lib/player.mjs, S000_INPUT=touch) names the button it
  // wants directly -- unlike a real finger, it never has to put a second one down to say "right".
  // These call the exact same replay() a real gesture produces (touch-tagged pointer events through
  // input-common's click(), never a mouse event), so a row comparing the two input modes' resulting
  // engine commands is still comparing two runs of the SAME dispatch code. `longPress` keeps its
  // name only because lib/player.mjs (not this module) calls it; it is the RIGHT BUTTON, and the
  // finger gesture for it is a second finger, not a hold.
  // `shiftKey` is the latch's own state for a scripted tap (the real finger's replay reads it in the
  // pointerup handler above), and an explicit opt still wins -- lib/player.mjs's fillBelt passes
  // `{shiftKey: true}` for the click it knows is a shift-click.
  return { canvas, container, stickBase, bar, nameInput, shiftButton,
    isShiftArmed: () => shift.armed,
    setShift,
    tap: (x, y, opts = {}) => replay(x, y, { button: 0, pointerType: 'touch', pointerId: 9001, shiftKey: shift.armed, ...opts }),
    longPress: (x, y, opts = {}) => replay(x, y, { button: 2, pointerType: 'touch', pointerId: 9001, ...opts }),
  };
}

if (typeof window !== 'undefined') window.__d2TouchInput = installTouchInput();
