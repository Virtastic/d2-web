// The scripted local game (SCRIPT), joined through the realm client path and read from the client state the
// packets build, compared field by field with a pinned reference: the same script read from the engine directly,
// recorded once from the last tree that still had that path (play/assets/one-path-baseline.json; its `commit`
// names the tree). The table printed is `field: differing/total`; `total` counts the records either side holds,
// `differing` the records only one of them holds. No field is masked, dropped or given a tolerance.
// The control plays the realm path again and compares it with the first run; it must read 0 on every field,
// or the run is not deterministic and the table means nothing.
//
// Re-recording the reference (only if SCRIPT changes): build that commit and run its
// play/assets/one-path-parity.test.mjs with D2_ONE_PATH_DUMP=<file>; the `direct` array of the dump is the baseline.
//
// The comparison itself is tested here with inputs that must differ. The live run needs a staged build
// (build-wasm/site), a Chrome and the game data; without one of them it is skipped and says which.
// D2_ONE_PATH_GATE=1 turns a non-zero field, or a run that never came up, into a failure; without it the run reports the table.
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawn } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
export const FIELDS = ['world', 'player', 'cast', 'life', 'monsters', 'players', 'missiles', 'objects', 'groundItems', 'ownedItems'];

// Objects: the units a client holds (a room with a client) that the server sends; the engine also lists the rooms
// no client is in, and the sound-only presets a client builds from its own level data (ids from 0x70000000), neither of which a packet carries.
// Monsters likewise: a client is told only of the monsters in rooms it holds, so the engine's list is read through the
// server's own room membership (d2_session_unit_client_rooms); the packet side lists what it was told.
// Evaluated in the page: one tick's snapshot as { field: [record, ...] }, each record a string.
export const SNAPSHOT_FIELDS = `(() => {
  const s = window.__d2Shell.snapshot ?? {}, p = s.player ?? {}, id = s.playerId >>> 0;
  const unit = (e) => [e.id >>> 0, e.classId, e.mode, e.x, e.y].join(':');
  const of = (type) => (s.entities ?? []).filter((e) => e.type === type && !(type === 0 && (e.id >>> 0) === id)).map(unit);
  const M = window.Module, held = !window.__d2Shell.fromPackets && M?._d2_session_unit_client_rooms ? new Set() : null;
  if (held) {
    const n = M.ccall('d2_session_unit_count', 'number', [], []), up = Number(M.ccall('d2_session_units', 'number', [], [])) >> 2, flags = M._malloc(n + 1);
    M.ccall('d2_session_unit_client_rooms', 'number', ['bigint', 'number'], [BigInt(flags), n]);
    for (let i = 0; i < n; i++) if (M.HEAPU32[up + i * 11 + 1] === 1 && M.HEAPU8[flags + i]) held.add(M.HEAPU32[up + i * 11]);
    M._free(flags);
  }
  const items = s.items ?? [], ground = (i) => Boolean(i.landed) || i.invPage === 0xff;
  return { world: ['act:' + s.act, 'level:' + s.level, 'difficulty:' + s.difficulty, 'class:' + s.playerClass],
    player: ['x:' + p.x, 'y:' + p.y, 'mode:' + s.mode, 'weapon:' + p.weaponClass],
    cast: ['mode:' + s.mode, 'right:' + window.__d2Shell.selectedSkill],
    life: ['hp:' + p.hp, 'hpMax:' + p.hpMax, 'mana:' + p.mana, 'manaMax:' + p.manaMax],
    monsters: of(1).filter((r) => !held || held.has(Number(r.split(':')[0]))), players: of(0), missiles: of(3),
    objects: (s.objects ?? []).filter((o) => o.inClientRoom !== false && (o.id >>> 0) < 0x70000000).map(unit),
    groundItems: items.filter(ground).map((i) => [i.id >>> 0, i.code ?? i.classId, i.x, i.y].join(':')),
    ownedItems: items.filter((i) => !ground(i)).map((i) => [i.id >>> 0, i.code ?? i.classId, i.invPage, i.bodyLoc, i.gridX, i.gridY].join(':')) };
})()`;

/** One tick, one field: the records only one side holds, over the records either side holds. */
export function diffField(a, b) {
  const left = new Set(a ?? []), right = new Set(b ?? []);
  let differing = 0;
  for (const r of left) if (!right.has(r)) differing++;
  for (const r of right) if (!left.has(r)) differing++;
  return { differing, total: new Set([...left, ...right]).size };
}

// What the server sends on a tick is flushed and dispatched to the client before the client's frame for that tick, as
// single player's loop does, so no field is read later than the engine's own (a lagged field is compared at t + n).
export const PACKET_LAG = {};

/** Two runs (arrays of per-tick field maps) to { field: { differing, total } }. A missing tick differs in full; a field
 *  lagged by n compares the second run's tick t + n, and the second run then holds n frames more. */
export function parityTable(direct, packets, fields = FIELDS, lag = {}) {
  const table = Object.fromEntries(fields.map((f) => [f, { differing: 0, total: 0 }]));
  const extra = Math.max(0, ...fields.map((f) => lag[f] ?? 0));
  for (let t = 0; t < Math.max(direct.length, packets.length - extra); t++) {
    for (const f of fields) {
      const d = diffField(direct[t]?.[f], packets[t + (lag[f] ?? 0)]?.[f]);
      table[f].differing += d.differing; table[f].total += d.total;
    }
  }
  return table;
}

export const formatTable = (table) => Object.entries(table).map(([f, v]) => `${f}: ${v.differing}/${v.total}`).join('\n');

// The script both runs play, on the same map (seed 1; a new character's map is otherwise drawn at random): where the pointer goes on which tick (canvas pixels; button 0 left, 2 right).
export const SCRIPT = { ticks: 170, query: '&class=1&level=2&skills=44:1,54:1',   // a sorceress with Ice Bolt and Teleport, in the Blood Moor (a town refuses Ice Bolt)
  clicks: [{ tick: 5, x: 560, y: 120, button: 0 }, { tick: 40, x: 120, y: 400, button: 0 },
    { tick: 75, x: 470, y: 200, button: 2 }, { tick: 95, x: 300, y: 250, button: 2 },    // right hand Ice Bolt, twice
    { tick: 115, x: 520, y: 380, button: 2 }, { tick: 140, x: 200, y: 300, button: 2 }],   // right hand Teleport, twice
  selects: [{ tick: 70, skill: 44, left: 0 }, { tick: 110, skill: 54, left: 0 }] };

// Run in the page as one synchronous call, so the page's own frame loop cannot tick inside it. That loop does run
// between the game coming up and this call, for as long as the wall clock allows (7 to 12 ticks measured), so the
// script first steps the game to a fixed tick: a monster that changes mode on tick 33 then does so on the same
// script tick in every run. A frame read from packets carries the local game's own tick (the server is in the page),
// re-read here because the page's loop may have ticked the game since it last drew.
export const START_TICK = 100;
export const PLAY_SCRIPT = `async (shell, script) => {
  // A game read from packets takes each one on a promise hop after the connection delivers it: drain those hops (microtasks only, so
  // the page's frame loop still cannot tick inside the script) after each step, or every packet waits until the script has ended.
  shell.refreshView?.();
  const settle = async () => { for (let i = 0; i < 64; i++) await Promise.resolve(); };
  const at = window.__d2?.session?.snapshot?.()?.tick ?? shell.snapshot?.tick, out = [];   // the game's own tick: a frame read from packets counts the client's frames, which start later than the local game it joined
  if (at > ${START_TICK}) throw new Error('the game was at tick ' + at + ' before the script began, past its start tick ${START_TICK}');
  for (let n = ${START_TICK} - at; n > 0; n--) { shell.step(); await settle(); }
  for (let t = 0; t < script.ticks + (shell.fromPackets ? Math.max(0, ...Object.values(${JSON.stringify(PACKET_LAG)})) : 0); t++) {
    // The hand's skill is chosen through the page's own command: the engine at once, or the 0x3C packet whose 0x23 answer the client reads back.
    for (const c of script.selects ?? []) if (c.tick === t) shell.selectSkill?.(c.skill, c.left ? 'left' : 'right');
    for (const c of script.clicks) if (c.tick === t) shell.input.pointer(c.x, c.y, c.button);
    shell.step(); await settle(); shell.refreshView?.();
    out.push(${SNAPSHOT_FIELDS});
  }
  return out; }`;

test('the field comparison counts what only one run holds, and a missing tick in full', () => {
  assert.deepEqual(diffField(['a', 'b'], ['a', 'b']), { differing: 0, total: 2 });
  assert.deepEqual(diffField(['a', 'b'], ['a', 'c']), { differing: 2, total: 3 });
  assert.deepEqual(diffField([], ['a']), { differing: 1, total: 1 });
  assert.deepEqual(diffField([], []), { differing: 0, total: 0 });
  const tick = { world: ['act:1'], monsters: ['1:5:1:10:10'] };
  const same = parityTable([tick, tick], [tick, tick], ['world', 'monsters']);
  assert.deepEqual(same, { world: { differing: 0, total: 2 }, monsters: { differing: 0, total: 2 } });
  // The control: a moved monster, and a run one tick short, must both show.
  const moved = parityTable([tick, tick], [tick, { ...tick, monsters: ['1:5:1:11:10'] }], ['world', 'monsters']);
  assert.deepEqual(moved.monsters, { differing: 2, total: 3 });
  assert.equal(moved.world.differing, 0);
  const short = parityTable([tick, tick], [tick], ['world', 'monsters']);
  assert.deepEqual(short.world, { differing: 1, total: 2 });
  assert.equal(formatTable(moved), 'world: 0/2\nmonsters: 2/3');
  // A lagged field reads the second run one tick on: the same monster a tick late is equal, and the extra frame is not a missing tick.
  const late = parityTable([tick, { ...tick, monsters: ['1:5:1:11:10'] }], [tick, tick, { ...tick, monsters: ['1:5:1:11:10'] }], ['world', 'monsters'], { monsters: 1 });
  assert.deepEqual(late.monsters, { differing: 0, total: 2 });
  assert.equal(late.world.differing, 0);
});

test('the page read names every field of the table', () => {
  for (const f of FIELDS) assert.match(SNAPSHOT_FIELDS, new RegExp(`\\b${f}:`), f);
});

test('the script starts on the same game tick however far the page had run before it', async () => {
  // A stand-in game: one monster whose mode turns on game tick 133, a hero the pointer moves.
  const run = async (ranBefore) => {
    const shell = { tick: ranBefore, x: 0, input: { pointer: (x) => { shell.x = x; } }, step: () => { shell.tick++; },
      get snapshot() { return { tick: shell.tick, playerId: 1, player: { x: shell.x, y: 0 }, entities: [{ id: 4, type: 1, classId: 147, mode: shell.tick < 133 ? 1 : 2, x: 5, y: 5 }] }; } };
    return new Function('window', `return (${PLAY_SCRIPT})(window.__d2Shell, ${JSON.stringify(SCRIPT)})`)({ __d2Shell: shell });
  };
  const first = await run(7);
  assert.equal(first.length, SCRIPT.ticks);
  assert.equal(first.findIndex((t) => t.monsters[0] === '4:147:2:5:5'), 133 - START_TICK - 1);
  for (const ranBefore of [8, 9, 12, START_TICK]) assert.deepEqual(await run(ranBefore), first, `${ranBefore} ticks before the script`);
  await assert.rejects(run(START_TICK + 1), /past its start tick/);
  // A frame with no game tick is stepped the script's ticks and no more.
  const steps = []; await new Function('window', `return (${PLAY_SCRIPT})(window.__d2Shell, ${JSON.stringify(SCRIPT)})`)({ __d2Shell: { snapshot: {}, input: { pointer() {} }, step: () => steps.push(1) } });
  assert.equal(steps.length, SCRIPT.ticks);
  // A frame read from packets: its tick is the game's, current only after the view is re-read (the page's loop
  // ticked the game since the last draw). The script starts on the same tick as the direct run above.
  const packetRun = (ranBefore) => {
    const game = { tick: ranBefore, x: 0 }; let drawn = { tick: ranBefore - 3 };
    const shell = { fromPackets: true, input: { pointer: (x) => { game.x = x; } }, step: () => { game.tick++; },
      refreshView: () => { drawn = { tick: game.tick, playerId: 1, player: { x: game.x, y: 0 }, entities: [{ id: 4, type: 1, classId: 147, mode: game.tick < 133 ? 1 : 2, x: 5, y: 5 }] }; },
      get snapshot() { return drawn; } };
    return new Function('window', `return (${PLAY_SCRIPT})(window.__d2Shell, ${JSON.stringify(SCRIPT)})`)({ __d2Shell: shell });
  };
  // It plays the lag's frames more (PACKET_LAG), for a lagged field to be read on the frame it lands; the frames before them are the direct run's.
  const lag = Math.max(0, ...Object.values(PACKET_LAG));
  for (const ranBefore of [7, 12, 60]) {
    const frames = await packetRun(ranBefore);
    assert.equal(frames.length, SCRIPT.ticks + lag, `packet frame, ${ranBefore} ticks before the script: its length`);
    assert.deepEqual(frames.slice(0, SCRIPT.ticks), first, `packet frame, ${ranBefore} ticks before the script`);
  }
});

// ---- the live run ----
const BASELINE = join(ROOT, 'play', 'assets', 'one-path-baseline.json');
const SITE = process.env.D2_ONE_PATH_SITE || join(ROOT, 'build-wasm', 'site');
const CHROME = process.env.CHROME_BIN || ['/usr/bin/chromium', '/usr/bin/chromium-browser', '/usr/bin/google-chrome'].find((p) => existsSync(p));
const RETAIL = process.env.D2_RETAIL_DIR || [join(ROOT, 'retail', 'D2'), join(ROOT, 'build-wasm', 'retail')].find((d) => existsSync(join(d, 'd2data.mpq')));
function unavailable() {
  if (process.env.D2_ONE_PATH_LIVE === '0') return 'D2_ONE_PATH_LIVE=0';
  // The staged engine sits under e/<version>/ (engine-version.txt names it); an unversioned site has it at the root.
  const version = existsSync(join(SITE, 'engine-version.txt')) ? readFileSync(join(SITE, 'engine-version.txt'), 'utf8').trim() : '';
  const engine = [join(SITE, 'e', version, 'd2.js'), join(SITE, 'd2.js')].find((p) => existsSync(p));
  if (!engine) return `no staged build at ${SITE} (ci/build.sh)`;
  if (!readFileSync(engine, 'utf8').includes('d2_local_d2gs_open')) return 'the staged engine has no in-process game connection (rebuild)';
  if (!existsSync(BASELINE)) return `no pinned reference at ${BASELINE}`;
  if (!CHROME || !existsSync(CHROME)) return 'no Chrome (CHROME_BIN)';
  if (!RETAIL) return 'no game data (D2_RETAIL_DIR)';
  return '';
}
const CONTROL_RUNS = Math.max(1, Number(process.env.D2_ONE_PATH_CONTROL_RUNS) || 2);
const freePort = () => new Promise((done, fail) => { const s = createServer(); s.once('error', fail); s.listen(0, '127.0.0.1', () => { const { port } = s.address(); s.close(() => done(port)); }); });
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

/** A page in a headless Chrome, driven over the DevTools socket. */
async function openPage(url) {
  const profile = mkdtempSync(join(tmpdir(), 'one-path-'));
  // No display is ever opened: headless Chrome still connects to the one it inherits.
  const chrome = spawn(CHROME, ['--headless=new', '--disable-gpu', '--no-sandbox', '--disable-dev-shm-usage', '--no-first-run', '--no-default-browser-check', '--disable-extensions',
    '--disable-renderer-backgrounding', '--disable-background-timer-throttling', '--disable-backgrounding-occluded-windows', '--window-size=800,600',
    '--remote-debugging-port=0', `--user-data-dir=${profile}`, 'about:blank'], { stdio: ['ignore', 'ignore', 'pipe'], env: { ...process.env, DISPLAY: '', WAYLAND_DISPLAY: '' } });
  const endpoint = await new Promise((done, fail) => {
    let err = ''; const timer = setTimeout(() => fail(new Error('Chrome did not open its DevTools socket')), 30000);
    chrome.stderr.on('data', (d) => { err += d; const m = err.match(/DevTools listening on (ws:\/\/\S+)/); if (m) { clearTimeout(timer); done(m[1]); } });
    chrome.once('exit', () => { clearTimeout(timer); fail(new Error('Chrome exited: ' + err.slice(-300))); });
  });
  const ws = new WebSocket(endpoint); await new Promise((done, fail) => { ws.onopen = done; ws.onerror = () => fail(new Error('DevTools socket refused')); });
  let seq = 0; const waiting = new Map();
  ws.onmessage = (e) => { const m = JSON.parse(e.data); const w = waiting.get(m.id); if (w) { waiting.delete(m.id); m.error ? w.fail(new Error(m.error.message)) : w.done(m.result); } };
  const send = (method, params = {}, sessionId) => new Promise((done, fail) => { const id = ++seq; waiting.set(id, { done, fail }); ws.send(JSON.stringify({ id, method, params, sessionId })); });
  const { targetId } = await send('Target.createTarget', { url });
  const { sessionId } = await send('Target.attachToTarget', { targetId, flatten: true });
  const evaluate = async (expression) => {
    const r = await send('Runtime.evaluate', { expression, returnByValue: true, awaitPromise: true }, sessionId);
    if (r.exceptionDetails) throw new Error(r.exceptionDetails.exception?.description || r.exceptionDetails.text);
    return r.result.value;
  };
  const waitFor = async (expression, ms, what) => { const end = Date.now() + ms; while (Date.now() < end) { if (await evaluate(expression).catch(() => false)) return; await sleep(200); } throw new Error(`timed out waiting for ${what}`); };
  const close = () => { try { ws.close(); } catch { /* gone */ } chrome.kill('SIGKILL'); rmSync(profile, { recursive: true, force: true }); };
  return { evaluate, waitFor, close };
}

// What a join that never came up says for itself: the transport's state and error, and how far the world load got.
const JOIN_STATE = `(() => { const s = window.__d2Shell, t = s.realmTransportEvidence ?? {}, w = s.realmWorldState ?? {};
  return JSON.stringify({ state: t.state, error: String(t.error ?? '').slice(0, 300), loadAct: w.hasLoadAct, loadComplete: w.loadCompleteEvent, mapReveal: w.hasMapReveal,
    scene: s.realmSceneEvidence ?? null, connection: s.localRealmEvidence, hpMax: s.snapshot?.player?.hpMax }); })()`;

/** Plays SCRIPT on one page and returns the per-tick field maps. */
async function play(origin, query) {
  const page = await openPage(`${origin}/index.html?retail=1&play=1&seed=1${query}`);
  try {
    // The run is ready when the frame is read from the client state; what that state then lacks is a field of the table, not a wait.
    await page.waitFor(`window.__d2Shell?.fromPackets === true || window.__d2Shell?.realmTransportEvidence?.state === 'failed'`, 180000, 'the game joined through the in-process connection')
      .catch(async (error) => { throw new Error(`${error.message}: ${await page.evaluate(JOIN_STATE).catch(String)}`); });
    if (!await page.evaluate('window.__d2Shell.fromPackets')) throw new Error(`the join through the in-process connection failed: ${await page.evaluate(JOIN_STATE)}`);
    return await page.evaluate(`(${PLAY_SCRIPT})(window.__d2Shell, ${JSON.stringify(SCRIPT)})`);
  } finally { page.close(); }
}

test('the pinned reference is the script it was recorded for, and its fields are the table\'s', { skip: !existsSync(BASELINE) && 'no pinned reference' }, () => {
  const base = JSON.parse(readFileSync(BASELINE, 'utf8'));
  assert.match(base.commit, /^[0-9a-f]{40}$/);
  assert.deepEqual(base.script, SCRIPT);
  assert.equal(base.direct.length, SCRIPT.ticks);
  for (const tick of base.direct) assert.deepEqual(Object.keys(tick), FIELDS);
  // The control for the reference: one record changed in it must show.
  const bent = structuredClone(base.direct); bent[50].world[0] += 'x';
  assert.equal(parityTable(base.direct, bent, ['world']).world.differing, 2);
  assert.equal(parityTable(base.direct, base.direct).world.differing, 0);
});

test('the single-player realm path shows the pinned direct-path frame, field by field', { skip: unavailable() || false, timeout: 600000 + CONTROL_RUNS * 200000 }, async () => {
  const port = await freePort();
  const server = spawn(process.execPath, [join(ROOT, 'play', 'serve.mjs'), String(port), SITE], { stdio: 'ignore', env: { ...process.env, D2_RETAIL_DIR: RETAIL } });
  try {
    const origin = `http://127.0.0.1:${port}`;
    for (let i = 0; i < 50; i++) { if (await fetch(origin + '/index.html').then((r) => r.ok, () => false)) break; await sleep(100); }
    const direct = JSON.parse(readFileSync(BASELINE, 'utf8')).direct;
    const query = SCRIPT.query + (process.env.D2_ONE_PATH_PACKET_QUERY ?? '');
    // A run that never comes up shows nothing: every record of the reference then differs, and the reason is printed.
    let joinFailure = '';
    const packets = await play(origin, query).catch((error) => { joinFailure = error.message; return []; });
    const table = parityTable(direct, packets, FIELDS, PACKET_LAG);
    // The control: the same game played again through the same path. A field that differs between two runs of one path is not a parity field at all.
    // Every repeat is compared with the first run and the counts are summed (D2_ONE_PATH_CONTROL_RUNS repeats, 2 unless set).
    const control = Object.fromEntries(FIELDS.map((f) => [f, { differing: 0, total: 0 }]));
    let again = [];
    if (!joinFailure) for (let i = 0; i < CONTROL_RUNS; i++) {
      again = await play(origin, query);
      for (const [f, v] of Object.entries(parityTable(packets, again))) { control[f].differing += v.differing; control[f].total += v.total; }
    }
    if (process.env.D2_ONE_PATH_DUMP) writeFileSync(process.env.D2_ONE_PATH_DUMP, JSON.stringify({ direct, again, packets }));   // the runs themselves, to read a differing field
    console.log(`one-path parity, ${SCRIPT.ticks} ticks, realm path against the pinned reference (field: differing/total)\n${formatTable(table)}\ncontrol, the realm path against ${CONTROL_RUNS} more runs of itself\n${formatTable(control)}${joinFailure ? `\npacket run: ${joinFailure}` : ''}`);
    for (const f of FIELDS) assert.equal(control[f].differing, 0, `${f} differs between two runs of the same path`);
    assert.ok(table.world.total > 0 && table.player.total > 0, 'the runs recorded nothing');
    if (process.env.D2_ONE_PATH_GATE === '1') assert.equal(joinFailure, '');
    if (process.env.D2_ONE_PATH_GATE === '1') for (const f of FIELDS) assert.equal(table[f].differing, 0, `${f}: ${table[f].differing}/${table[f].total}`);
  } finally { server.kill('SIGKILL'); }
});
