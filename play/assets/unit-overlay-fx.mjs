import { overlayStep } from './skill-fx.mjs';
// Unit hit overlays: the Overlay.txt effect a server set on a unit (UNITS_SetOverlay, stat 0xB2, e.g.
// frozenarmor_hit on a struck monster). The id is the Overlay.txt data-row index (0-based, blank rows kept),
// played once at AnimRate << 4 (overlayStep) from the tick the unit first carries it: frame = floor((tick - start) * step / 256),
// alive while frame < Frames. A later id on the same unit restarts it; the same id does not (the engine exports
// one id per unit, not a hit count). Limits: Dir/NumDirections/LoopWaitTime/offsets are not applied.
export function createUnitOverlayFx(read) {
  const raw = new TextDecoder('latin1').decode(read('data\\global\\excel\\Overlay.txt')).split(/\r?\n/).map((l) => l.split('\t'));
  const head = raw[0], rows = raw.slice(1).map((c) => Object.fromEntries(head.map((k, i) => [k, c[i]])));
  const state = new Map();   // `${type}:${id}` -> { overlay, start }
  return {
    /** hits: [{ type, id, overlayId }] for every unit carrying an overlay this tick. Returns one effect per live hit. */
    update(tick, hits) {
      const out = [], live = new Set();
      for (const h of hits) {
        const key = `${h.type}:${h.id}`, row = rows[h.overlayId], file = row?.Filename;
        if (!row || !file || file === 'null') continue;
        live.add(key);
        let s = state.get(key);
        if (!s || s.overlay !== h.overlayId) { s = { overlay: h.overlayId, start: tick }; state.set(key, s); }
        const rate = overlayStep(row), frames = Number(row.Frames) || 1, age = Math.floor((tick - s.start) * rate / 256);
        if (age < frames) out.push({ type: h.type, id: h.id, overlay: h.overlayId, kind: 'overlay', file, frames, rate, trans: Number(row.Trans) || 0, predraw: row.PreDraw === '1', age });
      }
      for (const key of state.keys()) if (!live.has(key)) state.delete(key);
      return out;
    },
  };
}
