import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { recordDc6, recordPrim } from './drawlist.mjs';
import { rulesBound, expBarPixels, orbDrawList, hudButtonsDrawList, panelBaseDrawList, staminaDrawList, staminaColours } from './client-rules.mjs';
import { replayOrbs, replayCels, replayPanel, CELL_PANEL, CELL_FLUID, CELL_OVERLAP, CELL_RUN, CELL_MENU } from './draw-list.mjs';

export function resourceRatio(value, maximum) {
  const max = Number(maximum), current = Number(value);
  return Number.isFinite(max) && max > 0 && Number.isFinite(current) ? Math.min(1, Math.max(0, current / max)) : 0;
}

// Retail's XP bar spans the current level only: (exp - levelStart) / (next - levelStart).
export function experienceBarRatio(exp, next, levelStart) {
  const start = Number(levelStart) || 0;
  return resourceRatio(Number(exp) - start, Number(next) - start);
}

// The console is CENTRED between the orbs, and the world shows either side of it. That is what
// retail looks like -- the panel is two orbs plus a centre console, not a solid band across the
// screen. The arithmetic confirms the offsets are exact rather than hand-placed: the orbs are 117
// wide so the space between them is 566, the console's four pieces are 128+128+128+86 = 470, and
// (566 - 470) / 2 = 48, which is centerX 117 + 48 = 165 and the console ending at 635, 48 short of
// the mana orb at 683.
//
// Cycle 110 mistook those two 48px margins for seams and "closed" them by drawing frame 6 in the
// console. Frame 6 is only 39% opaque, so that put real holes in the bar -- 22 of 55 rows at
// column 572 -- in place of margins that were correct. Reverted; see V23.
// OpenDiablo2's HUD uses the 800x600 retail coordinates (256,561), a 120x2 white
// fill.  This renderer is local to the 104px-high panel, hence y = 561 - 496 = 65.
// `d2_session_advancement` owns both values; this module only projects their ratio.
export const CONTROL_LAYOUT = Object.freeze({ healthX: 0, centerX: 165, manaX: 683, orbY: 0, height: 104,
  experienceX: 256, experienceY: 66, experienceWidth: 119, experienceHeight: 2 });

// The panel's TOP ROW on the 800x600 screen. Retail draws 800CtrlPnl7 frame 0 with its bottom row
// at y=599 and it is 104 tall, so its top row is 496 -- the same y this renderer's `y0` lands on,
// which is what lets the HUD pieces below be positioned by retail's own screen coordinates.
export const PANEL_SCREEN_TOP = 496;
// The panel's left edge on the canvas, in whole pixels. A half-pixel origin (an odd width) makes
// drawImage resample every panel cel, and the stamina blend reads those cels back: the resampled
// pixels match no palette entry, so the blend left them as the panel's dark art (the speckle).
export const panelOriginX = (width, scale) => Math.round((width - 800 * scale) / 2);
// The lit add-point buttons ride the orbs out to the screen edges in widescreen: left button x-edge, right x+edge (retail 40 / W-73).
export const levelButtonX = (x, i, edge) => (i ? x + edge : x - edge);

// RETAIL 1.13c'S OWN HUD GEOMETRY, read out of its draw lists (the retail captures,
// a2-town.txt, town-lit.txt; identical in every captured scene that reaches a town or the
// wilderness) rather than derived from pixels or taken from reference/OpenDiablo2. Those sources
// disagree, and retail wins: mini_panel.go puts the container at (325,551) and the button row at
// y=528, which is 24 px above where 1.13c draws them -- the reason the F10 salvage lane's blits
// moved five frame rows away from retail. Every y below is the draw list's y converted to a TOP
// row (a cel's `y` is its inclusive BOTTOM row: top = y - h + 1, ).
//
//   cel minipanel_s.dc6  frame=0 x=323 y=553 w=156 h=26   -> top 528
//   cel minipanelbtn.dc6 frame=0 x=326 y=550 w=20  h=20   -> top 531, then x += 21 each
//                        frames 0,2,4,8,10,12,14          (single player: no party button at 6)
//   cel menubutton.dc6   frame=2 x=392 y=584 w=15  h=24   -> top 561
//   cel runbutton.dc6    frame=0 x=255 y=590 w=16  h=20   -> top 571
//   box 273 573 375 591  color=109 mode=2                 -> the stamina bar, 102x18 of palette 109
//
// The menu button's frame is 2 whenever the icon row is drawn, in every captured scene; frames 0/1
// are its other state and no capture of a collapsed icon row exists, so the collapsed frame is an
// INFERENCE from the art (0/1 are a smaller arrow than 2/3) and is declared as unmeasured in
// V8. The run button follows the ENGINE's run/walk toggle: frame 0 (the
// walking figure) while the hero walks -- the frame in every captured scene, and the one retail's
// own draw list records at (255,590) 16x20 with frame-bytes hash f49f6a84 -- and frame 2 (the same
// figure in its running pose) while it runs. Which of the two draws is the engine's business, never
// this module's: `hud.runToggle` is `d2_session_run_state`, the same flag that decides whether the
// move command is retail's 0x01 Walk packet or its 0x03 Run one. No retail capture has the toggle
// on, so frame 2's identity is asserted against the DC6 itself (s482 decodes it in the scenario and
// compares the hash the renderer recorded) rather than against a retail draw list.
// Every y is a TOP ROW on the 800x600 screen, which is also what recordDc6 and the client's draw
// list take; the renderer subtracts PANEL_SCREEN_TOP only to place them inside the panel transform.
export const HUD_LAYOUT = Object.freeze({
  miniPanel: { path: 'data\\global\\ui\\PANEL\\minipanel_s.dc6', frame: 0, x: 323, y: 528, w: 156, h: 26 },
  miniButtons: { path: 'data\\global\\ui\\PANEL\\minipanelbtn.DC6', x: 326, y: 531, stride: 21,
    frames: [0, 2, 4, 8, 10, 12, 14] },
  menuButton: { path: 'data\\global\\ui\\PANEL\\menubutton.DC6', openFrame: 2, closedFrame: 0,
    x: 392, y: 561 },
  runButton: { path: 'data\\global\\ui\\PANEL\\runbutton.dc6', frame: 0, runFrame: 2, x: 255, y: 571 },
  // The bar is retail's `box ... color=109 mode=2`: the act PL2's blend table at byte 13568 (the
  // level the inventory grid's colour-234 box also uses, 13568 + 2*65536) maps the panel's own
  // index under each pixel through source index 109. Reproduces retail's drawn bar -- measured
  // (176,136,72) from the table against (165,136,72) in retail's frame at its own capture scale.
  // The icon row SLIDES when a side panel is open and is HIDDEN when one is open on each side --
  // OpenDiablo2's mini_panel.go moveLeft/moveRight/SetVisible structure, with retail 1.13c's own
  // distances measured from every draw list that reaches those screens:
  //   charpanel, waypoint-menu (a LEFT panel):  container 323 -> 454, buttons 326 -> 457   = +131
  //   skilltree, skilltree-act5 (a RIGHT panel): container 323 -> 195, buttons 326 -> 198   = -128
  //   stash-town, vendor-malah-act5 (one on EACH side): no minipanel_s, no minipanelbtn at all
  // The run button (255), the stamina box (273) and the arrow (392) keep their x in every one of
  // those lists, so the shift moves the container and the buttons only.
  miniPanelShift: { left: 131, right: -128 },
  stamina: { x: 273, y: 573, width: 102, height: 18, index: 109, pl2Base: 13568 },
  // Two more panel controls retail draws in EVERY captured HUD frame, in the recesses the console
  // art leaves either side of the belt: `level.dc6` frame 2 at x=206 and x=563 (30x30, mode 5,
  // frame-bytes hash dc0759e0), i.e. top row 563 for a bottom row of 592. They are the same asset
  // OpenDiablo2 names as the add-point button (`d2resource.AddSkillButton` = `PANEL/level.DC6`,
  // `DisabledFrame 2`, hero_stats_panel.go/skillicon.go), drawn on the panel in their DISABLED frame
  // -- which is what every one of the nine retail draw lists that reach a HUD records at these
  // anchors (town, town-lit, den-entry, den-interior, den-outside, blood-moor-arrival, cold-plains,
  // a2-town, a3-docks). Retail's frame 0 (the lit state) is drawn -- in den-entry and cold-plains
  // only -- at OTHER anchors, (40,495)/(43,491) and (725,440)/(728,436), socketed by levelsocket.dc6;
  // those two pairs are not drawn here and their trigger is not measured, so nothing is claimed about
  // them. The lit frame at 206/563 is therefore unmeasured too and is NOT
  // invented: the panel draws retail's own measured cel.
  levelButtons: { path: 'data\\global\\ui\\PANEL\\level.DC6', disabledFrame: 2, litFrame: 0,
    // Frame 1 is the held (sunken) gem: 841 opaque pixels against frame 0's 870 in the same 30x30 cel. No draw list
    // carries it because retail draws it only while the button is held, so the frame is measured from the art alone.
    pressedFrame: 1,
    xs: [206, 563], y: 563, w: 30, h: 30,
    // D2Client+0x50480: a pending point DRAWS (in addition to the disabled pair) Levelsocket then Level frame 0 at
    // (40, H-105) / (W-73, H-105) -> socket y 495, cel y 491 at 800x600; the 640x480 anchors (40,375)/(567,375) fit the same rule.
    socketPath: 'data\\global\\ui\\PANEL\\levelsocket.DC6', litXs: [40, 727], socketTop: 460, litTop: 462 },
});

// PERF (F198): the panel's button cels were re-expanded into a fresh canvas on every frame (seven per frame); a frame object is decoded once
// and its canvas only ever drawn from, so it is kept per (frame, palette).
const frameCanvases = new WeakMap();
function frameCanvas(frame, palette, documentObject) {
  let byPalette = frameCanvases.get(frame);
  if (!byPalette) frameCanvases.set(frame, byPalette = new WeakMap());
  const kept = byPalette.get(palette);
  if (kept && kept.doc === documentObject) return kept.canvas;
  const canvas = documentObject.createElement('canvas'); canvas.width = frame.width; canvas.height = frame.height;
  canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  byPalette.set(palette, { canvas, doc: documentObject });
  return canvas;
}

/**
 * Retail's stamina bar is a `box` of palette index 109 composited through the act PL2, not a flat
 * colour: `pl2[base + under * 256 + 109]` is the index each covered pixel takes. `under` is the
 * panel index already on the canvas, so it is read back and mapped, exactly as the inventory grid's
 * colour-234 box does (play/assets/inventory-panel.mjs). At the 800x600 the game is built for the
 * canvas is 1:1 with the screen; above that the panel is scaled and the readback is rounded, which
 * is why the row asserts the geometry and the requested colour rather than this blend.
 */
function createStaminaBlend(read, palette) {
  const pl2 = read('data\\global\\palette\\ACT1\\pal.pl2');
  const index = new Map();
  for (let k = 255; k >= 0; k--) index.set((palette[k * 4] << 16) | (palette[k * 4 + 1] << 8) | palette[k * 4 + 2], k);
  // A readback colour no palette entry matches (a resampled panel, below 800 px wide) takes the
  // nearest entry, memoised in `index`: the blend is then total, so no pixel keeps the panel's art.
  const nearest = (r, g, b) => { let best = 0, bestD = Infinity;
    for (let k = 0; k < 256; k++) { const dr = palette[k * 4] - r, dg = palette[k * 4 + 1] - g, db = palette[k * 4 + 2] - b, d = dr * dr + dg * dg + db * db;
      if (d < bestD) { bestD = d; best = k; } }
    return best; };
  return { index, pl2,
    colour: [palette[109 * 4], palette[109 * 4 + 1], palette[109 * 4 + 2]],
    apply(ctx, x, y, width, height, colour = HUD_LAYOUT.stamina.index) {
      const X = Math.round(x), Y = Math.round(y), W = Math.round(width), H = Math.round(height);
      if (W <= 0 || H <= 0) return [];
      // The readback is in CANVAS pixels, so the panel transform is dropped for it and restored by
      // the matching ctx.restore() -- at the 800x600 the game is built for they are the same grid.
      ctx.save(); ctx.setTransform(1, 0, 0, 1, 0, 0);
      const image = ctx.getImageData(X, Y, W, H), d = image.data, base = HUD_LAYOUT.stamina.pl2Base, seen = new Set();
      for (let i = 0; i < d.length; i += 4) {
        const key = (d[i] << 16) | (d[i + 1] << 8) | d[i + 2];
        let under = index.get(key);
        if (under === undefined) index.set(key, under = nearest(d[i], d[i + 1], d[i + 2]));
        const mapped = pl2[base + under * 256 + colour];
        if (mapped === undefined) continue;
        seen.add((palette[mapped * 4] << 16) | (palette[mapped * 4 + 1] << 8) | palette[mapped * 4 + 2]);
        d[i] = palette[mapped * 4]; d[i + 1] = palette[mapped * 4 + 1]; d[i + 2] = palette[mapped * 4 + 2];
      }
      ctx.putImageData(image, X, Y); ctx.restore();
      return [...seen].map((v) => [(v >> 16) & 255, (v >> 8) & 255, v & 255]);
    } };
}

// The cel a lit add-point button draws: its held frame while the pointer holds it (hud.levelPressed names the index).
export function levelButtonFrame(index, pressed) {
  return Number(pressed) === index ? HUD_LAYOUT.levelButtons.pressedFrame : HUD_LAYOUT.levelButtons.litFrame;
}

export function createControlPanelRenderer(read, documentObject = globalThis.document) {
  const palette = decodeDatPalette(read('data\\global\\palette\\ACT1\\pal.dat'));
  const panel = decodeDc6(read('data\\global\\ui\\PANEL\\800ctrlpnl7.dc6'));
  const fluids = decodeDc6(read('data\\global\\ui\\PANEL\\hlthmana.DC6'));
  const overlaps = decodeDc6(read('data\\global\\ui\\PANEL\\overlap.DC6'));
  if (panel.frames.length !== 7 || fluids.frames.length < 2 || overlaps.frames.length < 2) throw new Error('unexpected retail control-panel frame set');
  const p = panel.frames.map((f) => frameCanvas(f, palette, documentObject));
  const fluid = fluids.frames.slice(0, 2).map((f) => frameCanvas(f, palette, documentObject));
  const overlap = overlaps.frames.slice(0, 2).map((f) => frameCanvas(f, palette, documentObject));
  const FLUID_PATH = 'data\\global\\ui\\PANEL\\hlthmana.dc6', OVERLAP_PATH = 'data\\global\\ui\\PANEL\\overlap.dc6';
  const orbCels = { [CELL_FLUID]: fluid.map((image, i) => ({ image, path: FLUID_PATH, frame: i, dc6: fluids.frames[i] })),
    [CELL_OVERLAP]: overlap.map((image, i) => ({ image, path: OVERLAP_PATH, frame: i, dc6: overlaps.frames[i] })) };
  // The mini-panel, its icon row, the open/close arrow and the run button, each decoded once.
  const miniPanelDc6 = decodeDc6(read(HUD_LAYOUT.miniPanel.path));
  const miniButtonDc6 = decodeDc6(read(HUD_LAYOUT.miniButtons.path));
  const menuButtonDc6 = decodeDc6(read(HUD_LAYOUT.menuButton.path));
  const runButtonDc6 = decodeDc6(read(HUD_LAYOUT.runButton.path));
  const levelButtonDc6 = decodeDc6(read(HUD_LAYOUT.levelButtons.path));
  const socketDc6 = decodeDc6(read(HUD_LAYOUT.levelButtons.socketPath));
  if (!miniPanelDc6.frames.length || miniButtonDc6.frames.length < 15 || menuButtonDc6.frames.length < 3 ||
      runButtonDc6.frames.length <= HUD_LAYOUT.runButton.runFrame ||
      levelButtonDc6.frames.length <= HUD_LAYOUT.levelButtons.pressedFrame)
    throw new Error('unexpected retail mini-panel frame set');
  const buttonCels = {
    [CELL_RUN]: runButtonDc6.frames.map((f, i) => ({ image: frameCanvas(f, palette, documentObject), path: HUD_LAYOUT.runButton.path, frame: i, dc6: f })),
    [CELL_MENU]: menuButtonDc6.frames.map((f, i) => ({ image: frameCanvas(f, palette, documentObject), path: HUD_LAYOUT.menuButton.path, frame: i, dc6: f })) };
  const miniPanel = frameCanvas(miniPanelDc6.frames[HUD_LAYOUT.miniPanel.frame], palette, documentObject);
  // The draw list records EVERY draw, so the HUD is recorded only where a row is comparing: the
  // records are what makes "the thing asked for is the thing drawn" checkable per blit.
  const staminaBlend = (() => { try { return createStaminaBlend(read, palette); } catch { return null; } })();
  let staminaSlots = null;
  const PANEL_PATH = 'data\\global\\ui\\PANEL\\800ctrlpnl7.dc6';
  const panelCels = { [CELL_PANEL]: p.map((image, i) => ({ image, path: PANEL_PATH, frame: i, dc6: panel.frames[i] })) };
  const record = (spec, index, frame, left, top) =>
    recordDc6(spec.path, index, frame, left, top, { op: 'cel', mode: 5 });

  /**
   * The mini-panel's open/close arrow, if the renderer drew it: only a click INSIDE the arrow
   * consumes, so an ordinary world click beside it still walks. The box is the one the last frame
   * actually placed (the same rule objectUnderPointer follows), not a re-derived constant.
   */
  const hitControlBox = (box, x, y, width, height) => {
    if (!box) return false;
    const scale = Math.min(1, width / 800); // the SAME scale draw() uses, or a short window hits off the drawn box
    const x0 = panelOriginX(width, scale), y0 = height - CONTROL_LAYOUT.height * scale;
    const left = x0 + box.left * scale, top = y0 + (box.top - PANEL_SCREEN_TOP) * scale;
    return x >= left && x < left + box.w * scale && y >= top && y < top + box.h * scale;
  };
  const hitMenuButton = (evidence, x, y, width, height) => hitControlBox(evidence?.hud?.menuButton, x, y, width, height);
  // The run/walk toggle, on the box the LAST frame actually placed -- the same rule the arrow and
  // the world's own objectUnderPointer use, so a caller cannot aim at a re-derived constant.
  // The mini-panel icon under (x, y) on the boxes the LAST frame placed: index into miniButtons.frames, or -1.
  const hitMiniButton = (evidence, x, y, width, height) => (evidence?.hud?.miniPanel?.buttons || [])
    .findIndex((b) => hitControlBox(b, x, y, width, height));
  // The button column the pointer is over for the hover text (retail tests the 20-wide column inside the row's
  // box, strictly, not the button art): its index and its left edge in screen pixels.
  const miniButtonScreenLeft = (evidence, index, width) => {
    const b = evidence?.hud?.miniPanel?.buttons?.[index], scale = Math.min(1, width / 800);
    return b ? panelOriginX(width, scale) + b.left * scale : null;
  };
  const hitRunButton = (evidence, x, y, width, height) => hitControlBox(evidence?.hud?.runButton, x, y, width, height);
  function draw(ctx, width, height, snapshot, advancement, hud = {}) {
    const scale = Math.min(1, width / 800), x0 = panelOriginX(width, scale), y0 = height - CONTROL_LAYOUT.height * scale;
    ctx.save(); ctx.translate(x0, y0); ctx.scale(scale, scale);
    // SGD2FreeRes: the orbs sit at the screen edges; `edge` is how far each moves out, 0 at 800x600.
    const edge = x0 / scale;
    // Per-blit records in retail's own order: the six panel frames, then each orb's fluid
    // (celex, extra = the fluid rows shown, of the cel's height) and its overlap.
    // The six base cels are the ported D2Client+0x27290's own D2Gfx calls (draw sink), replayed in its order.
    if (rulesBound()) replayPanel(ctx, panelBaseDrawList(800, 600, 1), panelCels, edge, recordDc6);
    const life = resourceRatio(snapshot?.player?.hp, snapshot?.player?.hpMax);
    const mana = resourceRatio(snapshot?.player?.mana, snapshot?.player?.manaMax);
    // D2Client+0x27BD0 / +0x27A90 (engine rule): fill rows = cur * 80 / max, integer; the float rounding below is the pre-engine fallback.
    // hud.orbs (raw 1/256): a local game's values come smoothed through D2Client+0x26B20 at the client tick, as 27BD0/27A90 read them.
    const pl = snapshot?.player, alive = snapshot?.mode !== 0x11, orbs = hud.orbs || null;
    // The orbs are the ported draw functions' own D2Gfx calls (draw sink), replayed in their order.
    if (rulesBound()) {
      const v = orbs || { life: pl?.hp, lifeMax: pl?.hpMax, mana: pl?.mana, manaMax: pl?.manaMax };
      replayOrbs(ctx, orbDrawList(v.life, v.lifeMax, v.mana, v.manaMax, alive, CELL_FLUID, CELL_OVERLAP), orbCels, edge, recordDc6);
    }
    // The ornate strips between each skill button and the console (wide views only): hud.strips paints them.
    if (edge > 0 && hud.strips) hud.strips(ctx);

    // --- Retail's HUD, in retail's own draw order (den-entry.txt lines 90-157). The run button, the
    // stamina bar and the open/close arrow are drawn on every screen; the icon row slides with a
    // side panel and is hidden when one is open on each side (HUD_LAYOUT.miniPanelShift explains
    // the distances). Panel-local placement: the HUD spec is in retail's screen coordinates, this
    // transform's origin is the panel's top row (PANEL_SCREEN_TOP at 1:1), and the recorder wants
    // the screen back -- hence `local` for the canvas and the raw spec for every record.
    const local = (spec) => ({ x: spec.x, y: spec.y - PANEL_SCREEN_TOP });
    const stamina = hud.stamina || null;
    const staminaRatio = stamina ? resourceRatio(stamina.value, stamina.maximum) : 0;
    // The bar is the ported D2Client+0x278B0's own D2Gfx box (draw sink): its width, colour slot and rectangle are the port's.
    // 278B0 reads raw 1/256 values (its max + 5 blue test is 5/256 of a point): hud.orbs when the engine gave them, else whole points shifted up.
    // The three colours it latches on its first draw come from D2Win #10190 over the palette (staminaColours); [109, 109, 109] without the engine.
    if (stamina && rulesBound() && !staminaSlots) staminaSlots = staminaColours(palette);
    const staminaBox = stamina && rulesBound()
      ? (orbs ? staminaDrawList(orbs.stamina, orbs.staminaMax, false, staminaSlots || Array(3).fill(HUD_LAYOUT.stamina.index))
        : staminaDrawList(stamina.value * 256, stamina.maximum * 256, false, staminaSlots || Array(3).fill(HUD_LAYOUT.stamina.index))) : null;
    const staminaPixels = staminaBox ? staminaBox.x2 - staminaBox.x1 : 0;
    const staminaIndex = staminaBox ? staminaBox.colour : HUD_LAYOUT.stamina.index;
    // The run button's cel follows the engine's own run/walk toggle (hud.runToggle), never a click
    // this module saw: the toggle's action goes to the engine and this reads the engine back.
    const running = hud.runToggle === true;
    // The run button and the arrow are the ported draw functions' own D2Gfx calls (D2Client+0x28320, +0x26880 via the draw sink):
    // which frame (down only while armed AND the pointer is over it) and where are the engine's; this replays them. The pointer is
    // taken into the 800x600 space the ports run in. The records are replayed where each button used to draw (run, then the bar, then the arrow).
    const ptr = hud.hudPointer, bx = ptr ? Math.round((ptr.x - x0) / scale) : -1, by = ptr ? Math.round((ptr.y - y0) / scale) + PANEL_SCREEN_TOP : -1;
    const buttons = rulesBound() ? hudButtonsDrawList(bx, by, running, hud.hudPressed === 'run', hud.arrowOpen !== false, hud.hudPressed === 'menu', CELL_RUN, CELL_MENU) : [];
    const placeButton = (cell, extra) => {
      const [p] = replayCels(ctx, buttons.filter((r) => r.cell === cell), buttonCels,
        (path, frame, dc6, x, top) => recordDc6(path, frame, dc6, x, top, { op: 'cel', mode: 5 }), PANEL_SCREEN_TOP);
      return p ? { ...p.rec, ...extra, left: p.left, top: p.top, w: p.w, h: p.h } : null;
    };
    const run = placeButton(CELL_RUN, { pixels: 1, running });
    const bar = (() => {
      const spec = HUD_LAYOUT.stamina, at = local(spec);
      if (staminaPixels <= 0) return { ...spec, requested: stamina?.value ?? null, next: stamina?.maximum ?? null,
        ratio: staminaRatio, pixels: 0, colours: [], record: null };
      // The bar is drawn INSIDE the panel transform, so the blend reads the canvas back at the
      // transformed rectangle, not at retail's raw screen coordinates.
      const colours = staminaBlend
        ? staminaBlend.apply(ctx, x0 + at.x * scale, y0 + at.y * scale, staminaPixels * scale, spec.height * scale, staminaIndex)
        : [];
      if (!staminaBlend) { ctx.fillStyle = `rgb(${palette[staminaIndex * 4]},${palette[staminaIndex * 4 + 1]},${palette[staminaIndex * 4 + 2]})`;
        ctx.fillRect(at.x, at.y, staminaPixels, spec.height); }
      // Retail's own record: `box 273 573 375 591 color=109 mode=2` at full stamina, so the right
      // and bottom edges are exclusive and the whole 102 px rect is covered when the bar is full.
      recordPrim('box', spec.x, spec.y, spec.x + staminaPixels, spec.y + spec.height, staminaIndex, 2);
      return { ...spec, requested: stamina?.value ?? null, next: stamina?.maximum ?? null,
        ratio: staminaRatio, pixels: staminaPixels, colours, record: { op: 'box', x: spec.x, y: spec.y,
          x2: spec.x + staminaPixels, y2: spec.y + spec.height, colour: staminaIndex, mode: 2 } };
    })();
    // The arrow's cel follows the ARROW's OWN toggle (`hud.arrowOpen`), which is a different state
    // from the row being hidden by a panel on each side: retail keeps menubutton frame 2 while the
    // row is gone in stash-town.txt and vendor-malah-act5.txt (both carry the arrow, neither carries
    // a minipanel record), so only a click on the arrow may show frame 0 -- and no capture of a
    // collapsed row exists, so frame 0 is still the inference the module comment declares. Reading
    // the ROW's visibility here instead is what s391 measures: it drew frame 0 in the stash state.
    const menu = placeButton(CELL_MENU, { collapsed: hud.arrowOpen === false });
    // The two add-point buttons either side of the belt, at retail's own anchors and in retail's own
    // (disabled) frame -- see HUD_LAYOUT.levelButtons.
    // LIT (frame 0, same anchors) while the engine holds points to spend: retail's charpanel-points /
    // skilltree-points frames (s192) show both red at 206/563 at level 2; left = stat points, right =
    // skill points (OpenDiablo2's two add-point buttons; only "both pending" is measured).
    const spec = HUD_LAYOUT.levelButtons;
    const pending = [advancement?.statPoints, advancement?.skillPoints].map((n) => Number(n) > 0);
    const lit = spec.xs.map((bx, i) => {
      if (!pending[i]) return null;
      const index = levelButtonFrame(i, hud.levelPressed);
      const f = levelButtonDc6.frames[index]; // lit cel replaces the grey one in the panel slot, not over the orbs
      ctx.drawImage(frameCanvas(f, palette, documentObject), bx, spec.y - PANEL_SCREEN_TOP);
      return { ...record(spec, index, f, bx, spec.y), left: bx, top: spec.y, w: f.width, h: f.height };
    });
    const levelButtons = spec.xs.map((bx, i) => {
      if (lit[i]) return lit[i];   // the grey cel drawn AFTER the lit one covered it (retail draws the lit over the grey)
      const f = levelButtonDc6.frames[spec.disabledFrame];
      ctx.drawImage(frameCanvas(f, palette, documentObject), bx, spec.y - PANEL_SCREEN_TOP);
      return { ...record(spec, spec.disabledFrame, f, bx, spec.y), left: bx, top: spec.y, w: f.width, h: f.height };
    });
    let mini = null;
    if (hud.iconRow !== false) {
      const spec = HUD_LAYOUT.miniPanel, shift = Math.round(Number(hud.offsetX) || 0);
      ctx.drawImage(miniPanel, spec.x + shift, spec.y - PANEL_SCREEN_TOP);
      // The container's record is emitted HERE, at its own draw, not when the evidence object is
      // built: a record pushed after the buttons loop reads as the container drawn LAST, where
      // retail's list is minipanel_s then its seven buttons (den-entry.txt lines 150-157). s391
      // diffs our own order against retail's, which is the difference this closes.
      const container = record(spec, spec.frame, miniPanelDc6.frames[spec.frame], spec.x + shift, spec.y);
      const buttons = HUD_LAYOUT.miniButtons.frames.map((index, i) => {
        const bf = miniButtonDc6.frames[index + (hud.miniPressed === i ? 1 : 0)], bx = HUD_LAYOUT.miniButtons.x + i * HUD_LAYOUT.miniButtons.stride + shift;
        ctx.drawImage(frameCanvas(bf, palette, documentObject), bx, HUD_LAYOUT.miniButtons.y - PANEL_SCREEN_TOP);
        return { ...record(HUD_LAYOUT.miniButtons, index + (hud.miniPressed === i ? 1 : 0), bf, bx, HUD_LAYOUT.miniButtons.y), left: bx, top: HUD_LAYOUT.miniButtons.y, w: bf.width, h: bf.height };
      });
      mini = { container, buttons, offsetX: shift };
    }

    const experience = Number(advancement?.experience);
    const nextExperience = Number(advancement?.nextExperience);
    const experienceRatio = experienceBarRatio(experience, nextExperience, advancement?.prevExperience);
    // D2's renderer truncates the float width to an integer before its DrawRect call.
    const expStart = Number(advancement?.prevExperience) || 0;
    const experiencePixels = expBarPixels(Math.max(0, experience - expStart), Math.max(0, nextExperience - expStart)); // D2Client+0x26970: into*119/span, rows H-38,H-37
    if (experiencePixels > 0) {
      ctx.fillStyle = '#fff';
      ctx.fillRect(CONTROL_LAYOUT.experienceX, CONTROL_LAYOUT.experienceY,
        experiencePixels, CONTROL_LAYOUT.experienceHeight);
    }
    ctx.restore();
    return { x: x0, y: y0, scale, edge, life, mana, panelFrames: panel.frames.length,
      panelBytes: panel.frames.reduce((sum, frame) => sum + frame.width * frame.height, 0),
      hud: { miniPanel: mini, menuButton: menu, runButton: run, stamina: bar, levelButtons,
        iconRow: hud.iconRow !== false },
      experience: { requested: experience, next: nextExperience, ratio: experienceRatio,
        pixels: experiencePixels, x: x0 + CONTROL_LAYOUT.experienceX * scale,
        y: y0 + CONTROL_LAYOUT.experienceY * scale, width: CONTROL_LAYOUT.experienceWidth * scale,
        height: CONTROL_LAYOUT.experienceHeight * scale } };
  }

  // The add-point button under (x, y) on the box the LAST frame placed, and only while it is LIT
  // (frame 0: the engine holds points to spend): 0 = stat points (left), 1 = skill points (right), else -1.
  // A disabled button (frame 2) is decoration and its click belongs to the world.
  const hitLevelButton = (evidence, x, y, width, height) => (evidence?.hud?.levelButtons || [])
    .findIndex((b) => (b.frame === HUD_LAYOUT.levelButtons.litFrame || b.frame === HUD_LAYOUT.levelButtons.pressedFrame)
      && hitControlBox(b, x, y, width, height));
  return { draw, hitMenuButton, hitRunButton, hitMiniButton, miniButtonScreenLeft, hitLevelButton };
}

// ---- Row 2 (touch): the console gap a shift rest finger may land on ----------------------------
// The console is centred between the orbs, and its middle (x 167..635 on the 800x600 screen) is the
// only stretch of the bottom band that is neither an orb, a skill button nor a retail control drawn
// on the console. A finger that lands in the band there, on nothing the shell can act on, is the
// phone's shift key (input-touch.mjs): it sends nothing itself. The gap is the one the console leaves
// between the two skill buttons (117..167 and 635..685), per the comment on CONTROL_LAYOUT above.
// `top` is the console bar's top row (the mini panel container's bottom row, y=553 above): over the
// gap, the panel's 104-row cel is clear above it, and a mouse click there walks (s315 measures y=520),
// so a finger there must aim like the mouse, not rest.
export const HUD_CONSOLE_GAP = Object.freeze({ left: 167, right: 635, top: 553 });

// The shell's own boxes come in three shapes: {left,top,w,h} (the drawn controls), {left,top,width,
// height} (a panel's rect) and {left,top,right,bottom} (the belt's). One test for all of them.
function hudBoxContains(box, x, y) {
  if (!box) return false;
  const left = box.left, top = box.top;
  const right = box.right ?? (left + (box.w ?? box.width ?? 0));
  const bottom = box.bottom ?? (top + (box.h ?? box.height ?? 0));
  return x >= left && x < right && y >= top && y < bottom;
}

/**
 * True when the canvas point (x, y) -- canvas pixels, the space the touch layer aims in -- is a point
 * of the HUD band that nothing the shell can act on sits under: inside the bottom panel's band, inside
 * the console gap, and outside every control the frame last placed.
 *
 *   `hud`        the shell's controlPanelEvidence.hud (menuButton, runButton, miniPanel, levelButtons);
 *   `belt`       the shell's beltHudBoxes;
 *   `extra`      open panel rects the band overlaps (the inventory's and the stash's): live;
 *   `panelOpen`  true while a panel that covers the band itself is up (the character screen, the skill
 *                tree, the skill popup): then the band is no place for a rest finger at all.
 *
 * The geometry is the renderer's own -- panelOriginX, CONTROL_LAYOUT.height, PANEL_SCREEN_TOP, the
 * scale draw() uses -- so the zone moves with the panel when the window is narrower than 800.
 */
export function isHudDeadZone(x, y, { width = 800, height = 600, hud = {}, belt = [], extra = [], panelOpen = false } = {}) {
  if (panelOpen) return false;
  const scale = Math.min(1, width / 800);
  const x0 = panelOriginX(width, scale), y0 = height - CONTROL_LAYOUT.height * scale;
  // Retail coordinates: x as the 800-wide screen has it, y as a top row on the 800x600 screen.
  const rx = (x - x0) / scale, ry = (y - y0) / scale + PANEL_SCREEN_TOP;
  if (ry < HUD_CONSOLE_GAP.top || ry >= PANEL_SCREEN_TOP + CONTROL_LAYOUT.height) return false;
  if (rx < HUD_CONSOLE_GAP.left || rx >= HUD_CONSOLE_GAP.right) return false;
  const live = [...belt, ...extra, hud.menuButton, hud.runButton, ...(hud.levelButtons || []), ...(hud.miniPanel?.buttons || [])];
  if (hud.miniPanel) {
    const spec = HUD_LAYOUT.miniPanel, shift = Math.round(Number(hud.miniPanel.offsetX) || 0);
    live.push({ left: spec.x + shift, top: spec.y, w: spec.w, h: spec.h });
  }
  live.push({ left: HUD_LAYOUT.stamina.x, top: HUD_LAYOUT.stamina.y, w: HUD_LAYOUT.stamina.width, h: HUD_LAYOUT.stamina.height });
  return !live.some((b) => hudBoxContains(b, rx, ry));
}

// How far (in game px) a finger may miss a small HUD control and still press it. The controls are
// 16x20 (run), 16x16-ish (menu arrow) and 18x18 (the mini-panel icons): 11-14 CSS px of glass at the
// phone floor (0.6867 CSS px per game px), against a finger's own aim error of ~15 game px. Half a
// control alone is 8-10 game px, under that error, so the reach is the larger of the two.
export const HUD_FINGER_REACH = 15;

/**
 * Where a finger at canvas point (x, y) should press, if it landed NEAR one of the HUD's small live
 * controls without being on one: the centre of the nearest control, in canvas pixels; else null.
 *
 * The controls are the ones the shell's own hit tests take: the run button, the menu arrow, the
 * mini-panel icons while the row is shown, and the add-point buttons only while lit. A point already
 * inside one, or inside a belt box or an open panel rect (`extra`), is null: the shell's own hit test
 * decides it exactly as before. Nearest wins, so two neighbouring icons split the gap between them.
 */
export function hudAimPoint(x, y, { width = 800, height = 600, hud = {}, belt = [], extra = [], reach = HUD_FINGER_REACH } = {}) {
  const scale = Math.min(1, width / 800);
  const x0 = panelOriginX(width, scale), y0 = height - CONTROL_LAYOUT.height * scale;
  const rx = (x - x0) / scale, ry = (y - y0) / scale + PANEL_SCREEN_TOP;
  if ([...belt, ...extra].some((b) => hudBoxContains(b, rx, ry))) return null;
  const level = HUD_LAYOUT.levelButtons;
  const controls = [hud.runButton, hud.menuButton, ...(hud.miniPanel?.buttons || []),
    ...(hud.levelButtons || []).filter((b) => b && (b.frame === level.litFrame || b.frame === level.pressedFrame))]
    .filter((b) => b && b.w > 0 && b.h > 0);
  let best = null, bestDistance = Infinity;
  for (const b of controls) {
    if (hudBoxContains(b, rx, ry)) return null;
    const dx = Math.max(b.left - rx, 0, rx - (b.left + b.w)), dy = Math.max(b.top - ry, 0, ry - (b.top + b.h));
    const distance = Math.hypot(dx, dy);
    if (distance > Math.max(reach, Math.min(b.w, b.h) / 2) || distance >= bestDistance) continue;
    best = b; bestDistance = distance;
  }
  if (!best) return null;
  return [x0 + (best.left + best.w / 2) * scale, y0 + (best.top + best.h / 2 - PANEL_SCREEN_TOP) * scale];
}
