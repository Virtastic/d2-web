// D18: the state graphics of units, from States.txt alone. While a state holds, its overlay1-4 loop
// (Overlay.txt Filename/Frames/AnimRate/Trans/PreDraw; PreDraw = drawn behind the unit); on gain its
// castoverlay plays once, on loss its removerlay plays once. Aura rings are overlay1 of the aura states.
// Each unit (the hero, and each monster as 'type:id') keeps its own gain/loss edges and clock.
// Limits (unverified): Overlay.txt Dir/NumDirections/LoopWaitTime/offsets are not applied.
import { table, overlayStep } from './skill-fx.mjs';
export function createStateFx(read) {
  const overlays = new Map(table(read, 'data\\global\\excel\\Overlay.txt').map((o) => [o.overlay, o]));
  const states = table(read, 'data\\global\\excel\\states.txt').filter((s) => s.id !== '' && ['overlay1', 'overlay2', 'overlay3', 'overlay4', 'castoverlay', 'removerlay'].some((k) => overlays.get(s[k])?.Filename && overlays.get(s[k]).Filename !== 'null'));
  const fx = (name, extra) => { const o = overlays.get(name); return o && o.Filename && o.Filename !== 'null' ? { kind: 'overlay', file: o.Filename, frames: Number(o.Frames) || 1, rate: overlayStep(o), trans: Number(o.Trans) || 0, predraw: o.PreDraw === '1', ...extra } : null; };
  const units = new Map();   // unit key -> { had, once }
  let heroTick = 0;
  return {
    /** has(stateId) -> bool for the unit `key`; t = engine tick (the hero's own counter when omitted). Returns {kind:'overlay',file,frames,trans,predraw,age} for each live state graphic. */
    update(has, key = 'hero', t, seq) {
      if (t === undefined) t = ++heroTick;
      let u = units.get(key);
      if (!u) units.set(key, u = { had: new Set(), once: [], seq: new Map(), t0: new Map() });
      const out = [];
      for (const s of states) {
        const id = Number(s.id), on = !!has(id);
        if (on !== u.had.has(id)) { (on ? u.had.add(id) : (u.had.delete(id), u.t0.delete(id))); const e = fx(on ? s.castoverlay : s.removerlay, { start: t }); if (e) u.once.push(e); }
        // a re-set (the engine's set counter moved while the state held) makes retail create a fresh loop node at pos 0
        const q = seq ? seq(id) : 0; if (on && u.seq.has(id) && u.seq.get(id) !== q) u.t0.set(id, t); u.seq.set(id, q);
        if (on) for (const k of ['overlay1', 'overlay2', 'overlay3', 'overlay4']) { const e = fx(s[k]); if (e) out.push({ ...e, age: Math.floor((t - (u.t0.get(id) ?? 0)) * e.rate / 256) % e.frames }); }
      }
      u.once = u.once.filter((e) => (t - e.start) * e.rate / 256 < e.frames);
      return out.concat(u.once.map((e) => ({ ...e, age: Math.floor((t - e.start) * e.rate / 256) })));
    },
    /** Forget every monster not in `keys` (one that left the view restarts its states when it returns). */
    retain(keys) { for (const k of units.keys()) if (k !== 'hero' && !keys.has(k)) units.delete(k); },
  };
}
