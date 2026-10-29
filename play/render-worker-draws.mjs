// Draw passes the render worker runs. Stage 4: terrain with units interleaved.
// A pass is (ctx, frame, assets, env); env = { read, decodeDt1, decodeDatPalette, documentObject, missed }.
import { createWeatherRenderer } from './assets/weather-renderer.mjs';
import { weatherWaterAnchors } from './assets/weather-water-anchors.mjs';
import { createSessionTerrain, worldToScreen, UNIT_CENTRE_Y } from './assets/session-terrain.mjs';
import { createPlayerRenderer } from './assets/unit-renderer.mjs';
import { createMonsterRenderer } from './assets/monster-renderer.mjs';
import { createObjectRenderer } from './assets/object-renderer.mjs';
import { createMissileRenderer } from './assets/missile-renderer.mjs';
import { createItemRenderer } from './assets/item-renderer.mjs';
import { createNpcSpeechAlertRenderer } from './assets/npc-speech-alert.mjs';
import { unitAnchor } from './assets/unit-anchor.mjs';
import { hudDraw, resetHud } from './render-worker-hud.mjs';

// The ops recordTile can push: terrainDrawOp's whole range (terrain-draw-identity.mjs).
export const TERRAIN_OPS = Object.freeze(['floor', 'shadowtile', 'roof', 'wall']);
// Unit/object/missile records: the unit renderers push 'unit' (body layers), objects push 'dcc'.
export const UNIT_OPS = Object.freeze(['unit', 'dcc']);
// Ground items, NPC balloons and heroFx overlays record no ops of their own beyond these (sprite blits through the
// unit/missile paths), so the covered set is unchanged; weather is drawn in the worker too and records no covered op.
const OPS = new Set([...TERRAIN_OPS, ...UNIT_OPS]);
// HUD_OPS (render-worker-hud.mjs) stay out: 'box' is also drawn by UI screens the worker does not own.
export const covers = (record) => Boolean(record) && OPS.has(record.op);
export { resetHud };

const sessions = new Map();   // palettePath -> session terrain (one per env, reset with resetTerrain)
let sessionsEnv = null;
export function resetTerrain() { sessions.clear(); sessionsEnv = null; }

let weatherR = null, weatherEnv = null;
export function resetWeather() { weatherR = null; weatherEnv = null; }
function weatherFor(env) {
  if (weatherEnv !== env) { weatherR = null; weatherEnv = env; }
  if (!weatherR) weatherR = createWeatherRenderer({ read: env.read, documentObject: env.documentObject });
  return weatherR;
}

export function terrainDraw(ctx, frame, assets, env) {
  const meta = frame.meta ?? {}, tiles = frame.tiles ?? frame.terrain;
  if (!env || !meta.palettePath || !tiles || !tiles.length) return;
  if (sessionsEnv !== env) { sessions.clear(); sessionsEnv = env; }
  // Ask for every DT1 library at once (one round trip), not one per rebuilt-and-discarded session.
  let pre = false;
  for (const p of frame.files ?? []) { try { env.read(p); } catch { pre = true; } }
  if (pre) return;
  let terrain = sessions.get(meta.palettePath);
  if (!terrain) {
    terrain = createSessionTerrain({ read: env.read, decodeDt1: env.decodeDt1, decodeDatPalette: env.decodeDatPalette,
      palettePath: meta.palettePath, documentObject: env.documentObject });
    sessions.set(meta.palettePath, terrain);
  }
  const lm = frame.lightmap;
  terrain.setLightGrid(lm ? (a, b) => lm.grid(a, b) : null, lm?.at, lm?.ambient, lm);
  const sprites = buildSprites(ctx, frame, env);
  let stats = null;
  const wx = meta.weather, wr = wx ? weatherFor(env) : null;
  if (wr) { try { wr.setLevel(wx.rain); wr.advance(wx.ticks); } catch { /* missed files are in env.missed */ } }
  const wcb = wr ? (c, floors) => { try { wr.draw(c, meta.width, meta.height, wx.ticks, wx.wcam.x, wx.wcam.y, weatherWaterAnchors(floors, wx.nearFeed)); } catch { /* a broken weather does not take the world down */ } } : null;
  try { stats = terrain.draw(ctx, tiles, frame.files ?? [], meta.player, meta.width, meta.height, sprites, wcb, meta.viewCX); }
  finally {
    // A file that was not here yet poisons the session's caches (a failed library or palette is remembered): rebuild next frame.
    // Only a terrain file poisons it; a sprite file still in flight leaves the (expensive) session intact.
    if (env.missed && [...env.missed].some((m) => /\.(dt1|pal|pl2|dat)$/i.test(m))) sessions.delete(meta.palettePath);
  }
  if (env.evidence) env.evidence.terrainStats = stats;
  if (wr && wx.rain > 0) { try { wr.drawStreaks(ctx, meta.width, meta.height, wx.ticks, wx.cam.x, wx.cam.y); } catch { /* as above */ } }
  if (!stats?.drawn) for (const sp of sprites) sp.draw(ctx);   // no wall pass to interleave into (as main)
  endUnits(env, sprites);
}

// Units, objects, missiles. Main hands sprites to the terrain pass (depth-sorted against walls); so does the worker.
let renderers = null, renderersEnv = null;
const buildRenderers = (env, frame) => {
  const rd = env.read, doc = env.documentObject, cels = frame.meta?.missileCels;
  const fxCels = {}, lazy = (make) => { let r = null; return { draw: (...a) => (r ??= make()).draw(...a) }; };
  return { fxCels, item: lazy(() => createItemRenderer(rd, doc)), speech: lazy(() => createNpcSpeechAlertRenderer(rd)),
    fxMissile: lazy(() => createMissileRenderer(rd, doc, (c) => fxCels[c] ?? '')), fxOverlay: lazy(() => createMissileRenderer(rd, doc, (f) => f, 'overlays')), player: createPlayerRenderer(rd, doc, { shadow: true }), monster: createMonsterRenderer(rd, doc, { shadow: true }),
    object: createObjectRenderer(rd, doc, { shadow: true }), missile: createMissileRenderer(rd, doc, (c) => cels?.[c] ?? '') };
};
export function resetUnits() { renderers = null; renderersEnv = null; }

// The sprites main hands the terrain pass: same order (entities, objects), same depth/wx/wy/pass keys, so the
// terrain pass's own overlay sort places them. The local player (meta.localPlayer) goes last, as main pushes it;
// its heroFx overlays draw around it as main's drawPlayer does. Weather stays on main: it
// needs the engine's active-near feed and rain level, which the frame does not carry yet.
export function buildSprites(ctx, frame, env) {
  const meta = frame.meta ?? {}, units = frame.units ?? [], lp = meta.localPlayer;
  if (!env || (!units.length && !lp && !meta.items?.length) || !meta.player) return [];
  if (renderersEnv !== env) { renderers = buildRenderers(env, frame); renderersEnv = env; }
  const cam = worldToScreen(meta.player.x - 0.5, meta.player.y - 0.5), h = meta.height, act = meta.act ?? 0, tick = meta.tick ?? 0;
  const proj = (wx, wy) => { const p = worldToScreen(wx, wy); return [p.x - cam.x + meta.viewCX, p.y - cam.y + (h / 2 + UNIT_CENTRE_Y)]; };
  const facings = new Map((frame.facings ?? []).map((f) => [f.id, f]));
  const guard = (fn) => (c) => { try { fn(c); } catch { sprites.failed = true; } };   // a file not here yet is recorded in env.missed by read()
  const sprites = [], hover = meta.hover, hov = (kind, id) => hover?.kind === kind && hover.id === id, speech = new Set(meta.speechIds ?? []);
  const ordered = units.filter((u) => u.kind !== 'object').concat(units.filter((u) => u.kind === 'object'));
  for (const u of ordered) {
    if (u.kind === 'item' || (u.kind === 'player' && u.id === meta.playerId)) continue;
    const f = facings.get(u.id), e = f ? { ...u, direction: f.direction, hasDynamicPath: f.hasDynamicPath } : u;
    if (u.kind === 'object') {
      const [sx, sy] = proj(u.x - 0.5, u.y - 0.5);
      sprites.push({ depth: sy, wx: u.x, wy: u.y, pass: 'object', draw: guard((c) => renderers.object.draw(c, sx, sy, e, act, hov('object', u.id))) });
      continue;
    }
    const [sx, sy] = proj(...unitAnchor(e));
    if (u.kind === 'missile') {
      const m = e.missile;
      sprites.push({ depth: sy, wx: u.x, wy: u.y, pass: 'missile', draw: guard((c) => { if (m) renderers.missile.draw(c, sx, sy, e, m.direction, tick, m.framePrecise >>> 8, m.trans, act, false); }) });
    } else if (u.kind === 'player') {
      sprites.push({ depth: sy, wx: u.x, wy: u.y, pass: 'player-corpse', draw: guard((c) => renderers.player.draw(c, sx, sy, { playerClass: e.classId, mode: e.mode, direction: e.direction ?? 0, tick, act })) });
    } else {
      sprites.push({ depth: sy, wx: u.x, wy: u.y, draw: guard((c) => {
        // Unit hit overlay (Overlay.txt via unit-overlay-fx.mjs): PreDraw draws behind the body, else over it.
        // Active state overlays (States.txt overlay1-4, Inner Sight, curses, Chilled...) draw in the same PreDraw split.
        const ov = e.overlay, states = e.stateOverlays ?? [];
        const drawOv = (pre) => { for (const f of states) if (f.predraw === pre) renderers.fxOverlay.draw(c, sx, sy, { classId: f.file }, 0, 0, f.age, f.trans === 3 ? 1 : 0, act);
          if (ov && ov.predraw === pre) renderers.fxOverlay.draw(c, sx, sy, { classId: ov.file }, 0, 0, ov.age, ov.trans === 3 ? 1 : 0, act); };
        drawOv(true);
        if (e.animation) renderers.monster.draw(c, sx, sy, e.animation, act, hov('unit', u.id));
        drawOv(false);
        renderers.speech.draw(c, sx, sy, e, speech.has(u.id), tick, act);   // NPC overhead balloon, after the body as main
      }) });
    }
  }
  for (const it of meta.items ?? []) {
    const [sx, sy] = proj(it.x - 0.5, it.y - 0.5);
    sprites.push({ depth: sy, wx: it.x, wy: it.y, pass: 'item', draw: guard((c) => renderers.item.draw(c, sx, sy, it, hov('item', it.id))) });
  }
  if (lp) {
    const py = h / 2 + UNIT_CENTRE_Y, fx = meta.heroFx ?? [];
    for (const f of fx) if (f.kind === 'missile') renderers.fxCels[f.classId] = f.cel ?? '';
    const drawFx = (c, pre) => { for (const f of fx) if (f.predraw === pre) (f.kind === 'overlay' ? renderers.fxOverlay : renderers.fxMissile)
      .draw(c, ...(f.wx === undefined ? [meta.viewCX, py] : proj(f.wx - 0.5, f.wy - 0.5)), { classId: f.kind === 'overlay' ? f.file : f.classId }, 0, 0, f.age, f.trans === 3 ? 1 : 0, act); };
    sprites.push({ depth: py, wx: lp.x, wy: lp.y, pass: 'player', draw: guard((c) => {
      try { drawFx(c, true); } catch { sprites.failed = true; }   // not fatal (as main); a missed file resets the renderers
      renderers.player.draw(c, meta.viewCX, py, lp.renderState);
      try { drawFx(c, false); } catch { sprites.failed = true; }
    }) });
  }
  return sprites;
}
const endUnits = (env, sprites) => { if (sprites.failed && env.missed && env.missed.size) resetUnits(); };   // renderers remember failed loads

// Standalone unit pass (no terrain to interleave with): sprites in frame order.
export function unitsDraw(ctx, frame, assets, env) {
  const sprites = buildSprites(ctx, frame, env);
  for (const s of sprites) s.draw(ctx);
  endUnits(env, sprites);
}

export const draws = [terrainDraw, hudDraw];
