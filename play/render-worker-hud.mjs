// In-game HUD (control panel + belt) as a render-worker pass: (ctx, frame, assets, env), reading frame.meta.hud.
import { createControlPanelRenderer, HUD_LAYOUT } from './assets/control-panel.mjs';
import { createBeltRenderer } from './assets/belt-renderer.mjs';
import { hudOrigin } from './assets/hud-origin.mjs';

// The ops these two renderers push into the draw list.
export const HUD_OPS = Object.freeze(['cel', 'celex', 'box']);

const num = (v) => (Number.isFinite(Number(v)) ? Number(v) : 0);

// Pure: exactly what the control-panel and belt renderers read, as plain data. Inputs are the values the
// gameplay shell holds at the HUD call site; w/h are the canvas size.
export function hudState({ w, h, snapshot, advancement, screen, left = false, right = false, iconRow = true, runToggle = false,
  miniDown = null, beltBoxes = null, beltKeys = null, clearScreen = false } = {}) {
  if (clearScreen) return null;
  const p = snapshot?.player ?? {}, a = advancement ?? {}, shift = HUD_LAYOUT.miniPanelShift;
  return {
    w, h,
    player: { hp: num(p.hp), hpMax: num(p.hpMax), mana: num(p.mana), manaMax: num(p.manaMax) },
    advancement: { statPoints: num(a.statPoints), skillPoints: num(a.skillPoints), experience: a.experience, nextExperience: a.nextExperience, prevExperience: a.prevExperience },
    stamina: screen ? { value: screen[4], maximum: screen[5] } : null,
    iconRow: iconRow && !(left && right),
    offsetX: left && !right ? shift.left : right && !left ? shift.right : 0,
    arrowOpen: !!iconRow, runToggle: runToggle === true, miniPressed: miniDown ?? null,
    wideStrips: w > 800,   // main still paints the free-resolution strips (needs its art); not drawn in the worker
    items: (snapshot?.items ?? []).filter((it) => it.mode === 2).map((it) => ({ mode: 2, gridX: it.gridX, invFile: it.invFile ?? '' })),
    beltBoxes: (beltBoxes ?? []).map((b) => ({ left: b.left, right: b.right, top: b.top, bottom: b.bottom })),
    beltKeys: beltKeys ? [...beltKeys] : undefined,
  };
}

let built = null, builtEnv = null;
export function resetHud() { built = null; builtEnv = null; }

export function hudDraw(ctx, frame, assets, env) {
  const s = frame?.meta?.hud;
  if (!env || !s) return;
  if (builtEnv !== env || !built) {
    built = {};
    builtEnv = env;
  }
  try {
    built.panel ??= createControlPanelRenderer(env.read, env.documentObject);
    built.belt ??= createBeltRenderer(env.read, env.documentObject);
    const panelEv = built.panel.draw(ctx, s.w, s.h, { player: s.player }, s.advancement,
      { iconRow: s.iconRow, offsetX: s.offsetX, arrowOpen: s.arrowOpen, runToggle: s.runToggle, miniPressed: s.miniPressed, strips: null, stamina: s.stamina });
    const bo = hudOrigin(s.w, s.h, 'bottom');
    ctx.save(); ctx.translate(bo.x, bo.y);
    try { const beltEv = built.belt.draw(ctx, s.items, s.beltBoxes, true, s.beltKeys);
      if (env.evidence) { env.evidence.controlPanel = panelEv; env.evidence.belt = { ...beltEv, origin: { x: bo.x, y: bo.y } }; } } finally { ctx.restore(); }
  } catch {
    // A file not here yet is recorded in env.missed by read(); rebuild both renderers next frame.
    if (env.missed && env.missed.size) resetHud();
  }
}
