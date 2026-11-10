// The same scripted local game, run twice: read from the engine directly, and joined through the
// in-process game connection and read from the client state the packets build (?localrealm=1). Each
// tick's frame snapshot is compared field by field; the table printed is `field: differing/total`.
// A field is a set of records per tick; `total` counts the records either run holds, `differing` the
// records only one of them holds. Every field at 0 is the condition for making the packet path the
// only one.
//
// The comparison itself is tested here with inputs that must differ. The live run needs a staged
// build (build-wasm/site with the in-process connection), a Chrome and the game data; without one of
// them it is skipped and says which. D2_ONE_PATH_GATE=1 turns a non-zero field, or a packet run that
// never came up, into a failure; without it the run reports the table.
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawn } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { createServer } from 'node:net';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
export const FIELDS = ['world', 'player', 'life', 'monsters', 'players', 'missiles', 'objects', 'groundItems', 'ownedItems'];

// Evaluated in the page: one tick's snapshot as { field: [record, ...] }, each record a string.
export const SNAPSHOT_FIELDS = `(() => {
  const s = window.__d2Shell.snapshot ?? {}, p = s.player ?? {}, id = s.playerId >>> 0;
  const unit = (e) => [e.id >>> 0, e.classId, e.mode, e.x, e.y].join(':');
  const of = (type) => (s.entities ?? []).filter((e) => e.type === type && !(type === 0 && (e.id >>> 0) === id)).map(unit);
  const items = s.items ?? [], ground = (i) => Boolean(i.landed) || i.invPage === 0xff;
  return { world: ['act:' + s.act, 'level:' + s.level, 'difficulty:' + s.difficulty, 'class:' + s.playerClass],
    player: ['x:' + p.x, 'y:' + p.y, 'mode:' + s.mode, 'weapon:' + p.weaponClass],
    life: ['hp:' + p.hp, 'hpMax:' + p.hpMax, 'mana:' + p.mana, 'manaMax:' + p.manaMax],
    monsters: of(1), players: of(0), missiles: of(3),
    objects: (s.objects ?? []).map(unit),
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

/** Two runs (arrays of per-tick field maps) to { field: { differing, total } }. A missing tick differs in full. */
export function parityTable(direct, packets, fields = FIELDS) {
  const table = Object.fromEntries(fields.map((f) => [f, { differing: 0, total: 0 }]));
  for (let t = 0; t < Math.max(direct.length, packets.length); t++) {
    for (const f of fields) {
      const d = diffField(direct[t]?.[f], packets[t]?.[f]);
      table[f].differing += d.differing; table[f].total += d.total;
    }
  }
  return table;
}

export const formatTable = (table) => Object.entries(table).map(([f, v]) => `${f}: ${v.differing}/${v.total}`).join('\n');

// The script both runs play, on the same map (seed 1; a new character's map is otherwise drawn at random): where the pointer goes on which tick (canvas pixels; button 0 left, 2 right).
export const SCRIPT = { ticks: 120, clicks: [{ tick: 5, x: 560, y: 120, button: 0 }, { tick: 40, x: 120, y: 400, button: 0 }, { tick: 80, x: 470, y: 200, button: 2 }] };

// Run in the page as one synchronous call, so the page's own frame loop cannot tick inside it. That loop does run
// between the game coming up and this call, for as long as the wall clock allows (7 to 12 ticks measured), so the
// script first steps the game to a fixed tick: a monster that changes mode on tick 33 then does so on the same
// script tick in every run. A frame read from packets carries no game tick and starts where it is.
export const START_TICK = 100;
export const PLAY_SCRIPT = `(shell, script) => {
  const at = shell.snapshot?.tick, out = [];
  if (at > ${START_TICK}) throw new Error('the game was at tick ' + at + ' before the script began, past its start tick ${START_TICK}');
  for (let n = ${START_TICK} - at; n > 0; n--) shell.step();
  for (let t = 0; t < script.ticks; t++) {
    for (const c of script.clicks) if (c.tick === t) shell.input.pointer(c.x, c.y, c.button);
    shell.step(); shell.refreshView?.();
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
});

test('the page read names every field of the table', () => {
  for (const f of FIELDS) assert.match(SNAPSHOT_FIELDS, new RegExp(`\\b${f}:`), f);
});

test('the script starts on the same game tick however far the page had run before it', () => {
  // A stand-in game: one monster whose mode turns on game tick 133, a hero the pointer moves.
  const run = (ranBefore) => {
    const shell = { tick: ranBefore, x: 0, input: { pointer: (x) => { shell.x = x; } }, step: () => { shell.tick++; },
      get snapshot() { return { tick: shell.tick, playerId: 1, player: { x: shell.x, y: 0 }, entities: [{ id: 4, type: 1, classId: 147, mode: shell.tick < 133 ? 1 : 2, x: 5, y: 5 }] }; } };
    return new Function('window', `return (${PLAY_SCRIPT})(window.__d2Shell, ${JSON.stringify(SCRIPT)})`)({ __d2Shell: shell });
  };
  const first = run(7);
  assert.equal(first.length, SCRIPT.ticks);
  assert.equal(first.findIndex((t) => t.monsters[0] === '4:147:2:5:5'), 133 - START_TICK - 1);
  for (const ranBefore of [8, 9, 12, START_TICK]) assert.deepEqual(run(ranBefore), first, `${ranBefore} ticks before the script`);
  assert.throws(() => run(START_TICK + 1), /past its start tick/);
  // A frame with no game tick is stepped the script's ticks and no more.
  const steps = []; new Function('window', `return (${PLAY_SCRIPT})(window.__d2Shell, ${JSON.stringify(SCRIPT)})`)({ __d2Shell: { snapshot: {}, input: { pointer() {} }, step: () => steps.push(1) } });
  assert.equal(steps.length, SCRIPT.ticks);
});

// ---- the live run ----
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
async function play(origin, query, packets) {
  const page = await openPage(`${origin}/index.html?retail=1&play=1&seed=1${query}`);
  try {
    // The packet run is ready when the frame is read from the client state; what that state then lacks is a field of the table, not a wait.
    if (!packets) await page.waitFor('(window.__d2Shell?.snapshot?.player?.hpMax ?? 0) > 0', 180000, 'the local game');
    if (packets) {
      await page.waitFor(`window.__d2Shell?.fromPackets === true || window.__d2Shell?.realmTransportEvidence?.state === 'failed'`, 180000, 'the game joined through the in-process connection')
        .catch(async (error) => { throw new Error(`${error.message}: ${await page.evaluate(JOIN_STATE).catch(String)}`); });
      if (!await page.evaluate('window.__d2Shell.fromPackets')) throw new Error(`the join through the in-process connection failed: ${await page.evaluate(JOIN_STATE)}`);
    }
    else assert.equal(await page.evaluate('window.__d2Shell.fromPackets'), false);
    return await page.evaluate(`(${PLAY_SCRIPT})(window.__d2Shell, ${JSON.stringify(SCRIPT)})`);
  } finally { page.close(); }
}

test('a local game read from the engine and the same game read from its packets show the same frame', { skip: unavailable() || false, timeout: 600000 + CONTROL_RUNS * 200000 }, async () => {
  const port = await freePort();
  const server = spawn(process.execPath, [join(ROOT, 'play', 'serve.mjs'), String(port), SITE], { stdio: 'ignore', env: { ...process.env, D2_RETAIL_DIR: RETAIL } });
  try {
    const origin = `http://127.0.0.1:${port}`;
    for (let i = 0; i < 50; i++) { if (await fetch(origin + '/index.html').then((r) => r.ok, () => false)) break; await sleep(100); }
    const direct = await play(origin, '', false);
    // The script is the same game again: a field that differs between two direct runs is not a parity field at all.
    // Every repeat is compared with the first run and the counts are summed (D2_ONE_PATH_CONTROL_RUNS repeats, 2 unless set).
    const control = Object.fromEntries(FIELDS.map((f) => [f, { differing: 0, total: 0 }]));
    let again = [];
    for (let i = 0; i < CONTROL_RUNS; i++) {
      again = await play(origin, '', false);
      for (const [f, v] of Object.entries(parityTable(direct, again))) { control[f].differing += v.differing; control[f].total += v.total; }
    }
    // A packet run that never comes up shows nothing: every record of the direct run then differs, and the reason is printed.
    let joinFailure = '';
    const packets = await play(origin, '&localrealm=1', true).catch((error) => { joinFailure = error.message; return []; });
    const table = parityTable(direct, packets);
    if (process.env.D2_ONE_PATH_DUMP) writeFileSync(process.env.D2_ONE_PATH_DUMP, JSON.stringify({ direct, again, packets }));   // the runs themselves, to read a differing field
    console.log(`one-path parity, ${SCRIPT.ticks} ticks (field: differing/total)\n${formatTable(table)}\ncontrol, direct against ${CONTROL_RUNS} more direct\n${formatTable(control)}${joinFailure ? `\npacket run: ${joinFailure}` : ''}`);
    for (const f of FIELDS) assert.equal(control[f].differing, 0, `${f} differs between two direct runs`);
    assert.ok(table.world.total > 0 && table.player.total > 0, 'the runs recorded nothing');
    if (process.env.D2_ONE_PATH_GATE === '1') assert.equal(joinFailure, '');
    if (process.env.D2_ONE_PATH_GATE === '1') for (const f of FIELDS) assert.equal(table[f].differing, 0, `${f}: ${table[f].differing}/${table[f].total}`);
  } finally { server.kill('SIGKILL'); }
});
