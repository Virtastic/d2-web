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
  // harness record of retail's OV node format: the Overlay.txt data-row index (blank rows kept), step, total
  const rowOf = new Map(new TextDecoder('latin1').decode(read('data\\global\\excel\\Overlay.txt')).split(/\r?\n/).slice(1).map((l, i) => [l.split('\t')[0], i]));
  const node = (type, name, live, pos) => `${type},${rowOf.get(name).toString(16)},${live.toString(16)},${pos.toString(16)},${overlayStep(overlays.get(name)).toString(16)},${((Number(overlays.get(name).Frames) || 1) << 8).toString(16)}`;
  const rowsOfId = new Map();
  states.forEach((s, i) => { const id = Number(s.id); (rowsOfId.get(id) || rowsOfId.set(id, []).get(id)).push(i); });
  const units = new Map();   // unit key -> { had, once }
  let heroTick = 0;
  // 2A590 (1.13c D2Client+0x2A590): a monster's hit class high nibble picks the Overlay.txt row its 6B300 node plays once
  const HIT_ROW = { 0x10: 0x36, 0x20: 0x51, 0x30: 0x24, 0x40: 0x01, 0x70: 0x93, 0x80: 0x15, 0x90: 0x71 };
  const rowName = (r) => new TextDecoder('latin1').decode(read('data\\global\\excel\\Overlay.txt')).split(/\r?\n/)[r + 1]?.split('\t')[0];
  return {
    /** Harness only: an array that update() appends retail-format OV node records to (null in play). */
    ov: null,
    /** A 0x0C hit (mode 0x13) on the unit `key` at tick t with hit class `cls`: gate = high nibble in HIT_ROW and low nibble non-zero. Its node plays once from t+1. */
    hit(key, t, cls) {
      const row = HIT_ROW[cls & 0xf0], name = row === undefined || !(cls & 0xf) ? null : rowName(row);
      if (!name || !fx(name)) return;
      let u = units.get(key);
      if (!u) units.set(key, u = { had: new Set(), once: [], seq: new Map(), t0: new Map() });
      if (u.once.some((e) => e.name === name)) return;   // the same row already plays on this unit (a state gain with that castoverlay made it): one node
      u.once.push(fx(name, { start: t, name }));
    },
    /** has(stateId) -> bool for the unit `key`; t = engine tick (the hero's own counter when omitted). Returns {kind:'overlay',file,frames,trans,predraw,age} for each live state graphic. */
    update(has, key = 'hero', t, seq, active) {
      if (t === undefined) t = ++heroTick;
      let u = units.get(key);
      if (!u) units.set(key, u = { had: new Set(), once: [], seq: new Map(), t0: new Map() });
      const out = [], tag = `${t} OV ${key === 'hero' ? '0:1' : key}`;
      // `active` (the ids the unit holds now, when the caller has them) narrows the walk to the states that can change anything:
      // one held now or one held last tick. Every other row has on === had and draws nothing, in States.txt order either way.
      let list = states;
      if (active) {
        const hit = new Set(), add = (id) => { for (const i of rowsOfId.get(id) || []) hit.add(i); };
        for (const id of active) add(id);
        for (const id of u.had) add(id);
        list = [...hit].sort((a, b) => a - b).map((i) => states[i]);
      }
      for (const s of list) {
        const id = Number(s.id), on = !!has(id);
        if (on !== u.had.has(id)) { (on ? (u.had.add(id), u.t0.set(id, t)) : (u.had.delete(id), u.t0.delete(id))); const name = on ? s.castoverlay : s.removerlay, e = fx(name, { start: t, name }); if (e) { u.once.push(e); if (on) this.ov?.push(`${tag} ${node(9, name, 0, 0)}`); } }
        // a re-set (the engine's set counter moved while the state held) makes retail create a fresh loop node at pos 0
        // The set counter only matters while the state holds (a gain stamps t0 itself), so read it only then: asking the engine about every state of every monster was ~180 wasm calls a monster a tick.
        if (on) { const q = seq ? seq(id) : 0; if (u.seq.has(id) && u.seq.get(id) !== q) u.t0.set(id, t); u.seq.set(id, q); }
        if (on) for (const k of ['overlay1', 'overlay2', 'overlay3', 'overlay4']) { const e = fx(s[k]); if (e) { const n = t - (u.t0.get(id) ?? 0); out.push({ ...e, age: (Math.floor(n * e.rate / 256) + 1) % e.frames }); this.ov?.push(`${tag} ${node(8, s[k], n % 51, (n * e.rate) % (e.frames << 8))}`); } }
      }
      for (const e of u.once) if (t > e.start) this.ov?.push(`${tag} ${(t - e.start - 1) * e.rate / 256 < e.frames ? node(2, e.name, 1, (t - e.start - 1) * e.rate) : node(2, e.name, 0, e.frames * 256)}`);   // the last record is the node's end (live 0, pos = frames)
      u.once = u.once.filter((e) => (t - e.start - 1) * e.rate / 256 < e.frames);
      return out.concat(u.once.filter((e) => t > e.start && Math.floor((t - e.start - 1) * e.rate / 256) + 1 < e.frames).map((e) => ({ ...e, age: Math.floor((t - e.start - 1) * e.rate / 256) + 1 })));   // retail's first frame is the tick after the create (OV pos 0)
    },
    /** Forget every monster not in `keys` (one that left the view restarts its states when it returns). */
    retain(keys) { for (const k of units.keys()) if (k !== 'hero' && !keys.has(k)) units.delete(k); },
  };
}
