// D18: the hero's state graphics, from States.txt alone. While a state holds, its overlay1-4 loop
// (Overlay.txt Filename/Frames/AnimRate/Trans/PreDraw; PreDraw = drawn behind the unit); on gain its
// castoverlay plays once, on loss its removerlay plays once. Aura rings are overlay1 of the aura states.
// Limits (unverified): Overlay.txt Dir/NumDirections/LoopWaitTime/offsets are not applied.
import { table } from './skill-fx.mjs';
export function createStateFx(read) {
  const overlays = new Map(table(read, 'data\\global\\excel\\Overlay.txt').map((o) => [o.overlay, o]));
  const states = table(read, 'data\\global\\excel\\states.txt').filter((s) => s.id !== '' && ['overlay1', 'overlay2', 'overlay3', 'overlay4', 'castoverlay', 'removerlay'].some((k) => overlays.get(s[k])?.Filename && overlays.get(s[k]).Filename !== 'null'));
  const fx = (name, extra) => { const o = overlays.get(name); return o && o.Filename && o.Filename !== 'null' ? { kind: 'overlay', file: o.Filename, frames: Number(o.Frames) || 1, rate: Number(o.AnimRate) || 256, trans: Number(o.Trans) || 0, predraw: o.PreDraw === '1', ...extra } : null; };
  const had = new Set(); let tick = 0, once = [];
  return {
    /** Per engine tick; has(stateId) -> bool. Returns {kind:'overlay',file,frames,trans,predraw,age} for each live state graphic. */
    update(has) {
      tick++;
      const out = [];
      for (const s of states) {
        const id = Number(s.id), on = !!has(id);
        if (on !== had.has(id)) { (on ? had.add(id) : had.delete(id)); const e = fx(on ? s.castoverlay : s.removerlay, { start: tick }); if (e) once.push(e); }
        if (on) for (const k of ['overlay1', 'overlay2', 'overlay3', 'overlay4']) { const e = fx(s[k]); if (e) out.push({ ...e, age: Math.floor(tick * e.rate / 256) % e.frames }); }
      }
      once = once.filter((e) => (tick - e.start) * e.rate / 256 < e.frames);
      return out.concat(once.map((e) => ({ ...e, age: Math.floor((tick - e.start) * e.rate / 256) })));
    },
  };
}
