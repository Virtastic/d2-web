// D18: the hero's client-side skill effects. A cast mode entry starts the skill's Skills.txt `castoverlay`
// (Overlay.txt: Filename, Frames, Trans; D2Client 0x74210/0x74480 create it from skill +0xEA..0xEE); an A1/A2 skill
// with `cltmissilea` (Sacrifice: blood1, cltdofunc 34 = D2Client 0x2B530) spawns that client missile on the hero
// at the do frame, which the server marks by the self-damage hp drop. A castoverlay's frames advance one update per tick by `AnimRate << 4` in 1/256 frame units (D2Client 0x6B300 sets the node's per-update step to Overlay.txt AnimRate << 4; 0x6C130 adds it to framePrecise), so Overlay.txt AnimRate 16 (ice_cast_2) is one frame per tick. Missiles.txt `AnimRate` is unchanged here. Limits (unverified): the column semantics.
const CAST = new Set([10, 13, 14, 15, 16, 18]), ATTACK = new Set([7, 8]);
// Cast entry: D2Client 0xA2C90 spawns the skill's cltmissile (skill +0xE8) first; then cltdofunc 23 (0x74AC0), 25 (0x73CB0) and 27 (0x73E80) spawn cltmissilea (+0xEA). Ponytail: one missile; 23 spawns N bolts and 27 is target-gated, neither modelled.
const CAST_SPAWN = new Set([23, 25, 27]);
// `hold`: the non-looping Sacrifice missile keeps its last frame (see update); every other effect ends after Range frames.
const spawn = (m, hero, tick, hold = false) => { const len = Number(m.AnimLen) || 9;
  return { kind: 'missile', classId: Number(m.Id), frames: Number(m.Range) || len, animLen: len, hold, loop: m.LoopAnim === '1', rate: Number(m.animrate) || 256, trans: 0, predraw: true, start: tick, wx: hero.x, wy: hero.y }; };
// Overlay.txt AnimRate -> the node's per-update step in 1/256 frame units (D2Client 0x6B300: `AnimRate << 4`). Every overlay (castoverlay, state overlay, unit hit) uses it.
export const overlayStep = (row) => (Number(row.AnimRate) || 16) << 4;
export const table = (read, p) => { const r = new TextDecoder('latin1').decode(read(p)).split(/\r?\n/).map((l) => l.split('\t')), h = r[0];
  return r.slice(1).filter((c) => c[0]).map((c) => Object.fromEntries(h.map((k, i) => [k, c[i]]))); };
export function createSkillFx(read) {
  const overlays = new Map(table(read, 'data\\global\\excel\\Overlay.txt').map((o) => [o.overlay, o]));
  const missiles = new Map(table(read, 'data\\global\\excel\\Missiles.txt').map((m) => [m.Missile, m]));
  const skills = new Map(table(read, 'data\\global\\excel\\skills.txt').map((s) => [Number(s.Id), s]));
  let mode, frame, hp, tick = 0, active = [];
  return {
    /** Per render frame, `now` = the engine tick (25/s). Returns the effects now alive: {kind:'overlay',file,frames,trans,age} | {kind:'missile',classId,age}. */
    update(hero, skillId, now) {
      tick = now ?? tick + 1;
      const fp = hero?.animation?.framePrecise, entry = hero && hero.mode !== mode || (hero && fp < frame);
      const s = skills.get(skillId);
      if (hero && entry && s) {
        const o = overlays.get(s.castoverlay);
        if ((CAST.has(hero.mode) || ATTACK.has(hero.mode)) && o) active.push({ kind: 'overlay', file: o.Filename, frames: Number(o.Frames) || 1, rate: overlayStep(o), trans: Number(o.Trans) || 0, start: tick });
        const cm = CAST.has(hero.mode) && (s.cltmissile || (CAST_SPAWN.has(Number(s.cltdofunc)) && s.cltmissilea));
        // a flying missile (Vel > 0: Fire Bolt) is the engine's own missile unit, drawn where it is; a static copy at the hero never ends
        if (cm && missiles.has(cm) && !(Number(missiles.get(cm).Vel) > 0)) active.push(spawn(missiles.get(cm), hero, tick));
      }
      const cur = hero?.hp;
      if (hero && ATTACK.has(hero.mode) && s?.cltmissilea && missiles.has(s.cltmissilea) && hp !== undefined && cur < hp) {
        // D2Client 0xB17C0: a non-looping client missile holds its last AnimLen frame; a retail recording shows retail still drawing it at tick 400 (Range 128 from 19), so no expiry here (ponytail: freed with the area/session; add a room-unload free if casts pile up).
        active.push(spawn(missiles.get(s.cltmissilea), hero, tick, true));
      }
      mode = hero?.mode; frame = fp; hp = cur;
      active = active.filter((a) => a.hold || (a.kind === 'missile' ? tick - a.start : (tick - a.start) * a.rate / 256) < a.frames)  // a missile lives Range ticks;
      return active.map((a) => ({ ...a, age: a.loop ? Math.floor((tick - a.start) * a.rate / 256) % a.animLen : Math.min(Math.floor((tick - a.start) * a.rate / 256), (a.animLen ?? Infinity) - 1) }));
    },
  };
}
