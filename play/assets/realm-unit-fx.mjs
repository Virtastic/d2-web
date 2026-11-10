import { overlayStep } from './skill-fx.mjs';
// Unit effects of a realm game. The engine's skill start and hit rules queue what a unit plays: a
// sound row due some ticks later, or a one-shot Overlay.txt row on the unit. This module only times
// them against the engine's tick and hands the overlays to the frame's entities; it decides nothing.

/** Engine records are {version, tick, kind, unitType, unitGuid, row, delay, skill}, eight u32s. Kind 2 is an overlay; 1, 3 and 4 are sounds (skill start, weapon swing, class start). */
export function parseRealmUnitFx(bytes, count) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isInteger(count) || count < 0 || view.byteLength < count * 32) throw new RangeError('truncated unit effect records');
  return Array.from({ length: count }, (_, i) => {
    const w = (n) => view.getUint32(i * 32 + n * 4, true);
    const event = { version: w(0), tick: w(1), kind: w(2), unitType: w(3), unitGuid: w(4), row: w(5), delay: w(6), skill: w(7) };
    if (event.version !== 1 || event.kind < 1 || event.kind > 4) throw new Error(`invalid unit effect record ${i}`);
    return event;
  });
}

/**
 *   read : the asset reader, for Overlay.txt (rows by data-row index, blank rows kept)
 *   play : (soundRow, event) => void
 */
export function createRealmUnitFx(read, play) {
  const raw = new TextDecoder('latin1').decode(read('data\\global\\excel\\Overlay.txt')).split(/\r?\n/).map((l) => l.split('\t'));
  const head = raw[0], rows = raw.slice(1).map((c) => Object.fromEntries(head.map((k, i) => [k, c[i]])));
  const sounds = [], overlays = new Map(); // overlays: 'type:guid' to the list of { row, start, ... } still running
  let now = 0;
  const age = (o) => Math.floor((now - o.start) * o.rate / 256) + 1;
  const of = (type, guid) => (overlays.get(`${type}:${guid}`) ?? []).filter((o) => now >= o.start && age(o) < o.frames)
    .map((o) => ({ kind: 'overlay', overlay: o.row, file: o.file, frames: o.frames, rate: o.rate, trans: o.trans, predraw: o.predraw, age: age(o) }));
  return {
    /** Once per ticked frame: `events` just taken from the engine, `tick` the engine's tick. `mute(event)` drops a sound. */
    step(events, tick, mute) {
      now = tick;
      for (const e of events ?? []) {
        if (e.kind !== 2) { if (!mute?.(e)) sounds.push(e); continue; }
        const row = rows[e.row], file = row?.Filename;
        if (!row || !file || file === 'null') continue;
        const key = `${e.unitType}:${e.unitGuid}`;
        overlays.set(key, [...(overlays.get(key) ?? []), { row: e.row, start: e.tick, file, frames: Number(row.Frames) || 1, rate: overlayStep(row), trans: Number(row.Trans) || 0, predraw: row.PreDraw === '1' }]);
      }
      for (let i = 0; i < sounds.length;) {
        const e = sounds[i];
        if (e.tick + e.delay > now) { i++; continue; }
        sounds.splice(i, 1);
        play(e.row, { unitType: e.unitType, unitId: e.unitGuid, kind: 'skillstart', skill: e.skill, baseRow: e.row });
      }
      for (const [key, list] of overlays) {
        const live = list.filter((o) => age(o) < o.frames);
        if (live.length) overlays.set(key, live); else overlays.delete(key);
      }
    },
    /** Puts each unit's running overlays on its entity of the frame; returns the selected player's own (he is not an entity). */
    apply(snapshot, heroGuid) {
      for (const e of snapshot?.entities ?? []) {
        if (e.type !== 0 && e.type !== 1) continue;
        const fx = of(e.type, e.id);
        if (fx.length) e.stateOverlays = [...(e.stateOverlays ?? []), ...fx];
      }
      return heroGuid === undefined ? [] : of(0, heroGuid);
    },
  };
}
