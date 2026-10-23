// Area prefetch (s-perf-streaming): the first visit to an area stalled the main thread on 2 MB range
// fetches of its DT1s (streamfs fetchChunkSync spins). The DT1 list of a neighbouring level is known
// from Levels.txt (Vis0-7) + LvlTypes.txt long before the player walks there, and where each file sits
// in the archive is in the MPQ hash/block table, so the chunks covering them are fetched in the
// background into streamfs's in-memory cache. Pure latency hiding: no byte the engine reads changes.
import { openMpqIndex } from './mpq-index.mjs';
import { resolveLevelMusic } from './act-music.mjs';

const MPQS = ['patch_d2', 'd2exp', 'd2data', 'd2xmusic', 'd2music'];   // highest layer first
export function createAreaPrefetch({ read, StreamFS }) {
  let tables = null, txt = null;                // { levels: Map id -> {type, vis[]}, types: Map id -> files[] }
  let indexes = null;               // [{ path, find }]
  const done = new Set();
  const sheet = (name) => { const rows = new TextDecoder().decode(read(name)).split(/\r?\n/).map((r) => r.split('\t')); const h = rows.shift(); return { col: (n) => h.indexOf(n), rows }; };
  function load() {
    const L = sheet('data\\global\\excel\\Levels.txt'), T = sheet('data\\global\\excel\\LvlTypes.txt');
    const iId = L.col('Id'), iType = L.col('LevelType'), iVis = Array.from({ length: 8 }, (_, i) => L.col('Vis' + i));
    const levels = new Map(), types = new Map();
    for (const r of L.rows) if (r[iId] !== undefined && r[iId] !== '') levels.set(+r[iId], { type: +r[iType], vis: iVis.map((i) => +r[i]).filter((v) => v > 0) });
    const f0 = T.col('File 1'), tId = T.col('Id');
    for (const r of T.rows) if (r[tId] !== undefined && r[tId] !== '') types.set(+r[tId], r.slice(f0, f0 + 32).filter((f) => f && f !== '0'));
    txt = ['Levels', 'SoundEnviron', 'Sounds'].map((n) => new TextDecoder('latin1').decode(read('data\\global\\excel\\' + n + '.txt')));
    const M = sheet('data\\global\\excel\\monstats.txt'), mI = M.col('Id'), mC = M.col('Code'), code = new Map();
    for (const r of M.rows) if (r[mI]) code.set(r[mI], r[mC]);
    const mons = L.col('mon1');
    for (const r of L.rows) if (r[iId] !== undefined && r[iId] !== '') levels.get(+r[iId]).mons = [...new Set(r.slice(mons, mons + 10).map((m) => code.get(m)).filter(Boolean))];
    tables = { levels, types };
    indexes = MPQS.map((n) => { const path = '/retail/' + n + '.mpq'; try { return { path, find: openMpqIndex((p, l) => StreamFS.readAt(path, p, l)) }; } catch { return null; } }).filter((x) => x && x.find);
  }
  function warmLevel(id) {
    if (done.has(id)) return; done.add(id);
    const lv = tables.levels.get(id); if (!lv) return;
    const warmFile = (name) => { for (const ix of indexes) { const hit = ix.find(name); if (hit) { StreamFS.warm(ix.path, hit.pos, hit.size); return; } } };
    for (const f of tables.types.get(lv.type) || []) warmFile('data\\global\\tiles\\' + f.replace(/\//g, '\\'));
    // A monster's DCCs sit together in the archive, so one anchor file's 2 MB chunk covers its siblings.
    for (const c of lv.mons || []) for (const w of ['HTH', '1HS']) warmFile('data\\global\\monsters\\' + c + '\\tr\\' + c + 'trlitnu' + w + '.dcc');
    // The area's music is read whole and synchronously on arrival (act-music.mjs prepare): 6 MB of d2music.
    try { const m = resolveLevelMusic(id, ...txt); if (m) warmFile('data\\global\\music\\' + m.path); } catch { /* no music row: nothing to warm */ }
  }
  // Called often with the current level and the destinations of its warps; does work once per level.
  return function tick(level, warpDests = []) {
    try {
      if (!tables) load();
      const lv = tables.levels.get(level); if (!lv) return;
      for (const d of [...lv.vis, ...warpDests]) warmLevel(d);
    } catch (e) { console.error('areaPrefetch', String(e)); tables = tables || { levels: new Map(), types: new Map() }; indexes = indexes || []; }
  };
}
