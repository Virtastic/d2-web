// The server-to-client half of Battle.net, replayed from a recorded stock 1.13c client session: the bytes PvPGN and the realm
// sent a retail Diablo II (`vectors/bnet-proof/<run>/rec.jsonl`, a TCP proxy's log, one JSON event per line). Every S>C frame is
// split the way the retail pump splits it and handled twice: once by a transcription of the retail handler its id dispatches to
// (BNClient's table at +0x1E128, D2MCPClient's at +0xA900), once by our client driven through the same bytes. The two handled
// records must agree. Retail's own C>S frames in the recording are a second oracle (the ping echo, the realm it logged on to, the
// MCP startup it built from the hand-off, the request ids it waited on). The vectors are private; without them the test skips.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import { bncsLogin, mcpListCharacters, mcpLogonCharacter, mcpListen, mcpRequestMotd, mcpCreateAndJoin } from './realm-client.mjs';
import { authRefusalEvent, logonFailureEvent } from './bnet-login-screen.mjs';
import { createBnetChat, parseChatEvent } from './bnet-chat.mjs';
import { createNews, createMotd } from './bnet-news.mjs';

const root = new URL('../../vectors/bnet-proof/', import.meta.url).pathname;
const runs = fs.existsSync(root) ? fs.readdirSync(root).filter((r) => fs.existsSync(`${root}${r}/rec.jsonl`)).sort() : [];
const hex = (u) => Buffer.from(u).toString('hex');
const u16 = (b, o) => b[o] | (b[o + 1] << 8);
const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
const cstr = (b, o, cap = Infinity) => { let e = b.indexOf(0, o); if (e < 0) e = b.length; return Buffer.from(b.subarray(o, Math.min(e, o + cap))).toString('latin1'); };
const settle = async (n = 20) => { for (let i = 0; i < n; i++) await new Promise((r) => setImmediate(r)); };

// One run's TCP connections, each a list of chunks in arrival order.
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
// A game connection opens with the 0x01 selector, then BNCS speaks FF frames and MCP `len16 op` frames (0x02 is BNFTP).
const isBncs = (chunks) => { const c = stream(chunks, 'c2s'); return c[0] === 0x01 && c[1] === 0xff; };
const isMcp = (chunks) => { const c = stream(chunks, 'c2s'); return c[0] === 0x01 && c[1] !== 0xff && c[3] === 0x01; };

// BNClient+0xEA60: from the next 0xFF, a frame needs its four header bytes and its declared length; the pump steps on by that
// length (a declared length under four never moves it again, so the split stops there).
export function bncsFrames(bytes) {
  const out = []; let at = 0;
  for (;;) {
    while (at < bytes.length && bytes[at] !== 0xff) at++;
    if (at + 4 > bytes.length) break;
    const total = bytes[at + 2] | (bytes[at + 3] << 8);
    if (total < 4 || at + total > bytes.length) break;
    out.push({ id: bytes[at + 1], body: bytes.subarray(at + 4, at + total), frame: bytes.subarray(at, at + total) });
    at += total;
  }
  return out;
}
// MCP frames: `len16 op body`, the length counting its own three bytes.
function mcpFrames(bytes) {
  const out = []; let at = 0;
  while (at + 3 <= bytes.length) { const total = u16(bytes, at); if (total < 3 || at + total > bytes.length) break; out.push({ id: bytes[at + 2], body: bytes.subarray(at + 3, at + total) }); at += total; }
  return out;
}

// The ids with a non-zero entry in BNClient's dispatch table (+0x1E128, ids below 0x5E only).
const RETAIL_HANDLED = new Set([0x00, 0x04, 0x05, 0x06, 0x07, 0x09, 0x0a, 0x0b, 0x0f, 0x15, 0x1c, 0x1d, 0x20, 0x25, 0x26, 0x28, 0x29, 0x2a,
  0x31, 0x33, 0x3a, 0x3d, 0x3e, 0x40, 0x41, 0x42, 0x46, 0x4a, 0x4c, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59]);

// BNClient+0x12C30 and +0x12810 as written: insert from the end of the ts-ascending list, an equal ts replaces the text, a ts of 0
// is not stored; then the count of leading entries older than the +5 threshold, and the callback walk from the last entry down.
function retailNews(list, body) {
  const count = body[0] >= 0x80 ? 0x100000000 - 0x100 + body[0] : body[0];
  if (count > 0x100) return { fatal: true };
  let p = 0xd;
  for (let c = count; c; c--) {
    const ts = u32(body, p), text = cstr(body, p + 4, 0x400); p = body.indexOf(0, p + 4) + 1;
    if (ts === 0) continue;
    let k = list.length, at = -1;
    while (k) { k--; if (list[k].ts < ts) { at = k; break; } if (list[k].ts === ts) { at = -2; list[k].text = text; break; } }
    if (at !== -2) list.splice(at + 1, 0, { ts, text });
  }
  const threshold = u32(body, 5); let dc = 0; while (dc < list.length && list[dc].ts >= 0 && !(threshold <= list[dc].ts)) dc++;
  const shown = []; for (let i = list.length - 1; i >= dc; i--) shown.push({ ts: list[i].ts, text: list[i].text });
  return { shown };
}

// What retail's handler keeps from one S>C BNCS frame (the state its waits and the front end read), by id. `state` is the
// connection's handler state (the last ad id, the news list).
export function retailRecord(id, body, state = {}) {
  if (!RETAIL_HANDLED.has(id)) return { ignored: true };
  const n = body.length;
  switch (id) {
    case 0x00: // +0x10A80: returns 1, nothing kept.
      return { noop: true };
    case 0x25: // +0x10CF0: the body goes straight back through the send choke point (+0xEB00) under id 0x25.
      return { echo: hex([0xff, 0x25, (n + 4) & 255, (n + 4) >> 8, ...body]) };
    case 0x50: { // +0x115B0: over 0x15 bytes, last byte NUL, exactly one NUL in [0x14, n-1); else nothing is stored.
      if (n <= 0x15 || body[n - 1] !== 0) return { invalid: true };
      let nuls = 0; for (let i = 0x14; i < n - 1; i++) if (body[i] === 0) nuls++;
      if (nuls !== 1) return { invalid: true };
      const mpq = cstr(body, 0x14);
      return { logonType: u32(body, 0), serverToken: u32(body, 4), mpq, formula: cstr(body, 0x14 + mpq.length + 1, 0x103) };
    }
    case 0x51: { // +0x11330: at least 5 bytes ending in NUL, no NUL in [4, n-1); the event by result & 0xFF0F, 0x10 the expansion bit.
      if (n < 5 || body[n - 1] !== 0) return { invalid: true };
      for (let i = 4; i < n - 1; i++) if (body[i] === 0) return { invalid: true };
      const result = u32(body, 0), r = result & 0xff0f, x = (result & 0xf0) ? 4 : 0;
      const event = { 0: null, 0x100: 'patch', 0x101: 1, 0x102: 1, 0x200: 3 + x, 0x201: 6 + x, 0x202: 4 + x, 0x203: 5 + x }[r];
      return { result, event: event === undefined ? 'none' : event, ...(r === 0x201 ? { owner: cstr(body, 4) } : {}) };
    }
    case 0x3a: { // +0x109A0: 0 passes; 1, 2, 3 raise failure events 0xD, 0xE, 0xF; 6 raises 0x1B with the text after the result; else 0x12.
      const result = u32(body, 0);
      const event = result === 0 ? null : { 1: 0xd, 2: 0xe, 3: 0xf, 6: 0x1b }[result] ?? 0x12;
      return { result, event, ...(result === 6 ? { text: cstr(body, 4) } : {}) };
    }
    case 0x59: // +0x107C0: SID_SETEMAIL only sets the email-request flag (slot +0xB4), which D2Launch reads after a passed logon.
      return { emailRequested: true };
    case 0x33: // +0x116E0: the file time reply starts a BNFTP download of the named file (+0xE120) for the request id.
      return { download: cstr(body, 16), request: u32(body, 0) };
    case 0x15: { // +0x11670: over 0x10 bytes and a new ad id: a non-empty file name and URL start a BNFTP download (+0xE120).
      if (n <= 0x10 || u32(body, 0) === state.lastAd) return { noop: true };
      state.lastAd = u32(body, 0);
      const file = cstr(body, 16), url = cstr(body, 16 + file.length + 1);
      return file && url ? { download: file, url } : { noop: true };
    }
    case 0x40: { // D2Launch+0x15B60 over the reply BNClient's +0x10890 / +0xB7F0 stored: count at +4 (0x200 or more is fatal), then
      // `[u32][title\0][description\0]` per realm; the list is sorted by title, a plain byte compare (D2Launch+0x12020).
      const count = u32(body, 4); if (count >= 0x200) return { fatal: true };
      const titles = []; let o = 8;
      for (let i = 0; i < count; i++) { o += 4; const t = cstr(body, o); o += t.length + 1; o = body.indexOf(0, o) + 1; titles.push(t); }
      return { titles: titles.sort((a, b) => (a < b ? -1 : a > b ? 1 : 0)) };
    }
    case 0x3e: // +0x10CB0 / +0xB920: the body is stored; when it is not 8 bytes and at least 0x48, the IP is the raw bytes at +16 and
      // the port the big-endian word at +20.
      if (n === 8 || n < 0x48) return { stored: hex(body) };
      return { stored: hex(body), ip: [...body.subarray(16, 20)].join('.'), port: (body[20] << 8) | body[21] };
    case 0x0a: // +0x10CC0: the first string is the unique name, kept with a 0x100-byte copy.
      return { uniqueName: cstr(body, 0, 255) };
    case 0x0b: { // +0x108C0 / +0xB700 stores the body; D2Multi+0xB970 reads names up to the first empty one.
      const names = []; let o = 0; while (o < n && body[o]) { const s = cstr(body, o); names.push(s); o += s.length + 1; }
      return { channels: names };
    }
    case 0x0f: { // +0xFEC0: the body cut to 0x120 bytes is queued; D2Multi reads event, flags, ping, then the name at +24 and the text.
      const b = body.subarray(0, 0x120);
      if (b.length < 26 || b.indexOf(0, 24) < 0) return { invalid: true };
      const name = cstr(b, 24);
      return { event: u32(b, 0), flags: u32(b, 4), ping: u32(b, 8), username: name, text: cstr(b, 24 + name.length + 1) };
    }
    case 0x46: // +0x12C30 / +0x12810: the entries the news box is redrawn with, newest first.
      return retailNews(state.news ??= [], body);
    case 0x1c: // +0x10A60: a result of 0 sets the game-advertised flag.
      return { advertised: u32(body, 0) === 0 };
    default:
      return { unported: true };
  }
}

// What D2MCPClient's handler keeps from one S>C MCP frame. `pending` is the request id retail's own C>S frame carried.
function retailMcpRecord(op, body, state) {
  switch (op) {
    case 0x01: return { result: u32(body, 0) };   // the startup wait (+0x6990's synchronous reply) reads the result dword
    case 0x19: { // +0x65D0: the words at +0, +2, +4, +6 under 0x100 (else fatal); `[u32 expiry][name\0][stat\0]` x the word at +6.
      for (const at of [0, 2, 4, 6]) if (u16(body, at) >= 0x100) return { fatal: true };
      const names = [], expiry = []; let o = 8;
      for (let i = u16(body, 6); i; i--) { expiry.push(u32(body, o)); const s = cstr(body, o + 4); names.push(s); o = body.indexOf(0, body.indexOf(0, o + 4) + 1) + 1; }
      return { total: u32(body, 2), returned: u16(body, 6), names, expiry };
    }
    case 0x07: return { result: u32(body, 0) };   // +0x6570
    case 0x12: { // +0x5EE0 -> D2Multi+0x9E30 / +0x9D60: the key-sorted parts joined, at most 0x200 bytes, read as UTF-8.
      if (body.length < 1) return { noop: true };
      (state.motd ??= new Map()).set(body[0], Buffer.from(body.subarray(1, Math.min(body.indexOf(0, 1) < 0 ? body.length : body.indexOf(0, 1), 0x201))));
      const all = Buffer.concat([...state.motd].sort((a, b) => a[0] - b[0]).map((e) => e[1])).subarray(0, 0x200);
      return { motd: new TextDecoder().decode(all) };
    }
    case 0x03: // +0x6540: only the pending request's reply: the result at +6, the token at +2.
      if (u16(body, 0) !== state.pending?.[3]) return { stale: true };
      return { result: u32(body, 6), token: u16(body, 2) };
    case 0x04: // +0x64D0: only the pending request's reply; an IP of 0 fails with the result at +14, or 0x28 when that is 0 too.
      if (u16(body, 0) !== state.pending?.[4]) return { stale: true };
      if (u32(body, 6) === 0) return { result: u32(body, 14) || 0x28 };
      return { result: 0, ip: [...body.subarray(6, 10)].join('.'), token: u16(body, 2), hash: u32(body, 10) };
    default: return { unported: true };
  }
}

// A transport that replays one recorded connection: each frame our client sends releases what the server sent after retail's
// frame with that id, up to retail's next frame (a ping echo is the client's own answer and releases nothing).
function replayTransport(chunks, mcp) {
  const c2s = []; let first = true, pos = 0; const starts = [];
  chunks.forEach((c, i) => { if (c.dir === 'c2s') { starts.push({ i, from: pos }); pos += c.bytes.length; } });
  const all = stream(chunks, 'c2s'); const chunkAt = (o) => { let k = 0; while (k + 1 < starts.length && starts[k + 1].from <= o) k++; return starts[k].i; };
  for (let o = 0; o < all.length;) {
    if (first) { first = false; if (all[0] === 1) { c2s.push({ i: chunkAt(0), key: 0x101 }); o = 1; continue; } }
    if (mcp) { const n = u16(all, o); if (n < 3) break; c2s.push({ i: chunkAt(o), key: all[o + 2], body: all.subarray(o + 3, o + n) }); o += n; }
    else { if (all[o] !== 0xff) break; const n = u16(all, o + 2); if (n < 4) break; c2s.push({ i: chunkAt(o), key: all[o + 1], body: all.subarray(o + 4, o + n) }); o += n; }
  }
  const keyOf = (b) => (b.length === 1 ? 0x100 + b[0] : mcp ? b[2] : b[1]);
  const isEcho = (f) => !mcp && f.key === 0x25;
  let at = 0, cursor = 0, onData = () => {}; const queued = [], echoes = [], closers = [];
  const flush = () => { while (queued.length && onData.attached) onData(queued.shift()); };
  const release = (upTo) => { for (; at < upTo; at++) if (chunks[at].dir === 's2c') queued.push(chunks[at].bytes); flush(); };
  const transport = {
    isOpen: true, close() { transport.isOpen = false; for (const f of closers.splice(0)) f(); }, onClose(f) { closers.push(f); },
    onData: (cb) => { onData = cb; onData.attached = true; flush(); },
    send: (bytes) => {
      const b = Uint8Array.from(bytes), key = keyOf(b);
      if (!mcp && key === 0x25) { echoes.push(hex(b)); return true; }
      const k = c2s.findIndex((f, j) => j >= cursor && f.key === key);
      if (k < 0) return true;
      cursor = k + 1;
      const next = c2s.find((f, j) => j > k && !isEcho(f));
      release(next ? next.i : chunks.length);
      return true;
    },
  };
  return { transport, c2s, echoes, releaseAll: () => release(chunks.length), get at() { return at; } };
}

// Our client over one recorded BNCS connection: the logon, then the chat leg on the socket it keeps (the channel list and
// ENTERCHAT, the news request, the channel join, each line retail typed). Returns our handled record per delivered frame.
async function oursOverBncs(chunks) {
  const t = replayTransport(chunks, false);
  const seen = {};
  const protocol = {
    bncsSelector: () => Uint8Array.of(1),
    bncsFrame: (id, body) => Uint8Array.from([0xff, id, (body.length + 4) & 255, (body.length + 4) >> 8, ...body]),
    bncsAuthInfo: () => [],
    bncsAuthCheck: (a) => { seen.authInfo = a; return { body: [], clientToken: 1 }; },
    bncsLogon: () => [], bncsQueryRealms: () => { seen.queried = true; return []; }, tickCount: () => 1, realmToken: () => 1,
    bncsRealmEx: (r) => { seen.realm = r.realm; return []; },
  };
  let error = null, out = null;
  try { out = await bncsLogin(t.transport, protocol, { account: 'a', password: 'b', timeoutMs: 200 }); } catch (e) { error = e; }
  const got = { 0x0b: [], 0x0a: [], 0x0f: [], 0x46: [] };
  if (out) {
    const news = createNews();
    const chat = createBnetChat({ transport: t.transport, reader: out.reader, character: 'x',
      onChannels: (l) => got[0x0b].push({ channels: l }), onEntered: (u) => got[0x0a].push({ uniqueName: u }),
      // Retail queues the event's bytes, not a decoded string: compared on the bytes our client kept (`raw`, the statstring's source),
      // read up to their NUL the way `cstr` reads retail's.
      onEvent: (e) => { got[0x0f].push({ event: e.event, flags: e.flags, ping: e.ping, username: e.username, text: cstr(e.raw, 0) }); return true; },
      onNews: (b) => { const shown = news.apply(b); got[0x46].push(shown ? { shown } : { short: true }); } });
    chat.enter(); chat.news(0); chat.join();
    await settle();
    for (const f of t.c2s.filter((x) => x.key === 0x0e)) { chat.say(cstr(f.body, 0)); await settle(); }
    t.releaseAll(); await settle();
    chat.close();
  }
  const records = [];
  for (const f of bncsFrames(stream(chunks.slice(0, t.at), 's2c'))) {
    let r;
    if (f.id === 0x25) r = { echo: t.echoes.shift() };
    else if (f.id === 0x50 && seen.authInfo) {
      // Our AUTH_CHECK step gets the token, the archive name and the formula; it builds the logon with the type-0 (broken SHA-1) proof.
      const a = seen.authInfo, formula = cstr(a.challenge, 0);
      r = { logonType: 0, serverToken: a.serverToken, mpq: a.mpq, formula };
    } else if (f.id === 0x51) {
      // Passed: the flow went on to the logon (bncsAuth is kept). Refused: the error's result picks the box (none for a patch).
      const refused = !t.transport.bncsAuth && error?.authResult !== undefined;
      const result = refused ? error.authResult : t.transport.bncsAuth?.versionResult;
      const event = !refused ? null : (result & 0xff0f) === 0x100 ? 'patch' : authRefusalEvent(error) ?? 'none';
      r = { result, event, ...(refused && (result & 0xff0f) === 0x201 ? { owner: error.authInfo } : {}) };
    } else if (f.id === 0x3a) {
      // Passed: the flow went on to ask for the realms. Refused: the error's result and the box it raises.
      const result = seen.queried ? 0 : error?.logonResult;
      r = { result, event: result === 0 ? null : logonFailureEvent(error), ...(result === 6 ? { text: error.logonReason } : {}) };
    } else if (f.id === 0x59) r = { emailRequested: t.transport.bncsEmailRequested };
    else if (f.id === 0x40 && out) r = { titles: out.realmList.map((x) => x.title).sort((a, b) => (a < b ? -1 : a > b ? 1 : 0)) };
    else if (f.id === 0x3e && out) r = { stored: hex(out.handoff.realmReply), ip: out.handoff.ip, port: out.handoff.port };
    else if (f.id === 0x0f && !parseChatEvent(f.body)) r = { invalid: true };
    else if (got[f.id]?.length) r = got[f.id].shift();
    else if (f.id === 0x00) r = { noop: true };   // the chat loop's frames with no case: read and dropped
    else r = { notHandled: true };
    records.push({ id: f.id, frame: f, ours: r });
  }
  return { records, out, seen, c2s: t.c2s };
}

// Our MCP leg over the recorded realm connection: STARTUP and the character list, the character logon, the lobby's MOTD request,
// then the create and join retail made (our request ids set to retail's, the one piece of C>S state the replies are matched on).
async function oursOverMcp(chunks, handoff) {
  const t = replayTransport(chunks, true);
  const retailId = (op) => { const f = t.c2s.find((x) => x.key === op); return f ? u16(f.body, 0) : null; };
  const protocol = {
    mcpSelector: () => Uint8Array.of(1), mcpFrame: (op, body) => Uint8Array.from([(body.length + 3) & 255, (body.length + 3) >> 8, op, ...body]),
    mcpStartup: () => [...handoff.startup], mcpCharlist: (n) => [n, 0, 0, 0], mcpCharlogon: (name) => [...Buffer.from(`${name}\0`)],
    mcpCreateGame: () => [], mcpJoinGame: () => [],
  };
  const got = { 0x01: [], 0x19: [], 0x07: [], 0x12: [], 0x03: [], 0x04: [] };
  const listed = await mcpListCharacters(t.transport, protocol, { realmReply: handoff.realmReply, keepAliveMs: 1e9 });
  const session = listed.session; clearInterval(session.keepAlive);
  got[0x01].push({ result: 0 });
  got[0x19].push({ total: listed.total, returned: listed.returned, names: listed.characters.map((c) => c.name), expiry: listed.characters.map((c) => c.expiry) });
  const logon = t.c2s.find((x) => x.key === 0x07);
  if (logon) { await mcpLogonCharacter(session, cstr(logon.body, 0), { timeoutMs: 500 }); got[0x07].push({ result: 0 }); }
  const motd = createMotd();
  mcpListen(session, () => false);
  if (t.c2s.some((x) => x.key === 0x12)) mcpRequestMotd(session, (key, bytes) => got[0x12].push({ motd: motd.add(key, bytes) }));
  await settle();
  const join = t.c2s.find((x) => x.key === 0x04);
  if (join && logon) {
    const create = retailId(0x03) !== null;
    session.nextRequestId = create ? retailId(0x03) : retailId(0x04);
    const r = await mcpCreateAndJoin(t.transport, protocol, { realmReply: handoff.realmReply, character: cstr(logon.body, 0), gameName: cstr(join.body, 2), session, join: !create, gameReplyMs: 500 });
    if (create) got[0x03].push({ result: 0, token: r.game.createToken });
    got[0x04].push({ result: 0, ip: r.ticket.ip, token: r.ticket.token, hash: r.ticket.hash });
  }
  t.releaseAll(); await settle();
  mcpListen(session, null); t.transport.close();
  const records = mcpFrames(stream(chunks.slice(0, t.at), 's2c')).map((f) => ({ id: f.id, body: f.body, ours: got[f.id]?.shift() ?? { notHandled: true } }));
  return { records, pending: { 3: retailId(0x03), 4: retailId(0x04) } };
}

// Retail S>C frames our client is not built to receive, each for a reason the project chose: BNFTP downloads (0x33 file times, 0x15
// ads: the browser downloads no files, the server list and assets are the project's own) and the replies to requests it never
// makes (0x1C, STARTADVEX3's result: the closed realm's games are made over MCP and the game is not advertised on BNCS).
const byDesign = (id, retail) => Boolean(retail.download) || id === 0x1c;

// One count per S>C id: frames seen, frames whose records agree, and the differing ones with both records.
async function replay() {
  const tally = new Map(); const differing = [];
  const add = (key, same, detail) => { const t = tally.get(key) ?? { frames: 0, same: 0 }; t.frames++; if (same) t.same++; else differing.push(detail); tally.set(key, t); };
  for (const run of runs) {
    const conns = connections(run); let handoff = null;
    for (const [cid, chunks] of conns) {
      if (!isBncs(chunks)) continue;
      const { records, out, seen, c2s } = await oursOverBncs(chunks);
      if (out) handoff = out.handoff;
      // Retail's own C>S frames: its echo of each ping, the realm it named in the realm logon.
      const retailEchoes = bncsFrames(stream(chunks, 'c2s')).filter((f) => f.id === 0x25).map((f) => hex(f.frame));
      const realmEx = c2s.find((f) => f.key === 0x3e);
      if (realmEx && out) assert.equal(seen.realm, cstr(realmEx.body, 24), `${run}/${cid}: the realm named in 0x3E`);
      const state = {};
      for (const { id, frame, ours } of records) {
        const retail = retailRecord(id, frame.body, state);
        if (id === 0x25) assert.equal(retail.echo, retailEchoes.shift(), `${run}/${cid}: retail's own echo frame`);
        const same = JSON.stringify(retail) === JSON.stringify(ours) || (retail.ignored && ours.notHandled);
        add(`bncs 0x${id.toString(16).padStart(2, '0')}`, same, { run, cid, id, retail, ours });
      }
    }
    for (const [cid, chunks] of conns) {
      if (!isMcp(chunks) || !handoff) continue;
      // Retail's MCP STARTUP is the hand-off it stored, less the IP and port: the second oracle for 0x3E.
      const startup = mcpFrames(stream(chunks, 'c2s').subarray(1)).find((f) => f.id === 0x01);
      const stored = Buffer.from(handoff.realmReply);
      assert.equal(hex(startup.body), hex(Buffer.concat([stored.subarray(0, 16), stored.subarray(24)])), `${run}/${cid}: retail's STARTUP is its stored 0x3E`);
      assert.equal(hex(handoff.startup), hex(startup.body.subarray(0, 64)), `${run}/${cid}: our startup block`);
      const { records, pending } = await oursOverMcp(chunks, handoff);
      const state = { pending };
      for (const { id, body, ours } of records) {
        const retail = retailMcpRecord(id, body, state);
        add(`mcp 0x${id.toString(16).padStart(2, '0')}`, JSON.stringify(retail) === JSON.stringify(ours), { run, cid, id, retail, ours, mcp: true });
      }
    }
  }
  return { tally, differing };
}

test('every recorded S>C BNCS and MCP frame: our handled record == the retail handler', { skip: !runs.length }, async () => {
  const { tally, differing } = await replay();
  const rows = [...tally].sort().map(([k, t]) => `${k} ${t.same}/${t.frames}`).join(', ');
  const designed = differing.filter((d) => !d.mcp && byDesign(d.id, d.retail)).length;
  console.log(`bnet-s2c replay (${runs.join(' ')}): ${rows}; differing ${differing.length - designed} (+${designed} by design)`);
  for (const d of differing) console.log(`  differs ${d.run}/${d.cid} ${d.mcp ? 'mcp' : 'bncs'} 0x${d.id.toString(16)}${byDesign(d.id, d.retail) ? ' (by design)' : ''}: retail ${JSON.stringify(d.retail)} ours ${JSON.stringify(d.ours)}`);
  assert.ok(tally.size > 0, 'the recording holds no server frames');
  assert.deepEqual(differing.filter((d) => d.mcp || !byDesign(d.id, d.retail)), []);
});

// The server bytes PvPGN sent the stock client in the recording, inlined: AUTH_CHECK passed, then SETEMAIL ahead of the logon result 0.
async function loginOver(replies) {
  let onData = () => {}; const sent = [];
  const transport = { isOpen: true, close() {}, onClose() {}, onData: (cb) => { onData = cb; },
    send: (b) => { sent.push(b[0] === 0xff ? b[1] : -1); const r = b[0] === 0xff && replies[b[1]]; if (r) queueMicrotask(() => onData(Uint8Array.from(Buffer.from(r, 'hex')))); return true; } };
  const protocol = { bncsSelector: () => Uint8Array.of(1), bncsFrame: (id, body) => Uint8Array.from([0xff, id, (body.length + 4) & 255, (body.length + 4) >> 8, ...body]),
    bncsAuthInfo: () => [], bncsAuthCheck: () => ({ body: [], clientToken: 1 }), bncsLogon: () => [], bncsQueryRealms: () => [], bncsRealmEx: () => [], tickCount: () => 1, realmToken: () => 1 };
  const authInfo = 'ff50650000000000' + '4cd7697a0d000000001168fcc26fdd01' + Buffer.from('IX86ver1.mpq\0A=1 B=2 C=3 4 A=A+S B=B-C C=C^A A=A^B\0').toString('hex');
  const total = authInfo.length / 2; const head = `ff50${(total & 255).toString(16).padStart(2, '0')}${(total >> 8).toString(16).padStart(2, '0')}`;
  replies[0x50] = head + authInfo.slice(8);
  const error = await bncsLogin(transport, protocol, { account: 'a', password: 'b', timeoutMs: 50 }).catch((e) => e);
  return { error, transport, sent };
}

test('SETEMAIL ahead of a passed logon raises the email-request flag (Bnclient+0x107C0), read once the logon passed', async () => {
  const { transport, sent } = await loginOver({ 0x51: 'ff5109000000000000', 0x3a: 'ff590400ff3a080000000000' });
  assert.equal(transport.bncsEmailRequested, true);
  assert.ok(sent.includes(0x40));
  const plain = await loginOver({ 0x51: 'ff5109000000000000', 0x3a: 'ff3a080000000000' });
  assert.equal(plain.transport.bncsEmailRequested, false);
});

test('AUTH_CHECK result 0x10 passes: Bnclient+0x11330 tests result & 0xFF0F, the 0xF0 bits pick the expansion wording', async () => {
  const { transport, sent } = await loginOver({ 0x51: 'ff5109001000000000', 0x3a: 'ff3a080000000000' });
  assert.equal(transport.bncsAuth?.versionResult, 0x10);
  assert.ok(sent.includes(0x3a));
});

test('the splitter follows the pump: resync to 0xFF, a frame needs its whole declared length', () => {
  const b = Uint8Array.from([0x00, 0xff, 0x25, 0x08, 0, 1, 2, 3, 4, 0xff, 0x51, 0x09, 0]);
  assert.deepEqual(bncsFrames(b).map((f) => [f.id, hex(f.body)]), [[0x25, '01020304']]);
});

test('retail 0x51: the expansion bit is not a refusal, 0x201 carries its owner, a body with an inner NUL is not handled', () => {
  assert.deepEqual(retailRecord(0x51, Uint8Array.from([0x10, 0, 0, 0, 0])), { result: 0x10, event: null });
  assert.deepEqual(retailRecord(0x51, Uint8Array.from([0x11, 2, 0, 0, 0x41, 0])), { result: 0x211, event: 10, owner: 'A' });
  assert.deepEqual(retailRecord(0x51, Uint8Array.from([0, 0, 0, 0, 0x41, 0, 0x41, 0])), { invalid: true });
});
