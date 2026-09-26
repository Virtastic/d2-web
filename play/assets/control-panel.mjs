import { decodeDatPalette, decodeDc6, dc6FrameToRgba } from './d2-formats.mjs';

export function resourceRatio(value, maximum) {
  const max = Number(maximum), current = Number(value);
  return Number.isFinite(max) && max > 0 && Number.isFinite(current) ? Math.min(1, Math.max(0, current / max)) : 0;
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
  experienceX: 256, experienceY: 65, experienceWidth: 120, experienceHeight: 2 });

function frameCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas'); canvas.width = frame.width; canvas.height = frame.height;
  canvas.getContext('2d').putImageData(new ImageData(dc6FrameToRgba(frame, palette), frame.width, frame.height), 0, 0);
  return canvas;
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
  return { draw(ctx, width, height, snapshot, advancement) {
    const scale = Math.min(1, width / 800), panelWidth = 800 * scale, x0 = (width - panelWidth) / 2, y0 = height - CONTROL_LAYOUT.height * scale;
    ctx.save(); ctx.translate(x0, y0); ctx.scale(scale, scale);
    ctx.drawImage(p[0], CONTROL_LAYOUT.healthX, 0);
    let x = CONTROL_LAYOUT.centerX; for (let i = 1; i <= 4; i++) { ctx.drawImage(p[i], x, CONTROL_LAYOUT.height - p[i].height); x += p[i].width; }
    ctx.drawImage(p[5], CONTROL_LAYOUT.manaX, 0);
    const life = resourceRatio(snapshot?.player?.hp, snapshot?.player?.hpMax);
    const mana = resourceRatio(snapshot?.player?.mana, snapshot?.player?.manaMax);
    const drawFluid = (image, x, y, ratio) => { const top = Math.round(image.height * (1 - ratio)), shown = image.height - top;
      if (shown > 0) ctx.drawImage(image, 0, top, image.width, shown, x, y + top, image.width, shown); };
    drawFluid(fluid[0], 30, 14, life); drawFluid(fluid[1], 691, 14, mana);
    ctx.drawImage(overlap[0], 28, 6); ctx.drawImage(overlap[1], 691, 10);
    const experience = Number(advancement?.experience);
    const nextExperience = Number(advancement?.nextExperience);
    const experienceRatio = resourceRatio(experience, nextExperience);
    // D2's renderer truncates the float width to an integer before its DrawRect call.
    const experiencePixels = Math.floor(experienceRatio * CONTROL_LAYOUT.experienceWidth);
    if (experiencePixels > 0) {
      ctx.fillStyle = '#fff';
      ctx.fillRect(CONTROL_LAYOUT.experienceX, CONTROL_LAYOUT.experienceY,
        experiencePixels, CONTROL_LAYOUT.experienceHeight);
    }
    ctx.restore();
    return { x: x0, y: y0, scale, life, mana, panelFrames: panel.frames.length,
      panelBytes: panel.frames.reduce((sum, frame) => sum + frame.width * frame.height, 0),
      experience: { requested: experience, next: nextExperience, ratio: experienceRatio,
        pixels: experiencePixels, x: x0 + CONTROL_LAYOUT.experienceX * scale,
        y: y0 + CONTROL_LAYOUT.experienceY * scale, width: CONTROL_LAYOUT.experienceWidth * scale,
        height: CONTROL_LAYOUT.experienceHeight * scale } };
  } };
}
