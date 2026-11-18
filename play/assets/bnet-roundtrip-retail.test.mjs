// The client-to-server half of the Battle.net round trip, measured against a recorded stock 1.13c client
// (`vectors/bnet-proof/<run>/rec.jsonl`, private; the test skips without them or without a staged engine).
// The browser's flow runs with the engine's composers over each recorded connection: login (selector, 0x50,
// 0x51, 0x3A, 0x40, 0x3E), the realm (selector, STARTUP, the character list, the character logon, the
// MOTD), the chat leg (channel list, ENTERCHAT, news, the channel join, each typed line), the game list,
// then the create and join with the BNCS frames that follow it. The Windows-side inputs retail read (its
// clock, address, locale, the Game.exe version check) are pinned from the recording. Every frame the
// browser sends is compared byte for byte with the frame retail sent at the same place in the sequence.
// Frames the browser never sends by design are listed apart: BNFTP downloads and the ad/file-time
// requests that start them (0x15, 0x21, 0x33), and the timer frames measured by their own tests
// (the 0x00 keepalive, the repeated 0x05 game list).
import { test } from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import { bncsLogin, mcpListCharacters, mcpLogonCharacter, mcpListen, mcpRequestMotd, mcpCreateAndJoin, bindEngineProtocol } from './realm-client.mjs';
import { createBnetChat } from './bnet-chat.mjs';
import { createChatLink } from './bnet-chat-link.mjs';
import { createMcpGameList } from './realm-gamelist.mjs';
import { haveEngine, loadEngine } from '../../test/helpers/node-engine.mjs';

const root = new URL('../../vectors/bnet-proof/', import.meta.url).pathname;
const runs = ['live1', 'live2'].filter((r) => fs.existsSync(`${root}${r}/rec.jsonl`));
const hex = (u) => Buffer.from(u).toString('hex');
const u16 = (b, o) => b[o] | (b[o + 1] << 8);
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
const cstr = (b, o) => { let e = b.indexOf(0, o); if (e < 0) e = b.length; return Buffer.from(b.subarray(o, e)).toString('latin1'); };
const settle = async (n = 20) => { for (let i = 0; i < n; i++) await new Promise((r) => setImmediate(r)); };

function connections(run) {
  const conns = new Map();
  for (const line of fs.readFileSync(`${root}${run}/rec.jsonl`, 'utf8').split('\n')) {
    if (!line.trim()) continue;
    const e = JSON.parse(line);
    if (e.ev !== 'chunk') continue;
    if (!conns.has(e.id)) conns.set(e.id, []);
    conns.get(e.id).push({ dir: e.dir, bytes: Uint8Array.from(Buffer.from(e.hex, 'hex')) });
  }
  return conns;
}
const stream = (chunks, dir) => Buffer.concat(chunks.filter((c) => c.dir === dir).map((c) => c.bytes));
const isBncs = (chunks) => { const c = stream(chunks, 'c2s'); return c[0] === 0x01 && c[1] === 0xff; };
const isMcp = (chunks) => { const c = stream(chunks, 'c2s'); return c[0] === 0x01 && c[1] !== 0xff && c[3] === 0x01; };

// Retail's C>S frames on one connection (the selector first), each with the chunk it ended in.
function retailFrames(chunks, mcp) {
  const starts = []; let pos = 0;
  chunks.forEach((c, i) => { if (c.dir === 'c2s') { starts.push({ i, from: pos }); pos += c.bytes.length; } });
  const all = stream(chunks, 'c2s'), out = [];
  const chunkAt = (o) => { let k = 0; while (k + 1 < starts.length && starts[k + 1].from <= o) k++; return starts[k].i; };
  for (let o = 0; o < all.length;) {
    if (o === 0 && all[0] === 1) { out.push({ i: chunkAt(0), key: 0x101, bytes: all.subarray(0, 1) }); o = 1; continue; }
    const n = mcp ? u16(all, o) : u16(all, o + 2);
    if ((!mcp && all[o] !== 0xff) || n < (mcp ? 3 : 4)) break;
    out.push({ i: chunkAt(o), key: mcp ? all[o + 2] : all[o + 1], bytes: all.subarray(o, o + n) });
    o += n;
  }
  return out;
}

// A recorded connection replayed to our client: each frame it sends releases the server's chunks up to retail's next frame.
function replayTransport(chunks, mcp) {
  const c2s = retailFrames(chunks, mcp), sent = [];
  const keyOf = (b) => (b.length === 1 ? 0x100 + b[0] : mcp ? b[2] : b[1]);
  let at = 0, cursor = 0, onData = () => {}; const queued = [], closers = [];
  const flush = () => { while (queued.length && onData.attached) onData(queued.shift()); };
  const release = (upTo) => { for (; at < upTo; at++) if (chunks[at].dir === 's2c') queued.push(chunks[at].bytes); flush(); };
  const transport = {
    isOpen: true, close() { transport.isOpen = false; for (const f of closers.splice(0)) f(); }, onClose(f) { closers.push(f); },
    onData: (cb) => { onData = cb; onData.attached = true; flush(); },
    send: (bytes) => {
      const b = Uint8Array.from(bytes), key = keyOf(b);
      sent.push({ key, bytes: b });
      if (!mcp && key === 0x25) return true;
      const k = c2s.findIndex((f, j) => j >= cursor && f.key === key);
      if (k < 0) return true;
      cursor = k + 1;
      const next = c2s.find((f, j) => j > k && f.key !== 0x25);
      release(next ? next.i : chunks.length);
      return true;
    },
  };
  return { transport, c2s, sent, releaseAll: () => release(chunks.length) };
}

// Not sent by the browser by design (BNFTP and what starts it), or timer frames measured by their own tests.
const BY_DESIGN = new Set([0x15, 0x21, 0x33]);
const TIMER_BNCS = new Set([0x00]);

// Pairs our frames with retail's in order per key and counts the differing bytes; retail frames with no partner are listed.
function compare(retail, ours, skip) {
  const left = retail.filter((f) => !skip(f)), rows = [];
  let diffs = 0;
  const used = new Set();
  for (const o of ours) {
    const k = left.findIndex((f, j) => !used.has(j) && f.key === o.key);
    if (k < 0) { rows.push({ key: o.key, extra: hex(o.bytes) }); diffs += o.bytes.length; continue; }
    used.add(k);
    const a = left[k].bytes, b = o.bytes;
    let d = 0; for (let x = 0; x < Math.max(a.length, b.length); x++) if (a[x] !== b[x]) d++;
    diffs += d;
    rows.push({ key: o.key, diffs: d, ...(d ? { retail: hex(a), ours: hex(b) } : {}) });
  }
  const missing = left.filter((_, j) => !used.has(j));
  for (const m of missing) { diffs += m.bytes.length; rows.push({ key: m.key, missing: hex(m.bytes) }); }
  return { diffs, rows };
}

async function roundTrip(run, Module) {
  const conns = [...connections(run).values()];
  const bncsChunks = conns.find(isBncs), mcpChunks = conns.find(isMcp);
  const bRetail = retailFrames(bncsChunks, false), mRetail = retailFrames(mcpChunks, true);
  const body = (frames, key, off) => frames.find((f) => f.key === key).bytes.subarray(off);
  // The host words retail read from Windows, from its own frames: the 0x51 seed step (tick + time from a zero seed), the 0x3A
  // GetTickCount, the address and the version check's outputs.
  const auth = body(bRetail, 0x51, 4), logonTick = u32(body(bRetail, 0x3a, 4), 0), info = body(bRetail, 0x50, 4);
  const exeInfo = cstr(auth, 20 + 36 * u32(auth, 12));
  const ticks = [u32(auth, 0), logonTick];
  const host = { tick: () => ticks.shift() ?? 0, time: () => 0, localIp: u32(info, 20), timezoneBias: () => u32(info, 24) | 0 };
  const protocol = bindEngineProtocol(Module, 65536, host);
  const account = cstr(body(bRetail, 0x3a, 4), 28), realm = cstr(body(bRetail, 0x3e, 4), 24);
  const password = 'probe-bnp2';

  const b = replayTransport(bncsChunks, false);
  const out = await bncsLogin(b.transport, protocol, { account, password, realm,
    auth: { exeVersion: u32(auth, 4), exeHash: u32(auth, 8), exeInfo }, timeoutMs: 500 });

  const m = replayTransport(mcpChunks, true);
  const listed = await mcpListCharacters(m.transport, protocol, { realmReply: out.handoff.realmReply, keepAliveMs: 1e9 });
  const session = listed.session; clearInterval(session.keepAlive);
  const character = cstr(body(mRetail, 0x07, 3), 0);
  await mcpLogonCharacter(session, character, { timeoutMs: 500 });
  mcpListen(session, () => false);
  mcpRequestMotd(session, () => {});
  await settle();

  // As the shell does: the link enters chat on the login's connection, ENTERCHAT carrying "<realm>,<character>".
  const link = createChatLink({ dial: async () => { throw new Error('no redial'); }, createChat: (o) => createBnetChat(o) });
  const chat = link.attach({ transport: b.transport, reader: out.reader }, { character, realm: `${out.realmList[0].title},${character}` });
  chat.news(0); await settle(); link.join(); await settle();
  const lines = bRetail.filter((f) => f.key === 0x0e).map((f) => cstr(f.bytes, 4));
  for (const line of lines) { chat.say(line); await settle(); }

  const list = createMcpGameList({ send: (op, bytes) => m.transport.send(protocol.mcpFrame(op, [...bytes])), nextId: () => { const id = session.nextRequestId; session.nextRequestId++; return id; } });
  session.nextRequestId = u16(body(mRetail, 0x05, 3), 0);
  list.request(); await settle();

  const create = body(mRetail, 0x03, 3), join = body(mRetail, 0x04, 3);
  session.nextRequestId = u16(create, 0);
  // The stock run typed only a name: the create panel's untouched boxes (4 players, level restriction on at 4).
  await mcpCreateAndJoin(m.transport, protocol, { realmReply: out.handoff.realmReply, character, gameName: cstr(join, 2), session, maxPlayers: 4, levelDifference: 4,
    keepAliveMs: 0, gameReplyMs: 500, onJoinSent: (name, opts) => link.enterGame(name, opts) });
  await settle();
  mcpListen(session, null); m.transport.close(); chat.close?.();

  // The first 0x05 is the panel opening compared here; the later ones are the 15 s refresh.
  let firstList = true;
  const bncs = compare(bRetail, b.sent, (f) => BY_DESIGN.has(f.key) || TIMER_BNCS.has(f.key));
  const mcp = compare(mRetail, m.sent, (f) => { if (f.key !== 0x05) return false; if (firstList) { firstList = false; return false; } return true; });
  const skipped = bRetail.filter((f) => BY_DESIGN.has(f.key) || TIMER_BNCS.has(f.key)).map((f) => f.key);
  return { bncs, mcp, skipped, bnftp: conns.filter((c) => stream(c, 'c2s')[0] === 0x02).length };
}

for (const run of runs) {
  test(`${run}: login, realm, lobby and create C>S frames against the stock client`, { skip: !haveEngine() && 'no staged engine' }, async () => {
    const Module = await loadEngine();
    const r = await roundTrip(run, Module);
    const report = (side) => r[side].rows.filter((x) => x.diffs !== 0).map((x) => `${side} 0x${x.key.toString(16)} ${JSON.stringify(x)}`);
    const bad = [...report('bncs'), ...report('mcp')];
    console.log(`${run}: bncs ${r.bncs.rows.length} frames ${r.bncs.diffs} differing bytes; mcp ${r.mcp.rows.length} frames ${r.mcp.diffs} differing bytes; ` +
      `not sent by design or timer-measured: ${r.skipped.length} BNCS frames, ${r.bnftp} BNFTP connections`);
    assert.equal(r.bncs.diffs + r.mcp.diffs, 0, bad.join('\n'));
  });
}
