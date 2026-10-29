// Area prefetch (s-perf-streaming): the first visit to an area stalled the main thread on 2 MB range
// fetches of its DT1s (streamfs fetchChunkSync spins). The DT1 list of a neighbouring level is known
// from Levels.txt (Vis0-7) + LvlTypes.txt long before the player walks there, and where each file sits
// in the archive is in the MPQ hash/block table, so the chunks covering them are fetched in the
// background into streamfs's in-memory cache. Pure latency hiding: no byte the engine reads changes.
import { mpqTables, openMpqIndex } from './mpq-index.mjs';
import { resolveLevelMusic } from './act-music.mjs';

// Highest layer first (index.html RETAIL_ORDER). d2char holds the front end's hero art: without its index the
// character-screen warm found nothing there and the select build spun 44 sync misses (271 ms).
const MPQS = ['patch_d2', 'd2exp', 'd2data', 'd2char', 'd2xmusic', 'd2music', 'd2sfx', 'd2speech'];
export function createAreaPrefetch({ read, StreamFS, preload = null }) {
  const pending = [];               // DT1 paths warmed on an earlier tick, decoded one per tick via preload (D49)
  let tables = null, txt = null;                // { levels: Map id -> {type, vis[]}, types: Map id -> files[] }
  let indexes = null, opening = null;   // [{ path, find }] once archives() has built them
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
    archives();
  }
  // The archives' hash/block tables, fetched in the background first and then indexed. They sit at each archive's
  // end, where the engine had not read: indexed synchronously they were 13 sync misses (368 ms) on the title screen.
  function archives() {
    return opening ??= Promise.all(MPQS.map(async (n) => {
      const path = '/retail/' + n + '.mpq', readAt = (p, l) => StreamFS.readAt(path, p, l);
      try {
        await StreamFS.fetched(path, 0, 32);
        const t = mpqTables(readAt(0, 32)); if (!t) return null;
        await Promise.all(t.map(([pos, k]) => StreamFS.fetched(path, pos, k * 16)));
        const find = openMpqIndex(readAt);
        return find && { path, find };
      } catch { return null; }   // not mounted
    })).then((ixs) => (indexes = ixs.filter(Boolean)));
  }
  function warmLevel(id) {
    if (done.has(id)) return; done.add(id);
    const lv = tables.levels.get(id); if (!lv) return;
    const warmFile = (name) => { for (const ix of indexes) { const hit = ix.find(name); if (hit) { StreamFS.warm(ix.path, hit.pos, hit.size); return; } } };
    for (const f of tables.types.get(lv.type) || []) { const n = 'data\\global\\tiles\\' + f.replace(/\//g, '\\'); warmFile(n); pending.push(n); }
    // A monster's DCCs sit together in the archive, so one anchor file's 2 MB chunk covers its siblings.
    for (const c of lv.mons || []) for (const w of ['HTH', '1HS']) warmFile('data\\global\\monsters\\' + c + '\\tr\\' + c + 'trlitnu' + w + '.dcc');
    // The area's music is read whole and synchronously on arrival (act-music.mjs prepare): 6 MB of d2music.
    try { const m = resolveLevelMusic(id, ...txt); if (m) warmFile('data\\global\\music\\' + m.path); } catch { /* no music row: nothing to warm */ }
  }
  // A sound's WAV is read whole and synchronously on its first play. This resolves once the first of `names`
  // found in the archives is in streamfs's cache (or could not be fetched), so that read does not freeze the frame.
  tick.fetched = (names) => archives().then((ixs) => {
    for (const n of names) for (const ix of ixs) { const hit = ix.find(n); if (hit) return StreamFS.fetched(ix.path, hit.pos, hit.size); }
  }).catch(() => { /* the sync read reports it */ });
  // The game's first levels, warmed from the character screens: the tick only runs once the game does, by when
  // bootstrapSession and the first frame had already read the town whole (11 + 13 sync misses, 398 + 314 ms).
  const SHEETS = ['Levels', 'LvlTypes', 'SoundEnviron', 'Sounds', 'monstats'].map((n) => 'data\\global\\excel\\' + n + '.txt');
  tick.warm = (levels) => archives().then(() => Promise.all(SHEETS.map((n) => tick.fetched([n])))).then(() => {
    if (!tables) load();
    for (const l of levels) for (const d of [l, ...(tables.levels.get(l)?.vis || [])]) warmLevel(d);
  }).catch((e) => console.error('areaPrefetch', String(e)));
  // Called often with the current level and the destinations of its warps; does work once per level.
  return tick;
  function tick(level, warpDests = []) {
    try {
      if (!tables) load();
      if (!indexes) return;   // still arriving (archives()); a later call warms
      const lv = tables.levels.get(level); if (!lv) return;
      const waiting = pending.length;
      // The current level first: a waypoint or town-portal arrival lands in a level no neighbour warmed, so
      // its monsters and music were still sync misses (its DT1s are already loaded; preload is cached).
      for (const d of [level, ...lv.vis, ...warpDests]) warmLevel(d);
      // Only a file warmed on an earlier tick: its chunk has had a second to arrive, so the read below is not a sync fetch.
      if (preload && waiting) preload(pending.splice(0, 1)[0]);
    } catch (e) { console.error('areaPrefetch', String(e)); tables = tables || { levels: new Map(), types: new Map() }; indexes = indexes || []; }
  }
}
