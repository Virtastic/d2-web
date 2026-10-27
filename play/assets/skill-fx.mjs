// D18: the hero's client-side skill effects. A cast mode entry starts the skill's Skills.txt `castoverlay`
// (Overlay.txt: Filename, Frames, Trans; D2Client 0x74210/0x74480 create it from skill +0xEA..0xEE); an A1/A2 skill
// with `cltmissilea` (Sacrifice: blood1, cltdofunc 34 = D2Client 0x2B530) spawns that client missile on the hero
// at the do frame, which the server marks by the self-damage hp drop. Frames advance AnimRate/256 per tick (Overlay.txt / Missiles.txt `AnimRate`, 256 when absent). Limits (unverified): the column semantics.
const CAST = new Set([10, 13, 14, 15, 16]), ATTACK = new Set([7, 8]);
export const table = (read, p) => { const r = new TextDecoder('latin1').decode(read(p)).split(/\r?\n/).map((l) => l.split('\t')), h = r[0];
  return r.slice(1).filter((c) => c[0]).map((c) => Object.fromEntries(h.map((k, i) => [k, c[i]]))); };
export function createSkillFx(read) {
  const overlays = new Map(table(read, 'data\\global\\excel\\Overlay.txt').map((o) => [o.overlay, o]));
  const missiles = new Map(table(read, 'data\\global\\excel\\Missiles.txt').map((m) => [m.Missile, m]));
  const skills = new Map(table(read, 'data\\global\\excel\\skills.txt').map((s) => [Number(s.Id), s]));
  let mode, frame, hp, tick = 0, active = [];
  return {
    /** Per engine tick. Returns the effects now alive: {kind:'overlay',file,frames,trans,age} | {kind:'missile',classId,age}. */
    update(hero, skillId) {
      tick++;
      const fp = hero?.animation?.framePrecise, entry = hero && hero.mode !== mode || (hero && fp < frame);
      const s = skills.get(skillId);
      if (hero && entry && s) {
        const o = overlays.get(s.castoverlay);
        if ((CAST.has(hero.mode) || ATTACK.has(hero.mode)) && o) active.push({ kind: 'overlay', file: o.Filename, frames: Number(o.Frames) || 1, rate: Number(o.AnimRate) || 256, trans: Number(o.Trans) || 0, start: tick });
      }
      const cur = hero?.hp;
      if (hero && ATTACK.has(hero.mode) && s?.cltmissilea && missiles.has(s.cltmissilea) && hp !== undefined && cur < hp)
        active.push({ kind: 'missile', classId: Number(missiles.get(s.cltmissilea).Id), frames: 9, rate: Number(missiles.get(s.cltmissilea).AnimRate) || 256, trans: 0, start: tick });
      mode = hero?.mode; frame = fp; hp = cur;
      active = active.filter((a) => (tick - a.start) * a.rate / 256 < a.frames);
      return active.map((a) => ({ ...a, age: Math.floor((tick - a.start) * a.rate / 256) }));
    },
  };
}
