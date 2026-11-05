// Entry warm: the files the first frames after a game join read for the units on screen (the hero's
// composite, the town's NPCs, its objects) are fetched into streamfs's cache while the load screen is
// still up. Cold, the first frame read them synchronously: 21 range misses of 2 MB each, ~300 ms of
// main-thread spin on localhost and seconds on a phone's network, spread by the live-frame sprite budget
// over the first frames as a stutter. Cache only: the bytes every later read returns are the same.
//
// The paths are the renderers' own: the same loaders run against a probe reader that returns a file only
// once its chunks are fetched and otherwise notes the path and throws. Each round fetches what the last
// one noted, until a round notes nothing (a loader that reads its layers in order gets one further per round).
import { loadMonsterAnimation } from './monster-renderer.mjs';
import { loadPlayerAnimation, drawnPlayerState } from './unit-renderer.mjs';
import { createPlayerCompositeResolver } from './player-composite.mjs';
import { decodeCof } from './unit-formats.mjs';

const BASE = Object.freeze({ 0: 'LIT', 1: 'LIT', 2: 'LIT', 3: 'LIT', 4: 'LIT', 7: 'LIT', 8: 'LIT', 9: 'LIT' });   // unit-renderer.mjs START_BASE
const COMPONENT = ['HD', 'TR', 'LG', 'RA', 'LA', 'RH', 'LH', 'SH', 'S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8'];
const OBJECT_MODE = ['NU', 'OP', 'ON', 'S1', 'S2', 'S3', 'S4', 'S5'];   // object-renderer.mjs MODE

// read(path) -> bytes (synchronous); fetched(path) -> Promise once the file's chunks are cached.
// hero: a player render state; monsters: monster animation actors; objects: snapshot objects.
export async function warmEntryFiles({ read, fetched, hero = null, monsters = [], objects = [], rounds = 24 }) {
  const ready = new Set(), seen = new Set();
  let need = [], blocked = false;
  const MISSING = {};
  const probe = (path) => {
    if (ready.has(path)) return read(path);
    if (!seen.has(path)) { seen.add(path); need.push(path); }
    blocked = true;
    throw MISSING;
  };
  const composite = createPlayerCompositeResolver(read);
  const jobs = [];
  if (hero) jobs.push(() => {
    const state = drawnPlayerState(hero);
    const composition = composite.resolve(state.items, BASE, state.weaponClass, state.primaryWeaponId);
    loadPlayerAnimation(probe, state.playerClass, state.mode, { direction64: state.direction ?? 0, composition });
  });
  for (const actor of monsters) jobs.push(() => loadMonsterAnimation(probe, actor));
  for (const object of objects) if (object?.token) jobs.push(() => {
    const mode = OBJECT_MODE[object.mode] || 'NU';
    const cof = decodeCof(probe(`data\\global\\OBJECTS\\${object.token}\\COF\\${object.token}${mode}HTH.COF`));
    for (const layer of cof.layerRecords) if (object.componentMask & (1 << layer.component)) {
      const component = COMPONENT[layer.component]; if (!component) continue;
      try { probe(`data\\global\\OBJECTS\\${object.token}\\${component}\\${object.token}${component}LIT${mode}${layer.weaponClass.toUpperCase()}.DCC`); } catch {}
    }
  });
  let files = 0;
  for (let r = 0; r < rounds; r++) {
    let open = 0;
    for (let j = 0; j < jobs.length; j++) {
      if (!jobs[j]) continue;
      blocked = false;
      try { jobs[j](); } catch { /* MISSING, or a unit the renderer cannot draw either */ }
      if (blocked) open++; else jobs[j] = null;   // read nothing uncached: its files are all fetched
    }
    if (!open) break;
    const batch = need; need = [];
    await Promise.all(batch.map((p) => Promise.resolve(fetched(p)).catch(() => {})));
    for (const p of batch) ready.add(p);
    files += batch.length;
  }
  return files;
}
