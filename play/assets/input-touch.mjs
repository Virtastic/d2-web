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
//   * INSPECT BEFORE ACT: a phone has no hover, so an item's tooltip (drawn from the shell's hover
//     pointer) never showed. A FIRST tap on an item (shop stock, an inventory, stash or cube cell) sends
//     only a hover at it; the SECOND tap is the click. See inspectKeyAt below. Mouse input never reaches it.
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
import { isHudDeadZone, hudAimPoint, HUD_FINGER_REACH } from './control-panel.mjs';
import { hudOrigin } from './hud-origin.mjs';
import { orbAimPoint } from './orb-readout.mjs';
import { nearestPointAim, nearestControlAim, panelControlPoints } from './touch-panel-aim.mjs';
import { panelSpec, panelOrigin, onPanelControl, panelControlAim as aimPanelControls } from './touch-panel-aim.mjs';

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

  // ---- one finger's reach, as a physical size on glass ------------------------------------------
  // Every snap below (the belt, the grids, the "+" buttons, the HUD's small controls, the close buttons,
  // the dialogs, the world and the out-of-game screens) reaches a finger this far from its target. The
  // reach is FINGER_REACH_CSS CSS px -- a finger's own aim error (~10 CSS px), with margin -- converted
  // to canvas px through the canvas's drawn box, so the same finger reaches the same distance on a phone
  // and on a desktop. The target rectangles stay in game space; only the distance is converted.
  // `legacyCanvasPx` is the reach this aim used before (in canvas px): returned unchanged when the drawn
  // box is not measurable (a stub, a canvas outside the page), so nothing moves where there is no glass.
  const FINGER_REACH_CSS = 16;
  function fingerReach(legacyCanvasPx) {
    const rect = canvas.getBoundingClientRect?.();
    if (!rect || !(rect.width > 0) || !(canvas.width > 0)) return legacyCanvasPx;
    return FINGER_REACH_CSS * canvas.width / rect.width;
  }

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
  // How far out from the band the finger still snaps is the finger's own reach (fingerReach), not a
  // share of a box. The box chosen is the nearest centre, so no point inside the band has a box "in
  // front of" another whatever the reach.
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
    // The reach out from the band, in game px (the finger's physical reach, converted at this scale).
    const half = fingerReach((bottom - top) / 2 * o.scale) / o.scale;
    if (x < left - half || x > right + half || y < top - half || y > bottom + half) return null;
    let best = boxes[0], bestDistance = Infinity;
    for (const b of boxes) {
      const distance = Math.abs((b.left + b.right) / 2 - x);
      if (distance < bestDistance) { bestDistance = distance; best = b; }
    }
    return [o.x + (best.left + best.right) / 2 * o.scale, o.y + (top + bottom) / 2 * o.scale];
  }

  // A game-px rectangle the shell reports ({left, top, width|w, height|h}, the 800x600 frame) as the canvas-px rectangle
  // it is drawn at. Every panel rect the shell hands this module is game px; a finger is canvas px, so they meet here.
  function canvasRect(r) {
    const o = panelOrigin(canvas.width, canvas.height);
    return { left: o.x + r.left * o.scale, top: o.y + r.top * o.scale,
      width: (r.width ?? r.w) * o.scale, height: (r.height ?? r.h) * o.scale };
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
  // The grid and the panel arrive in GAME px (the shell's stashGeometry / inventoryLayout and its panel rects, the
  // 800x600 frame); the finger is canvas px. Both are carried to canvas px here, through the origin and scale the
  // renderers draw the frame with (panelOrigin), so the ring sits where the grid is drawn on any canvas, not at its
  // game-px numbers. At 800x600 that is the identity.
  function gridAim(x, y, gameGrid, gamePanel) {
    const o = panelOrigin(canvas.width, canvas.height);
    const grid = { left: o.x + gameGrid.left * o.scale, top: o.y + gameGrid.top * o.scale, columns: gameGrid.columns,
      rows: gameGrid.rows, cellWidth: gameGrid.cellWidth * o.scale, cellHeight: gameGrid.cellHeight * o.scale };
    const panel = canvasRect(gamePanel);
    // The ring is the finger's reach (canvas px, the grid's rectangles being canvas px now).
    const halfW = fingerReach(grid.cellWidth / 2), halfH = fingerReach(grid.cellHeight / 2);
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
      const reach = fingerReach(r.w / 2);
      if (x < r.x - reach || x > r.x + r.w + reach || y < r.y - reach || y > r.y + r.h + reach) continue;
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
    // hudAimPoint measures in game px at its own scale: the finger's reach converted to that scale.
    const scale = Math.min(1, canvas.width / 800);
    const reach = fingerReach(HUD_FINGER_REACH * scale) / scale;
    return hudAimPoint(x, y, { width: canvas.width, height: canvas.height, hud, belt: shell.beltHudBoxes || [], extra, reach });
  }
  // ---- M18: the life and mana orbs -----------------------------------------------------------------
  // The orbs are the bottom band's two readouts, and their rings are 80 px wide at scale 1 (orbAt in
  // orb-readout.mjs). A finger a few px off a ring is on nothing the shell hit-tests, and the orb's
  // click (the pin) never sees it. So a finger within HUD_FINGER_REACH of a ring presses its centre,
  // like the HUD's small controls (M17). Off while a panel covers the band (the character screen, the
  // skill tree, the skill popup); an open inventory or stash keeps its own rectangle (orbAimPoint).
  function orbAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen || shell.skillSelectOpen || shell.characterVisible || shell.skillsVisible) return null;
    const panels = [];
    if (shell.inventoryVisible && shell.inventoryPanelRect) panels.push(canvasRect(shell.inventoryPanelRect));
    if (shell.stashPanelRect) panels.push(canvasRect(shell.stashPanelRect));
    const scale = Math.min(1, canvas.width / 800);
    return orbAimPoint(canvas.width, canvas.height, x, y, fingerReach(HUD_FINGER_REACH * scale), panels);
  }
  // ---- the world: a finger near a monster, NPC, ground item, object or exit picks it ------------------
  // A finger lands ~10 CSS px (~15 game px) off what the player meant. When the exact point resolves to
  // nothing a click would take (it would walk), the shell's own click resolver is asked at a ring of
  // points around it, nearest ring first; the first hit is where the press goes. A point that already
  // hits something is delivered raw, and ground that has nothing near it still walks exactly there.
  // Panels, the HUD band and the belt are excluded: their own aim rules (above) decide there.
  const WORLD_SPOKES = 8;
  // The right click (the second finger) is how a player attacks, and it is the hardest to land on a moving
  // monster: it searches wider (RIGHT_REACH_CSS), on more rings and spokes, nearest ring first.
  const RIGHT_REACH_CSS = 30, RIGHT_RINGS = 5, RIGHT_SPOKES = 16;
  function worldAim(x, y, wide) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen || typeof shell.touchPickAt !== 'function') return null;
    if (shell.skillSelectOpen || shell.characterVisible || shell.skillsVisible) return null;
    const panels = [shell.inventoryVisible && shell.inventoryPanelRect, shell.stashPanelRect].filter(Boolean).map(canvasRect);
    const inPanel = (px, py) => panels.some((r) => px >= r.left && px < r.left + (r.width ?? r.w) && py >= r.top && py < r.top + (r.height ?? r.h));
    const bandTop = canvas.height - 104 * Math.min(1, canvas.width / 800);
    if (y >= bandTop || inPanel(x, y)) return null;
    let hit;
    try { if (shell.touchPickAt(x, y)) return null; } catch { return null; }
    const reach = wide ? fingerReach(16) * RIGHT_REACH_CSS / FINGER_REACH_CSS : fingerReach(16);
    const rings = wide ? Array.from({ length: RIGHT_RINGS }, (_, k) => reach * (k + 1) / RIGHT_RINGS) : [reach / 2, reach];
    const spokes = wide ? RIGHT_SPOKES : WORLD_SPOKES;
    for (const r of rings) {
      for (let i = 0; i < spokes; i += 1) {
        const a = (i / spokes) * 2 * Math.PI, px = x + r * Math.cos(a), py = y + r * Math.sin(a);
        if (py >= bandTop || inPanel(px, py)) continue;
        try { hit = shell.touchPickAt(px, py); } catch { hit = false; }
        if (hit) return [px, py];
      }
    }
    return null;
  }
  // ---- the panels' close buttons ------------------------------------------------------------------
  // The inventory, character and stash close buttons are 32x32 game px. A finger that lands within half
  // a button of an open panel's close presses its centre; a point on it is delivered raw. Rects are the
  // ones the close modules hit-test, in the centred 800x600 space.
  const CLOSES = [['inventoryVisible', 418, 444, 33, 33], ['characterVisible', 208, 448, 33, 33], ['stashPanelRect', 352, 445, 32, 32]];
  function closeAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen) return null;
    const w = canvas.width, h = canvas.height, s = Math.min(1, w / 800, h / 600);
    const ox = (w - 800 * s) / 2, oy = (h - 600 * s) / 2, gx = (x - ox) / s, gy = (y - oy) / s;
    for (const [flag, l, t, bw, bh] of CLOSES) {
      if (!shell[flag]) continue;
      if (gx >= l && gx < l + bw && gy >= t && gy < t + bh) return null;
      const reach = fingerReach(bw * s / 2) / s;   // the finger's reach, in game px at this scale
      if (gx < l - reach || gx > l + bw + reach || gy < t - reach || gy > t + bh + reach) continue;
      return [ox + (l + bw / 2) * s, oy + (t + bh / 2) * s];
    }
    return null;
  }
  // ---- the inventory's weapon-set tabs ------------------------------------------------------------
  // The two tabs sit 24 game px above the arm slots, so a tab's near miss is close to a slot too. A
  // finger on a tab or an equipped slot is delivered raw; so is a finger within TAB_RING of any slot
  // (that finger is the slot's). Only a finger clear of every slot, with a tab inside the ring, is
  // pulled to the tab's first ring point. A dialog up over the inventory owns the touch: raw.
  const TAB_RING = 6, TAB_SPOKES = 8;
  function tabAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || typeof shell.touchWeaponTabAt !== 'function') return null;
    const probe = (px, py) => { try { return shell.touchWeaponTabAt(px, py); } catch { return null; } };
    const here = probe(x, y);
    if (!here || here.tab || here.slot || here.gap <= TAB_RING) return null;
    if (typeof shell.touchControlAt === 'function') {
      let on; try { on = shell.touchControlAt(x, y); } catch { return null; }
      if (on != null) return null;
    }
    for (let i = 0; i < TAB_SPOKES; i += 1) {
      const a = (i / TAB_SPOKES) * 2 * Math.PI, px = x + TAB_RING * Math.cos(a), py = y + TAB_RING * Math.sin(a);
      const c = probe(px, py);
      if (c && c.tab && !c.slot) return [px, py];
    }
    return null;
  }
  // ---- the two gold coins (the stash's withdraw, the inventory's deposit): small cels a finger misses ----
  // The coins' own hitAt (touchGoldAt) is the probe. A point on a coin is delivered raw, and so is one on a panel
  // control (that control's own hit test decides it). A finger beside a coin presses the first ring point, at
  // reach/2 then reach, that the probe answers for, as modalAim reads its dialog.
  const COIN_SPOKES = 8;
  function goldAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || typeof shell.touchGoldAt !== 'function') return null;
    const probe = (px, py) => { try { return shell.touchGoldAt(px, py); } catch { return null; } };
    if (probe(x, y)) return [x, y];
    if (onPanelControl(x, y, panelSpecNow())) return null;
    const reach = fingerReach(10);
    for (const r of [reach / 2, reach]) for (let i = 0; i < COIN_SPOKES; i += 1) {
      const a = (i / COIN_SPOKES) * 2 * Math.PI, px = x + r * Math.cos(a), py = y + r * Math.sin(a);
      if (probe(px, py)) return [px, py];
    }
    return null;
  }
  // ---- a dialog's small buttons: the gold dialog's arrows/ok/close, repair yes/no -------------------
  // While one is up, a finger that hits none of its buttons is probed at rings around it with the
  // dialog's own hit test; the nearest ring's first point on a button is where the press goes.
  function modalAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || typeof shell.touchControlAt !== 'function') return null;
    let on;
    try { on = shell.touchControlAt(x, y); } catch { return null; }
    if (on !== false) return null;
    try { return nearestControlAim(x, y, (px, py) => Boolean(shell.touchControlAt(px, py)), fingerReach(12)); } catch { return null; }
  }
  // ---- the panels' small targets: vendor tabs and buttons, waypoint tabs/close/rows, NPC menu rows, the
  // skill popup's icons ------------------------------------------------------------------------------
  // The shell reports each one's canvas centre (touchPanelTargets); a finger within reach of one presses
  // the nearest, so of two close targets the one the finger is nearer to wins.
  function panelTargetAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || typeof shell.touchPanelTargets !== 'function') return null;
    let t; try { t = shell.touchPanelTargets(); } catch { return null; }
    if (!t) return null;
    return nearestPointAim(x, y, panelControlPoints(t), fingerReach(12));
  }
  // ---- the out-of-game screens: title, gateways, character select/create, options ---------------------
  // A finger within the finger's reach of a control's click point presses that point; the nearest wins.
  function frontAim(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen === 'game' || typeof shell.uiTargets !== 'function') return null;
    let t; try { t = shell.uiTargets(); } catch { return null; }
    const pts = [], walk = (v) => {
      if (Array.isArray(v) && v.length === 2 && typeof v[0] === 'number' && typeof v[1] === 'number') pts.push(v);
      else if (v && typeof v === 'object') for (const k in v) walk(v[k]);
    };
    walk(t);
    const reach = fingerReach(20 * Math.min(1, canvas.width / 800, canvas.height / 600));
    let best = null, bd = reach * reach;
    for (const [px, py] of pts) { const d = (px - x) ** 2 + (py - y) ** 2; if (d <= bd) { bd = d; best = [px, py]; } }
    return best;
  }
  // The point a finger's aim snaps to, or null when the finger is on nothing the aim rules cover.
  // The shell's panel layout as a canvas-px spec (touch-panel-aim.mjs), or null when no panel control is up.
  function panelSpecNow() {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen || typeof shell.touchPanelLayout !== 'function') return null;
    let layout; try { layout = shell.touchPanelLayout(); } catch { return null; }
    return panelSpec(layout, canvas.width, canvas.height);
  }
  // ---- the stash's page buttons and close, the cube's Transmute button and its grid (touch-panel-aim.mjs) ----
  // The shell reports the open panels in game px; the same nearest-control and grid-ring rules as the rest apply.
  function panelControlAim(x, y) { return aimPanelControls(x, y, panelSpecNow(), fingerReach); }
  // The chain's order: a point on a target wins over a ring, and the gold coins' ring (goldAim) comes before the
  // panel-control ring, so a coin a finger is beside is pressed rather than a panel button past it.
  function snapAim(x, y) { return frontAim(x, y) || panelTargetAim(x, y) || modalAim(x, y) || closeAim(x, y) || goldAim(x, y) || tabAim(x, y) || panelControlAim(x, y) || beltAim(x, y) || panelAim(x, y) || statAim(x, y) || hudAim(x, y) || orbAim(x, y); }
  function fingerAim(x, y) { return snapAim(x, y) || worldAim(x, y); }
  // A finger's aim: the belt's boxes, an open panel's grid cells, the character screen's "+", else
  // exactly where it landed.
  function aim(x, y) { return fingerAim(x, y) || [x, y]; }
  // A dragging finger's aim: the world probe is for where a finger LANDS, never for a held walk's path.
  function moveAim(x, y) { return snapAim(x, y) || [x, y]; }

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
  // ---- inspect before act: a phone's first tap on an item shows its tooltip -----------------------
  // A tooltip is drawn from the shell's hover pointer, and only a pointermove sets it. A phone has no
  // hover, so a tap on an item never showed one, and a tap on shop stock bought it at once. So a finger
  // that lands on an item (shop stock, an inventory, stash or cube cell, an equipped slot) that is NOT
  // the item inspected last sends no press at all: it sends a hover (a pointermove with no button held)
  // at the item, which the shell's tooltip reads, and records the item as inspected. The SECOND tap on
  // the same item is the click it always was (buy, lift, sell, drop). Any other tap clears the
  // inspected item. A held finger on a grid cell is the right click (HOLD_MS, above) and counts as
  // inspected too. Taps with an item on the cursor act at once: placing it must not take two taps.
  // A BELT potion follows the same two taps, the second being a USE (a right click, the belt's own
  // right button) rather than a lift: see lift(). Its tooltip is the shell's to draw (see the handback).
  let inspected = null;
  function inspectKeyAt(x, y) {
    const shell = windowObject.__d2Shell;
    if (!shell || shell.screen !== 'game' || shell.clearScreen) return null;
    const [ix, iy] = moveAim(x, y);
    const shop = shell.touchShopItemAt?.(ix, iy) || 0;
    if (shop) return { key: `shop:${shop}`, shop: true };
    const skill = shell.touchSkillAt?.(ix, iy) || 0;
    if (skill) return { key: `skill:${skill}`, shop: false };
    const item = shell.touchItemAt?.(ix, iy) || 0;
    if (item) return { key: `item:${item}`, shop: false };
    const belt = shell.touchBeltItemAt?.(ix, iy) || 0;
    return belt ? { key: `belt:${belt}`, shop: false, belt: true } : null;
  }
  function heldItem() { return windowObject.__d2Shell?.cursorItem || 0; }
  // A dead player in the game (PLRMODE_DEAD = 17, the mode death-screen.mjs's respawnAllowed reads; the
  // shell's own Escape handler is what checks it). The death animation (0) is not yet a respawn.
  function deadAwaitingRespawn() {
    const shell = windowObject.__d2Shell;
    return Boolean(shell && shell.screen === 'game' && !shell.clearScreen && shell.snapshot?.mode === 17);
  }
  function hover(x, y, pointerId) {
    send('pointermove', x, y, { button: 0, buttons: 0, pointerType: 'touch', pointerId, shiftKey: restDown() });
  }
  // The second tap on a belt potion: a quick right click at the potion's box, the press and the menu a
  // real right click makes. The belt's right button uses the potion (belt-popup.mjs's beltClickAction).
  function useBelt(pointerId, x, y) {
    const shiftKey = restDown();
    send('pointermove', x, y, { button: 0, pointerType: 'touch', pointerId, shiftKey });
    send('pointerdown', x, y, { button: 2, pointerType: 'touch', pointerId, shiftKey });
    sendContextMenu(x, y, shiftKey);
    send('pointerup', x, y, { button: 2, pointerType: 'touch', pointerId, shiftKey });
  }
  function holdRight(pointerId, h) {
    h.pending = false;
    h.released = true;
    h.right = true;
    inspected = h.itemKey ?? null;
    const [ax, ay] = aim(h.x, h.y);
    const shiftKey = restDown();
    send('pointermove', ax, ay, { button: 0, pointerType: 'touch', pointerId, shiftKey });
    send('pointerdown', ax, ay, { button: 2, pointerType: 'touch', pointerId, shiftKey });
    sendContextMenu(ax, ay, shiftKey);
  }
  function land(pointerId, h) {
    if (!h.pending) return;
    h.pending = false;
    inspected = null;   // a press is a click: the item it lands on is no longer only inspected
    const [ax, ay] = aim(h.x, h.y);
    const opts = { button: 0, pointerType: 'touch', pointerId, shiftKey: restDown() };
    send('pointermove', ax, ay, opts);
    send('pointerdown', ax, ay, opts);
  }
  function flush() {
    for (const [id, h] of holds) {
      if (h.role !== 'aim' || h.released || inWindow(h)) continue;
      if (h.item && h.pending && h.t !== undefined) holdRight(id, h);
      else if (!h.hoverFirst) land(id, h);   // an inspect tap waits for its lift: a tap, a hold or a drag decides it
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
    if (h.role === 'aim' && !h.released && h.hoverFirst && !h.moved && !cancelled) {
      // An inspect tap on an item: a hover at the item, and no press (see inspectKeyAt above).
      h.released = true;
      inspected = h.itemKey;
      const [hx, hy] = moveAim(h.x, h.y);
      hover(hx, hy, pointerId);
      return;
    }
    if (h.role === 'aim' && !h.released && h.belt && !h.moved && !cancelled) {
      // The second tap on an inspected belt potion: a use, not a lift. The inspection is spent.
      h.released = true;
      inspected = null;
      const [ux, uy] = aim(h.x, h.y);
      useBelt(pointerId, ux, uy);
      return;
    }
    if (h.role === 'aim' && h.right) {
      const [ax, ay] = aim(h.x, h.y);
      send('pointerup', ax, ay, { button: 2, pointerType: 'touch', pointerId, shiftKey: restDown() });
      return;
    }
    if (h.role === 'aim' && !h.released) land(pointerId, h);   // a tap inside the window: move, down, up
    const shiftKey = restDown();
    if (h.role === 'aim' && !h.released) {
      const [ax, ay] = h.moved ? moveAim(h.x, h.y) : aim(h.x, h.y);
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
    // The death prompt reads "PRESS ESC TO CONTINUE" (retail's youdiedinst). A phone has no Escape, so a
    // touch on the dead player's screen is that Escape: the same keydown the shell answers with a respawn.
    // It is sent once, at the press; the finger is held as a dead hold, which sends nothing when it lifts.
    if (deadAwaitingRespawn()) { holds.set(e.pointerId, { role: 'dead' }); tapKey('Escape', 'Escape'); return; }
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
      const [ax, ay] = snapAim(aiming[1].x, aiming[1].y) || worldAim(aiming[1].x, aiming[1].y, true) || [aiming[1].x, aiming[1].y];
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
    // An item it lands on that is not the inspected one is inspected first (see inspectKeyAt): the
    // press waits for the second tap. With an item in hand, nothing waits.
    const found = heldItem() ? null : inspectKeyAt(x, y);
    const hoverFirst = Boolean(found && found.key !== inspected);
    if (!hoverFirst) inspected = null;
    if (hoverFirst && found.shop) {
      // Stock is bought on the press, so this tap is decided at the landing: a hover, no press.
      inspected = found.key;
      const [hx, hy] = moveAim(x, y);
      holds.set(e.pointerId, { role: 'aim', x, y, released: true });
      hover(hx, hy, e.pointerId);
      return;
    }
    holds.set(e.pointerId, { role: 'aim', x, y, pending: true, t: clock(), item: itemGround(x, y),
      hoverFirst, itemKey: found?.key ?? null, belt: Boolean(found?.belt) });
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
      const [ax, ay] = snapAim(x, y) || worldAim(x, y, true) || [x, y];
      held[1].ax = ax; held[1].ay = ay;
      send('pointermove', ax, ay, { button: 2, pointerType: 'touch', pointerId: held[0], shiftKey: restDown() });
      return;
    }
    // A jitter inside the landing window keeps the press waiting, at the point the finger landed.
    if (h.pending && inWindow(h) && Math.hypot(x - h.x, y - h.y) <= LAND_SLOP) return;
    land(e.pointerId, h);
    h.moved = true;   // the press goes where the finger landed, before the drag
    h.x = x; h.y = y;
    const [ax, ay] = moveAim(x, y);
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
  nameInput.addEventListener('keydown', (e) => { if (e.key === 'Enter') { tapKey('Enter', 'Enter'); e.preventDefault(); e.stopPropagation(); nameInput.blur(); } });
  // The phone keyboard opens on a tap on the box, never on its own when the box appears.
  nameInput.addEventListener('pointerdown', () => nameInput.focus());

  // The shell already exposes exactly the signal retail's own name box uses to appear (a class
  // picked): polled, so this needs no new hook into the hot shared shell file.
  setInterval(() => {
    const shell = windowObject.__d2Shell;
    const show = revealed && shell?.screen === 'create' && Boolean(shell?.charCreateEvidence?.chosen);
    if (show) placeNameBox(shell);
    if (show && nameInput.style.display === 'none') { nameInput.style.display = 'block'; prevValue = nameInput.value = ''; showTakenName(); }
    else if (!show && nameInput.style.display !== 'none') { nameInput.style.display = 'none'; nameInput.blur(); }
    // The name can change with no event on the box (a hardware key, the screen clearing it): the box follows it.
    if (show) showTakenName();
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
