import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';
import { recordDc6, recordPrim } from './drawlist.mjs';

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
    apply(ctx, x, y, width, height) {
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
        const mapped = pl2[base + under * 256 + HUD_LAYOUT.stamina.index];
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
  const miniPanel = frameCanvas(miniPanelDc6.frames[HUD_LAYOUT.miniPanel.frame], palette, documentObject);
  // The draw list records EVERY draw, so the HUD is recorded only where a row is comparing: the
  // records are what makes "the thing asked for is the thing drawn" checkable per blit.
  const staminaBlend = (() => { try { return createStaminaBlend(read, palette); } catch { return null; } })();
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
  const hitRunButton = (evidence, x, y, width, height) => hitControlBox(evidence?.hud?.runButton, x, y, width, height);
  function draw(ctx, width, height, snapshot, advancement, hud = {}) {
    const scale = Math.min(1, width / 800), x0 = panelOriginX(width, scale), y0 = height - CONTROL_LAYOUT.height * scale;
    ctx.save(); ctx.translate(x0, y0); ctx.scale(scale, scale);
    // SGD2FreeRes: the orbs sit at the screen edges; `edge` is how far each moves out, 0 at 800x600.
    const edge = x0 / scale;
    // Per-blit records in retail's own order: the six panel frames, then each orb's fluid
    // (celex, extra = its percentage) and its overlap.
    const PANEL_PATH = 'data\\global\\ui\\PANEL\\800ctrlpnl7.dc6', top = PANEL_SCREEN_TOP;
    ctx.drawImage(p[0], CONTROL_LAYOUT.healthX - edge, 0);
    recordDc6(PANEL_PATH, 0, panel.frames[0], CONTROL_LAYOUT.healthX - edge, top);
    let x = CONTROL_LAYOUT.centerX; for (let i = 1; i <= 4; i++) { ctx.drawImage(p[i], x, CONTROL_LAYOUT.height - p[i].height + 1); recordDc6(PANEL_PATH, i, panel.frames[i], x, top + CONTROL_LAYOUT.height - p[i].height + 1); x += p[i].width; }
    ctx.drawImage(p[5], CONTROL_LAYOUT.manaX + edge, 0);
    recordDc6(PANEL_PATH, 5, panel.frames[5], CONTROL_LAYOUT.manaX + edge, top);
    const life = resourceRatio(snapshot?.player?.hp, snapshot?.player?.hpMax);
    const mana = resourceRatio(snapshot?.player?.mana, snapshot?.player?.manaMax);
    const drawFluid = (image, x, y, ratio) => { const top = Math.round(image.height * (1 - ratio)), shown = image.height - top;
      if (shown > 0) ctx.drawImage(image, 0, top, image.width, shown, x, y + top, image.width, shown); };
    drawFluid(fluid[0], 29 - edge, 12, life); drawFluid(fluid[1], 689 + edge, 12, mana);
    ctx.drawImage(overlap[0], 28 - edge, 12); ctx.drawImage(overlap[1], 690 + edge, 8);
    const FLUID_PATH = 'data\\global\\ui\\PANEL\\hlthmana.dc6', OVERLAP_PATH = 'data\\global\\ui\\PANEL\\overlap.dc6';
    recordDc6(FLUID_PATH, 0, fluids.frames[0], 29 - edge, top + 12, { op: 'celex', extra: Math.round(life * 100) });
    recordDc6(OVERLAP_PATH, 0, overlaps.frames[0], 28 - edge, top + 12);
    recordDc6(FLUID_PATH, 1, fluids.frames[1], 689 + edge, top + 12, { op: 'celex', extra: Math.round(mana * 100) });
    recordDc6(OVERLAP_PATH, 1, overlaps.frames[1], 690 + edge, top + 8);
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
    const staminaPixels = Math.floor(staminaRatio * HUD_LAYOUT.stamina.width);
    // The run button's cel follows the engine's own run/walk toggle (hud.runToggle), never a click
    // this module saw: the toggle's action goes to the engine and this reads the engine back.
    const running = hud.runToggle === true;
    const run = (() => { const spec = HUD_LAYOUT.runButton, at = local(spec), index = running ? spec.runFrame : spec.frame;
      const f = runButtonDc6.frames[index];
      ctx.drawImage(frameCanvas(f, palette, documentObject), at.x, at.y);
      return { ...record(spec, index, f, spec.x, spec.y), pixels: 1, running,
        left: spec.x, top: spec.y, w: f.width, h: f.height }; })();
    const bar = (() => {
      const spec = HUD_LAYOUT.stamina, at = local(spec);
      if (staminaPixels <= 0) return { ...spec, requested: stamina?.value ?? null, next: stamina?.maximum ?? null,
        ratio: staminaRatio, pixels: 0, colours: [], record: null };
      // The bar is drawn INSIDE the panel transform, so the blend reads the canvas back at the
      // transformed rectangle, not at retail's raw screen coordinates.
      const colours = staminaBlend
        ? staminaBlend.apply(ctx, x0 + at.x * scale, y0 + at.y * scale, staminaPixels * scale, spec.height * scale)
        : [];
      if (!staminaBlend) { ctx.fillStyle = `rgb(${palette[spec.index * 4]},${palette[spec.index * 4 + 1]},${palette[spec.index * 4 + 2]})`;
        ctx.fillRect(at.x, at.y, staminaPixels, spec.height); }
      // Retail's own record: `box 273 573 375 591 color=109 mode=2` at full stamina, so the right
      // and bottom edges are exclusive and the whole 102 px rect is covered when the bar is full.
      recordPrim('box', spec.x, spec.y, spec.x + staminaPixels, spec.y + spec.height, spec.index, 2);
      return { ...spec, requested: stamina?.value ?? null, next: stamina?.maximum ?? null,
        ratio: staminaRatio, pixels: staminaPixels, colours, record: { op: 'box', x: spec.x, y: spec.y,
          x2: spec.x + staminaPixels, y2: spec.y + spec.height, colour: spec.index, mode: 2 } };
    })();
    // The arrow's cel follows the ARROW's OWN toggle (`hud.arrowOpen`), which is a different state
    // from the row being hidden by a panel on each side: retail keeps menubutton frame 2 while the
    // row is gone in stash-town.txt and vendor-malah-act5.txt (both carry the arrow, neither carries
    // a minipanel record), so only a click on the arrow may show frame 0 -- and no capture of a
    // collapsed row exists, so frame 0 is still the inference the module comment declares. Reading
    // the ROW's visibility here instead is what s391 measures: it drew frame 0 in the stash state.
    const menu = (() => { const spec = HUD_LAYOUT.menuButton, at = local(spec), index = hud.arrowOpen === false ? spec.closedFrame : spec.openFrame;
      const f = menuButtonDc6.frames[index];
      ctx.drawImage(frameCanvas(f, palette, documentObject), at.x, at.y);
      return { ...record(spec, index, f, spec.x, spec.y), collapsed: hud.arrowOpen === false,
        left: spec.x, top: spec.y, w: f.width, h: f.height }; })();
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
    const experiencePixels = Math.floor(experienceRatio * CONTROL_LAYOUT.experienceWidth); // D2Client+0x26970: into*119/span, rows H-38,H-37
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
  return { draw, hitMenuButton, hitRunButton, hitMiniButton, hitLevelButton };
}
