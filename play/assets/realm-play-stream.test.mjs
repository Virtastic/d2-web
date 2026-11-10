// A recorded game-server stream of real play, replayed through the joined-game client. Each recording is a text
// file of one record per line, `<tick> <kind> <payload>`:
//   S <hex>   one decoded server packet, at the tick the reference client took it from its network layer
//   C <hex>   one client packet, at the tick the reference client sent it
//   H x,y,mode,...   the reference client's own hero after that tick's update
// The join burst goes through d2gsJoin exactly as a live connection's does, then every later packet goes to the
// engine's client state and to the realm world one tick at a time, with the recorded movement requests applied
// as the local player's own. The test fails on a packet the client cannot frame or its state refuses, and on any
// opcode no handler reads that is not already in IGNORED below (the list may only shrink); the
// hero's first differing tick is reported per recording, and asserted with PLAY_STREAM_STRICT=1.
//
// It needs the built engine (play/d2.js, play/d2.wasm, or PLAY_STREAM_ENGINE), the game archives (retail/D2, or
// PLAY_STREAM_DATA) and at least one recording (PLAY_STREAM_DIR); without them it is skipped.
import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync, readdirSync, openSync, readSync, openAsBlob } from 'node:fs';
import { fileURLToPath, pathToFileURL } from 'node:url';
import path from 'node:path';
import vm from 'node:vm';
import { bindEngineProtocol, d2gsJoin } from './realm-client.mjs';
import { createRealmWorld } from './realm-world.mjs';
import { createItemTable } from './realm-tables.mjs';
import { routeRealmPackets } from './realm-packet-routes.mjs';
import { hashString, decrypt, mpqTables } from './mpq-index.mjs';
import { readMpqFile } from './mpq-read.mjs';

const ROOT = fileURLToPath(new URL('../../', import.meta.url));
const ENGINE = process.env.PLAY_STREAM_ENGINE || path.join(ROOT, 'play');
const DATA = process.env.PLAY_STREAM_DATA || path.join(ROOT, 'retail/D2');
const STREAMS = process.env.PLAY_STREAM_DIR || path.join(ROOT, 'vectors/session/bnet-play');
const ARCHIVES = ['patch_d2.mpq', 'd2exp.mpq', 'd2data.mpq'];
const recordings = existsSync(STREAMS) ? readdirSync(STREAMS).filter((f) => f.endsWith('.txt')).sort() : [];
const skip = !existsSync(path.join(ENGINE, 'd2.js')) || !existsSync(path.join(ENGINE, 'd2.wasm')) ? 'engine build absent'
  : !ARCHIVES.every((a) => existsSync(path.join(DATA, a))) ? 'game archives absent'
  : !recordings.length ? 'no recorded play stream' : false;

// Server opcodes of the recordings that no handler of the client reads today, by the source of each consumer.
// An opcode leaves this list when a handler for it lands; a new one may not join it silently.
// Every opcode of the recordings is read today: the game's flags, the cursor's use-item, the cleared cursor
// item, the player's kept value, the used scroll or book and the act's animation data joined the handlers.
// PLAY_STREAM_TRACE=from-to prints the hero of both sides on those ticks.
const TRACE = process.env.PLAY_STREAM_TRACE ? process.env.PLAY_STREAM_TRACE.split('-').map(Number) : null;
export const IGNORED = [];
// Packets the client state refuses (it then drops everything it holds). None is expected.
export const REFUSED = [];
// The key words of the logon. The reference client's own logon composer writes version 0x0d and these two
// words (the first is the expansion's); the recordings' driver wrote the logon itself, with version 0x0e and
// 0xED5FCC50, so the recorded logon is compared without those five bytes.
const EXPANSION_KEY = 0xED5DCC50, RECORDED_DRIVER_KEY = 0xED5FCC50;
const logonFields = (text) => text.slice(0, 16) + text.slice(18, 24) + text.slice(32);

const hex = (text) => Uint8Array.from(text.match(/../g) ?? [], (b) => parseInt(b, 16));
const h2 = (v) => v.toString(16).padStart(2, '0');

// The opcodes each consumer names, read from its source: the engine's packet handler table and route table, and the
// modules the game shell hands the same bytes to.
function namedOpcodes() {
  const named = new Map();   // opcode -> [consumer]
  const add = (op, who) => { if (!named.has(op)) named.set(op, []); if (!named.get(op).includes(who)) named.get(op).push(who); };
  const src = (rel) => { const f = path.join(ROOT, rel); return existsSync(f) ? readFileSync(f, 'utf8') : ''; };
  const state = src('src/core/realm/d2gs_client_state.cpp');
  // One row of the handler table per packet the feed gives a body; the row's comment names its opcode or opcode range.
  const from = state.indexOf('kRealmPacketHandlers[] = {'), to = state.indexOf('};', from);
  const feed = from >= 0 && to > from ? state.slice(from, to) : '';
  for (const m of feed.matchAll(/\},\s*\/\/ 0x([0-9a-f]{2})(?:\.\.0x([0-9a-f]{2}))?/g))
    for (let op = parseInt(m[1], 16); op <= parseInt(m[2] ?? m[1], 16); op++) add(op, 'engine');
  for (const m of src('src/core/realm/d2gs_client_pkt_route.h').matchAll(/case 0x([0-9a-f]{2}): return \{ PktHandler/g)) add(parseInt(m[1], 16), 'engine');
  for (const mod of ['realm-world', 'realm-hud-state', 'realm-npc-state', 'realm-packet-routes', 'bnet-game-life'])
    for (const m of src(`play/assets/${mod}.mjs`).matchAll(/case 0x([0-9a-f]{2})\b|\[0\] === 0x([0-9a-f]{2})\b/g)) add(parseInt(m[1] ?? m[2], 16), mod);
  for (const op of [0x00, 0x01, 0x02, 0x0b, 0x59]) if (op === 0x0b || op === 0x59) add(op, 'join');
  return { named, engineSource: feed.length > 0 };
}

async function loadEngine() {
  globalThis.window = globalThis;
  globalThis.document = { currentScript: { src: pathToFileURL(path.join(ENGINE, 'd2.js')).href }, createElement: () => ({ getContext: () => null, style: {} }), addEventListener() {}, body: { appendChild() {} } };
  // The build starts a worker pool before it reports ready; the replay runs on the main thread only.
  globalThis.Worker = class { postMessage(m) { if (m && m.cmd === 1) setTimeout(() => this.onmessage && this.onmessage({ data: { cmd: 3 } }), 0); } terminate() {} addEventListener() {} };
  const realFetch = globalThis.fetch;
  globalThis.fetch = async (u, o) => {
    const s = String(u?.url ?? u);
    if (s.startsWith('file://')) return new Response(readFileSync(fileURLToPath(s)), { headers: { 'content-type': 'application/wasm' } });
    return realFetch(u, o);
  };
  const errors = [];
  const up = new Promise((resolve, reject) => {
    globalThis.Module = { noInitialRun: true, print: () => {}, printErr: (s) => { if (errors.length < 40) errors.push(String(s)); }, onRuntimeInitialized: resolve, onAbort: reject };
  });
  vm.runInThisContext(readFileSync(path.join(ENGINE, 'd2.js'), 'utf8'), { filename: 'd2.js' });
  await up;
  const Module = globalThis.Module;
  for (const a of ARCHIVES) {
    Module.FS.writeFile('/' + a, readFileSync(path.join(DATA, a)));
    assert.equal(Module.ccall('d2_vfs_mount', 'number', ['string'], ['/' + a]), 1, `mount ${a}`);
  }
  return { Module, protocol: bindEngineProtocol(Module), errors };
}

// The item and object tables the realm world decodes with, read from the same archives.
async function worldTables() {
  const archives = [];
  for (const name of ARCHIVES) {
    const file = path.join(DATA, name), fd = openSync(file, 'r'), blob = await openAsBlob(file);
    const at = (p, l) => { const b = Buffer.alloc(l); readSync(fd, b, 0, l, p); return new Uint8Array(b); };
    const head = at(0, 32), [[hp, hn], [bp, bn]] = mpqTables(head), shift = new DataView(head.buffer).getUint16(14, true);
    const table = (pos, n, key) => { const u = new Uint32Array(n * 4); new Uint8Array(u.buffer).set(at(pos, n * 16)); decrypt(u, hashString(key, 3)); return u; };
    archives.push({ blob, shift, hn, bn, hash: table(hp, hn, '(hash table)'), block: table(bp, bn, '(block table)') });
  }
  const fetchFile = async (name) => {
    for (const a of archives) {
      const x = hashString(name, 1), y = hashString(name, 2);
      for (let i = hashString(name, 0) & (a.hn - 1), n = 0; n < a.hn; i = (i + 1) & (a.hn - 1), n++) {
        const bi = a.hash[i * 4 + 3]; if (bi === 0xFFFFFFFF) break;
        if (bi < a.bn && a.hash[i * 4] === x && a.hash[i * 4 + 1] === y) {
          const entry = { pos: a.block[bi * 4], csize: a.block[bi * 4 + 1], size: a.block[bi * 4 + 2], flags: a.block[bi * 4 + 3] };
          const f = await readMpqFile(a.blob, 0, a.shift, entry, name); if (f) return new Uint8Array(await f.arrayBuffer());
        }
      }
    }
    return null;
  };
  const files = new Map();
  for (const n of ['objects.txt', 'weapons.txt', 'armor.txt', 'misc.txt']) { const key = 'data\\global\\excel\\' + n; files.set(key, await fetchFile(key)); }
  const read = (p) => { const f = files.get(p.toLowerCase()); if (!f) throw new Error('missing ' + p); return f; };
  return { items: createItemTable(read) };
}

function parseRecording(file) {
  const records = [];
  for (const line of readFileSync(file, 'utf8').split('\n')) {
    const m = /^(\d+) ([SCH]) (\S+)/.exec(line);
    if (m) records.push({ tick: +m[1], kind: m[2], text: m[3] });
  }
  return records;
}

/** Replays one recording; returns what the client did with it. */
export async function replay(records, { Module, protocol }, tables, named) {
  const sizeOf = (b) => protocol.d2gsPacketSize(b);
  const report = { server: 0, client: 0, ticks: 0, seen: new Map(), ignored: new Map(), refused: [], unframed: [], worldThrew: new Map(),
    events: new Map(), inputsApplied: new Map(), inputsNotApplied: new Map(), firstPosition: null, firstMode: null, positionDiffs: 0, modeDiffs: 0, compared: 0, heroAbsent: 0, track: [], trace: [] };
  const bump = (map, key) => map.set(key, (map.get(key) ?? 0) + 1);
  const world = createRealmWorld(tables);

  // The join: the logon and the enter request are answered with the bytes the server sent at that point.
  const assignAt = records.findIndex((r) => r.kind === 'S' && r.text.startsWith('59'));
  const logon = records.find((r) => r.kind === 'C' && r.text.startsWith('68'));
  assert.ok(assignAt >= 0 && logon, 'a recording starts with the logon and the player assignment');
  const assign = hex(records[assignAt].text), guid = new DataView(assign.buffer).getUint32(1, true);
  const select = records.findIndex((r, i) => i > assignAt && r.kind === 'S' && r.text.startsWith('0b00') && new DataView(hex(r.text).buffer).getUint32(2, true) === guid);
  assert.ok(select > assignAt, 'the server selects the assigned player');
  let burstEnd = select; while (burstEnd + 1 < records.length && records[burstEnd + 1].tick === records[select].tick && records[burstEnd + 1].kind !== 'H') burstEnd++;
  const enterAt = records.findIndex((r) => r.kind === 'C' && r.text === '6b');
  const bytesOf = (from, to) => Uint8Array.from(records.slice(from, to).filter((r) => r.kind === 'S').flatMap((r) => [...hex(r.text)]));
  const count = (r) => { if (r.kind === 'S') { report.server++; bump(report.seen, parseInt(r.text.slice(0, 2), 16)); } };
  records.slice(0, burstEnd + 1).forEach(count);
  let onData = () => {};
  const sent = [];
  const transport = {
    onData: (f) => { onData = f; }, onClose: () => {}, isOpen: true, close() {},
    send: (b) => {
      const bytes = Uint8Array.from(b); sent.push(bytes);
      if (bytes[0] === 0x68) queueMicrotask(() => onData(bytesOf(0, enterAt)));
      if (bytes[0] === 0x6b) queueMicrotask(() => onData(bytesOf(enterAt, burstEnd + 1)));
      return true;
    },
  };
  const logonBytes = hex(logon.text), logonView = new DataView(logonBytes.buffer);
  const name = String.fromCharCode(...assign.subarray(6, 22)).replace(/\0.*$/, '');
  const joining = d2gsJoin(transport, protocol, { character: name, charClass: assign[5], gameHash: logonView.getUint32(1, true), gameToken: logonView.getUint16(5, true), expansion: [EXPANSION_KEY, RECORDED_DRIVER_KEY].includes(logonView.getUint32(12, true)), timeoutMs: 5000 });
  queueMicrotask(() => onData(Uint8Array.of(0xaf, 0x00)));   // the recording is the decoded stream: the unframed dialect
  const game = await joining;
  report.join = { sent: sent.map((b) => h2(b[0])), bytes: game.bytes, logon: Buffer.from(sent[0]).toString('hex'), recordedLogon: logon.text };
  world.setOwner(guid);

  const feedOne = (packet, tick) => {
    const op = packet[0];
    const result = protocol.d2gsClientPlayerFeed(packet);
    if (result < 0) report.refused.push({ tick, op, result, hex: Buffer.from(packet).toString('hex') });
    for (const event of protocol.d2gsClientTakeSessionEvents()) bump(report.events, event.op);
    let jsRead = false;
    try { world.feed(packet, sizeOf); } catch (e) { bump(report.worldThrew, op); }
    try { jsRead = routeRealmPackets(packet, sizeOf, { npcInteraction: () => true, applyHotkey: () => true, applyWeaponSwitch: () => true }).length > 0; } catch { /* counted as unread */ }
    if (!(result > 0) && !jsRead && !named.has(op)) bump(report.ignored, op);
    return result;
  };
  const feedBytes = (bytes, tick) => {
    for (let at = 0; at < bytes.length;) {
      const n = sizeOf(bytes.subarray(at));
      if (!(n > 0) || at + n > bytes.length) { report.unframed.push({ tick, op: bytes[at] }); return; }
      feedOne(bytes.subarray(at, at + n), tick); at += n;
    }
  };
  // What the join already gave the engine counts as read by it; the join's own world feed is the shell's.
  for (const r of records.slice(0, burstEnd + 1)) if (r.kind === 'S') { const p = hex(r.text), op = p[0]; try { world.feed(p, sizeOf); } catch { bump(report.worldThrew, op); } if (!named.has(op)) bump(report.ignored, op); }
  const tail = await game.readFrame();   // the rest of the burst, after the selection
  if (tail?.length) for (let at = 0; at < tail.length;) { const n = sizeOf(tail.subarray(at)); if (!(n > 0)) break; const r = protocol.d2gsClientPlayerFeed(tail.subarray(at, at + n)); if (r < 0) report.refused.push({ tick: records[select].tick, op: tail[at], result: r, hex: Buffer.from(tail.subarray(at, at + n)).toString('hex') }); at += n; }

  const move = (x, y, run) => Module.ccall('d2_realm_client_local_move', 'number', ['number', 'number', 'number'], [x, y, run]);
  const moveToUnit = (type, id, run) => Module.ccall('d2_realm_client_local_move_to_unit', 'number', ['number', 'number', 'number'], [type, id, run]);
  for (const r of records.slice(burstEnd + 1)) {
    if (r.kind === 'S') { count(r); const p = hex(r.text); if (sizeOf(p) !== p.length) report.unframed.push({ tick: r.tick, op: p[0] }); else feedOne(p, r.tick); continue; }
    if (r.kind === 'C') {
      report.client++;
      const p = hex(r.text), v = new DataView(p.buffer), op = p[0];
      if ((op === 0x01 || op === 0x03) && p.length === 5) { move(v.getUint16(1, true), v.getUint16(3, true), op === 0x03 ? 1 : 0); bump(report.inputsApplied, op); }
      else if ((op === 0x02 || op === 0x04) && p.length === 9) { moveToUnit(v.getUint32(1, true), v.getUint32(5, true), op === 0x04 ? 1 : 0); bump(report.inputsApplied, op); }
      else bump(report.inputsNotApplied, op);
      continue;
    }
    if (r.tick < records[select].tick) continue;
    protocol.d2gsClientMotionTick();
    report.ticks++;
    const [x, y, mode] = r.text.split(',').map(Number);
    const ours = protocol.d2gsClientPlayerState();
    report.track.push([x, y, ours?.active ? ours.x : null, ours?.active ? ours.y : null]);
    if (!ours?.active) { report.heroAbsent++; report.firstPosition ??= { tick: r.tick, retail: [x, y], ours: null }; report.firstMode ??= { tick: r.tick, retail: mode, ours: null }; continue; }
    report.compared++;
    const ourMode = ours.motion?.mode ?? ours.initialMode;
    if (TRACE && r.tick >= TRACE[0] && r.tick <= TRACE[1]) report.trace.push(`${r.tick} retail ${x},${y} mode ${mode} ours ${ours.x},${ours.y} mode ${ourMode}`);
    if (ours.x !== x || ours.y !== y) { report.positionDiffs++; report.firstPosition ??= { tick: r.tick, retail: [x, y], ours: [ours.x, ours.y] }; }
    if (ourMode !== mode) { report.modeDiffs++; report.firstMode ??= { tick: r.tick, retail: mode, ours: ourMode }; }
  }
  // How far the hero trails the reference: the shift in ticks at which the fewest positions differ.
  report.roster = protocol.d2gsClientRoster(); report.guid = guid; report.name = name;
  report.lag = [-2, -1, 0, 1, 2, 3, 4].map((k) => [k, report.track.filter((t, i) => { const o = report.track[i + k]; return !o || o[2] !== t[0] || o[3] !== t[1]; }).length]).sort((a, b) => a[1] - b[1])[0];
  return report;
}

const fmt = (map) => [...map].sort((a, b) => a[0] - b[0]).map(([op, n]) => `${h2(op)}x${n}`).join(' ') || 'none';

test('recorded play streams replay through the joined-game client', { skip }, async (t) => {
  const engine = await loadEngine();
  const tables = await worldTables();
  const { named, engineSource } = namedOpcodes();
  const ignoredAll = new Map();
  for (const file of recordings) {
    await t.test(file, async (t) => {
      const report = await replay(parseRecording(path.join(STREAMS, file)), engine, tables, named);
      t.diagnostic(`${file}: ${report.server} server packets, ${report.client} client packets, ${report.ticks} ticks`);
      if (logonFields(report.join.logon) !== logonFields(report.join.recordedLogon)) t.diagnostic(`  logon differs: reference ${report.join.recordedLogon} ours ${report.join.logon}`);
      t.diagnostic(`  roster: ${report.roster.map((p) => `${p.name} level ${p.level}`).join(', ') || 'empty'}; session events: ${fmt(report.events)}`);
      t.diagnostic(`  ignored opcodes: ${fmt(report.ignored)}`);
      t.diagnostic(`  refused by the client state: ${report.refused.length ? report.refused.slice(0, 6).map((r) => `tick ${r.tick} ${h2(r.op)} -> ${r.result}`).join(', ') + (report.refused.length > 6 ? ` (+${report.refused.length - 6})` : '') : 'none'}`);
      t.diagnostic(`  world threw on: ${fmt(report.worldThrew)}`);
      t.diagnostic(`  inputs applied: ${fmt(report.inputsApplied)}; inputs with no local prediction: ${fmt(report.inputsNotApplied)}`);
      t.diagnostic(`  hero position: ${report.positionDiffs}/${report.compared} ticks differ, first ${report.firstPosition ? `tick ${report.firstPosition.tick} retail ${report.firstPosition.retail} ours ${report.firstPosition.ours}` : 'none'}`);
      t.diagnostic(`  hero position with ours read ${report.lag[0]} ticks later: ${report.lag[1]} ticks differ; hero absent on ${report.heroAbsent} ticks`);
      t.diagnostic(`  hero mode: ${report.modeDiffs}/${report.compared} ticks differ, first ${report.firstMode ? `tick ${report.firstMode.tick} retail ${report.firstMode.retail} ours ${report.firstMode.ours}` : 'none'}`);
      for (const line of report.trace) t.diagnostic(`  ${line}`);
      for (const [op, n] of report.ignored) ignoredAll.set(op, (ignoredAll.get(op) ?? 0) + n);
      assert.deepEqual(report.join.sent, ['68', '6b'], 'the join sends the logon and the enter request');
      assert.deepEqual(report.unframed, [], 'every recorded packet has the size the client frames it with');
      const first = report.refused[0];
      assert.ok(!first || REFUSED.some((k) => k.op === first.op && k.result === first.result), `the client state refuses ${first && h2(first.op)} at tick ${first?.tick}, which is not a listed refusal`);
      assert.equal(report.worldThrew.size, 0, 'the realm world reads every packet without an error');
      assert.ok(report.roster.some((p) => p.guid === report.guid && p.name === report.name), 'the roster holds the local player');
      assert.equal(report.join.logon.slice(16, 18) + report.join.logon.slice(24, 32), '0d50cc5ded', 'the logon carries the client version and the expansion key');
      if (process.env.PLAY_STREAM_STRICT === '1') {
        assert.deepEqual(report.refused, [], 'the client state takes every packet of the stream');
        assert.equal(report.ignored.size, 0, 'no opcode is ignored');
        assert.equal(logonFields(report.join.logon), logonFields(report.join.recordedLogon), 'the logon bytes equal the reference client\'s');
        assert.equal(report.firstPosition, null, 'the hero is where the reference client had it on every tick');
        assert.equal(report.firstMode, null, 'the hero is in the reference client\'s mode on every tick');
      }
    });
  }
  t.diagnostic(`ignored opcodes over ${recordings.length} recordings: ${fmt(ignoredAll)}`);
  if (engineSource) assert.deepEqual([...ignoredAll.keys()].filter((op) => !IGNORED.includes(op)).map(h2), [], 'no opcode outside the listed ones is ignored');
});
