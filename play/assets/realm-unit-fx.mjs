import { overlayStep } from './skill-fx.mjs';
// Unit effects of a realm game. The engine's skill start and hit rules and the server's overlay,
// hit and unit sound records queue what a unit plays on ONE channel: a sound row due some ticks
// later, a one-shot Overlay.txt row on the unit, the mark of a hit or a unit sound event. This
// module only times them against the engine's tick and hands them to the frame's entities and the
// sound players; it decides nothing.

/** Engine records are {version, tick, kind, unitType, unitGuid, row, delay, skill}, eight u32s. Kind 2 is an overlay; 1, 3 and 4 are sounds (skill start or a record's own sound, weapon swing, class start); 5 is a hit (row: the hit class); 6 is a unit sound event (row: the event, skill: the unit's class). */
export function parseRealmUnitFx(bytes, count) {
  const view = bytes instanceof DataView ? bytes : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isInteger(count) || count < 0 || view.byteLength < count * 32) throw new RangeError('truncated unit effect records');
  return Array.from({ length: count }, (_, i) => {
    const w = (n) => view.getUint32(i * 32 + n * 4, true);
    const event = { version: w(0), tick: w(1), kind: w(2), unitType: w(3), unitGuid: w(4), row: w(5), delay: w(6), skill: w(7) };
    if (event.version !== 1 || event.kind < 1 || event.kind > 6) throw new Error(`invalid unit effect record ${i}`);
    return event;
  });
}

/**
 *   read : the asset reader, for Overlay.txt (rows by data-row index, blank rows kept)
 *   play : (soundRow, event) => void
 *   unitSound : ({ unitType, unitGuid, classId, soundEvent }) => void, a unit sound event the server named
 */
/** The Overlay.txt row the client starts on a unit the server marks (0x8A). */
export const MARK_ROW = 0x48;

export function createRealmUnitFx(read, play, unitSound) {
  const raw = new TextDecoder('latin1').decode(read('data\\global\\excel\\Overlay.txt')).split(/\r?\n/).map((l) => l.split('\t'));
  const head = raw[0], rows = raw.slice(1).map((c) => Object.fromEntries(head.map((k, i) => [k, c[i]])));
  const sounds = [], overlays = new Map(); // overlays: 'type:guid' to the list of { row, start, ... } still running
  const marked = new Map(); // 'type:guid' to the tick the unit's speech mark was first seen
  const markRow = rows[MARK_ROW], markFile = markRow?.Filename && markRow.Filename !== 'null' ? markRow.Filename : null;
  // The mark loops until the unit loses it: the frame wraps where a one-shot overlay ends.
  const markOf = (type, guid) => {
    const start = marked.get(`${type}:${guid}`);
    if (start === undefined || !markFile) return [];
    const frames = Number(markRow.Frames) || 1, rate = overlayStep(markRow);
    return [{ kind: 'overlay', overlay: MARK_ROW, file: markFile, frames, rate, trans: Number(markRow.Trans) || 0, predraw: markRow.PreDraw === '1', age: (Math.floor((now - start) * rate / 256) + 1) % frames }];
  };
  let now = 0, hits = new Map(); // hits: 'type:guid' to the hit class of a hit taken in the last step
  const age = (o) => Math.floor((now - o.start) * o.rate / 256) + 1;
  const of = (type, guid) => (overlays.get(`${type}:${guid}`) ?? []).filter((o) => now >= o.start && age(o) < o.frames)
    .map((o) => ({ kind: 'overlay', overlay: o.row, file: o.file, frames: o.frames, rate: o.rate, trans: o.trans, predraw: o.predraw, age: age(o) }));
  return {
    /** Once per ticked frame: `events` just taken from the engine, `tick` the engine's tick. `mute(event)` drops a sound. */
    step(events, tick, mute) {
      now = tick;
      hits = new Map();
      for (const e of events ?? []) {
        if (e.kind === 5) { hits.set(`${e.unitType}:${e.unitGuid}`, e.row); continue; }
        if (e.kind === 6) { unitSound?.({ unitType: e.unitType, unitGuid: e.unitGuid, classId: e.skill, soundEvent: e.row }); continue; }
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
    /** The hit class of a hit the unit took in the last step; undefined when it took none. */
    hitOf: (type, guid) => hits.get(`${type}:${guid}`),
    /**
     * Puts each unit's running overlays on its entity of the frame; returns the selected player's own (he is not an entity).
     * `marks` ([{ type, guid }], d2gsClientUnitMarks) are the units other than monsters carrying the speech mark.
     */
    apply(snapshot, heroGuid, marks = []) {
      const live = new Set(marks.map((m) => `${m.type}:${m.guid}`));
      for (const key of live) if (!marked.has(key)) marked.set(key, now);
      for (const key of [...marked.keys()]) if (!live.has(key)) marked.delete(key);
      for (const e of snapshot?.entities ?? []) {
        if (e.type !== 0 && e.type !== 1) continue;
        const fx = [...of(e.type, e.id), ...markOf(e.type, e.id)];
        if (fx.length) e.stateOverlays = [...(e.stateOverlays ?? []), ...fx];
      }
      return heroGuid === undefined ? [] : [...of(0, heroGuid), ...markOf(0, heroGuid)];
    },
  };
}
