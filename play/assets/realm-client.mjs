// The realm CLIENT half: it drives our protocol (BNCS/MCP/D2GS) over a byte-pipe transport.
//
// Everything here is ORCHESTRATION and PARSING. The message BODIES are composed by the engine's own
// realm code (the same `src/core/realm` sources the server side and realmd use), reached through
// `protocol` -- so this file adds no second copy of a wire layout to drift. What it owns is the
// ORDER a client sends and expects them in, and the client-side read offsets (which the server never
// sees and so cannot check): serverToken at body offset 4, the MPQ name at 20, the JOIN ticket's
// network-order IP, the char-select statstring's class byte 13 / level byte 25.
//
// A `transport` is the byte pipe. A browser has only one usable one -- a WebSocket -- and a browser
// cannot host the realm itself (the locked decision: point at one, never host it), so the page dials
// a project-configured endpoint through `webSocketTransport` and the `tools/realmd/ws-bridge.mjs`
// bridge copies its bytes to the realm's TCP sockets verbatim. `loopbackTransport` is the same pipe
// over an in-process server session, which is how the test runner proves the client against the engine's
// own server with no socket and no mock.
//
// Prior art: the request/reply ORDER and read offsets are cross-checked against
// tristramd, d2-clientless and the diablo2-protocol schemas; this file re-uses the same references
// and the engine exports s113-s125 already prove byte-for-byte.

const CONNECT_TIMEOUT_MS = 8000;

const cat = (a, b) => (a.length ? Uint8Array.from([...a, ...b]) : (b instanceof Uint8Array ? b : Uint8Array.from(b)));
const hex = (u8) => [...u8].map((b) => b.toString(16).padStart(2, '0')).join('');

/**
 * A reader over a transport that ACCUMULATES. The obvious per-call reader is wrong on a socket: one
 * read can deliver a whole reply, so a caller that reads the header then starts over waiting for the
 * body blocks on bytes already in hand. `want` returns the first n bytes WITHOUT consuming, so a
 * caller can read a frame header and then the whole frame it describes, then `skip` it.
 */
function makeReader(transport) {
  let buf = new Uint8Array(0), closed = false;
  const waiters = [];
  const pump = () => {
    for (let i = waiters.length - 1; i >= 0; i--) {
      const w = waiters[i];
      if (buf.length >= w.n) { clearTimeout(w.t); waiters.splice(i, 1); w.resolve(buf.subarray(0, w.n)); }
      else if (closed) { clearTimeout(w.t); waiters.splice(i, 1); w.reject(new Error(`transport closed after ${buf.length}/${w.n} bytes`)); }
    }
  };
  transport.onData((d) => { buf = cat(buf, d); pump(); });
  transport.onClose(() => { closed = true; pump(); });
  return {
    want: (n, ms = CONNECT_TIMEOUT_MS) => new Promise((resolve, reject) => {
      const w = { n, resolve, reject, t: null };
      w.t = setTimeout(() => { const i = waiters.indexOf(w); if (i >= 0) waiters.splice(i, 1); reject(new Error(`timed out after ${buf.length}/${n} bytes`)); }, ms);
      waiters.push(w);
      pump();
    }),
    skip: (n) => { buf = buf.subarray(n); },
    buffered: () => buf,
  };
}

/** One BNCS frame: `[0xFF][id][len u16 LE]` + body. */
async function readBncsFrame(reader, timeoutMs) {
  const head = await reader.want(4, timeoutMs);
  if (head[0] !== 0xff) throw new Error(`reply is not a 0xFF BNCS frame (first byte 0x${head[0].toString(16)})`);
  const total = head[2] | (head[3] << 8);
  if (total < 4) throw new Error(`BNCS frame length ${total} is shorter than its header`);
  const frame = await reader.want(total, timeoutMs);
  reader.skip(total);
  return { id: frame[1], body: frame.subarray(4) };
}

const u32 = (b, o) => (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;

/**
 * The BNCS login: selector, AUTH_INFO, AUTH_CHECK, LOGONRESPONSE2, QUERYREALMS2, LOGONREALMEX.
 * Returns the challenge the server issued, the realm list it advertised, and the LOGONREALMEX
 * handoff (the MCP ticket) -- the read offsets the server cannot check for us.
 */
export async function bncsLogin(transport, protocol, { account, password, realm = 'd2-web', product = 'D2XP', timeoutMs = CONNECT_TIMEOUT_MS } = {}) {
  const reader = makeReader(transport);
  const sent = [], received = [];
  const send = (id, body) => { const frame = protocol.bncsFrame(id, body); sent.push({ id, bytes: frame.length }); transport.send(frame); };

  transport.send(protocol.bncsSelector());                       // single 0x01 protocol selector
  const clientToken = protocol.clientToken();
  send(0x50, protocol.bncsAuthInfo(product));

  const authInfo = await readBncsFrame(reader, timeoutMs); received.push(authInfo.id);
  if (authInfo.id !== 0x50) throw new Error(`expected AUTH_INFO reply 0x50, got 0x${authInfo.id.toString(16)}`);
  const serverToken = u32(authInfo.body, 4);
  const mpqEnd = authInfo.body.indexOf(0, 20);
  const mpq = authInfo.body.subarray(20, mpqEnd < 0 ? authInfo.body.length : mpqEnd);
  const challenge = authInfo.body.subarray(mpqEnd + 1);

  send(0x51, protocol.bncsAuthCheck({ clientToken, serverToken, challenge, product }));
  const authCheck = await readBncsFrame(reader, timeoutMs); received.push(authCheck.id);
  if (authCheck.id !== 0x51) throw new Error(`expected AUTH_CHECK reply 0x51, got 0x${authCheck.id.toString(16)}`);
  const versionResult = u32(authCheck.body, 0);
  if (versionResult !== 0) throw new Error(`AUTH_CHECK rejected the version (result 0x${versionResult.toString(16)})`);

  send(0x3a, protocol.bncsLogon({ clientToken, serverToken, account, password }));
  const logon = await readBncsFrame(reader, timeoutMs); received.push(logon.id);
  if (logon.id !== 0x3a) throw new Error(`expected LOGONRESPONSE2 reply 0x3a, got 0x${logon.id.toString(16)}`);
  const logonResult = u32(logon.body, 0);
  if (logonResult !== 0) throw new Error(`LOGONRESPONSE2 rejected the account (result ${logonResult})`);

  send(0x40, new Uint8Array(0));                                  // QUERYREALMS2, empty body
  const realms = await readBncsFrame(reader, timeoutMs); received.push(realms.id);
  if (realms.id !== 0x40) throw new Error(`expected QUERYREALMS2 reply 0x40, got 0x${realms.id.toString(16)}`);
  const count = u32(realms.body, 4);
  const realmList = [];
  let o = 8;
  for (let i = 0; i < count; i++) {
    o += 4;                                                       // per-realm unknown(u32=1)
    const titleEnd = realms.body.indexOf(0, o);
    const descriptionEnd = realms.body.indexOf(0, titleEnd + 1);
    realmList.push({ title: new TextDecoder().decode(realms.body.subarray(o, titleEnd)),
      description: new TextDecoder().decode(realms.body.subarray(titleEnd + 1, descriptionEnd)) });
    o = descriptionEnd + 1;
  }

  send(0x3e, protocol.bncsRealmEx({ clientToken, serverToken, realm, password }));
  const realmEx = await readBncsFrame(reader, timeoutMs); received.push(realmEx.id);
  if (realmEx.id !== 0x3e) throw new Error(`expected LOGONREALMEX reply 0x3e, got 0x${realmEx.id.toString(16)}`);
  // MCPStatus is 1 on success -- the value the engine's own server sends and the independent 1.13
  // schema pins (`test/realm-protodef.test.mjs`), NOT the 0 the other replies use.
  const status = u32(realmEx.body, 4);
  if (status !== 1) throw new Error(`LOGONREALMEX did not hand off the realm (MCPStatus 0x${status.toString(16)})`);
  // The 1.13 schema lays the body out as MCPCookie(0) MCPStatus(4) MCPChunk1 x2(8,12)
  // IP[4] bytes(16) port u16 BE(20) zero(22) MCPChunk2 x12(24..72) name; so the IP is the four BYTES
  // at 16, not the u32 at 8, and the port is a big-endian u16 at 20.
  const ip = [...realmEx.body.subarray(16, 20)].join('.');
  const port = ((realmEx.body[20] << 8) | realmEx.body[21]) & 0xffff;
  const handoff = { ip, port, cookie: u32(realmEx.body, 0), uuid: u32(realmEx.body, 12),
    // MCP_STARTUP forwards bytes [0,16) then [24,72) of this reply -- the block the realm hands over.
    // A plain array, so the value survives the CDP hop a scenario result crosses (a typed array does not).
    startup: [...cat(realmEx.body.subarray(0, 16), realmEx.body.subarray(24, 72))] };

  return { clientToken, serverToken, mpq: new TextDecoder().decode(mpq), challenge, versionResult, logonResult, realmList, handoff, sent, received };
}

/**
 * The D2GS direct-connect join -- the TCP/IP HOST/JOIN leg. The server greets UNPROMPTED (`AF 01`)
 * before the client says anything; the client then sends GAMELOGON and reads the game-entry burst,
 * which arrives Huffman-compressed and length-framed. `expectedBurst` is the opcode sequence the
 * caller requires (default the 0x03/0x59/0x5b join burst).
 */
export async function d2gsJoin(transport, protocol, { character, charClass, gameToken, gameHash, unitId = 1, expectedBurst = [0x03, 0x59, 0x5b], timeoutMs = CONNECT_TIMEOUT_MS } = {}) {
  const reader = makeReader(transport);
  // The greeting's size is decided by its first two bytes (`AF 00`/`AF 01` are 2 bytes, `AF 81`
  // carries a 128-byte inline table), so read the flag pair first and then the whole thing.
  const flag = await reader.want(2, timeoutMs);
  const greeting = protocol.greetingParse(flag);
  const greetingSize = greeting >> 8, mode = greeting & 0xff;
  if (!greetingSize) throw new Error(`not a D2GS greeting: ${hex(flag)}`);
  await reader.want(greetingSize, timeoutMs);
  reader.skip(greetingSize);

  const gamelogon = protocol.d2gsGamelogon({ character, charClass, gameToken, gameHash });
  transport.send(gamelogon);

  const response = await readD2gsFrame(reader, protocol, timeoutMs);
  // The burst is the server's join sequence (retail's own act-load / player-assign / player-in-game
  // schemas, the `src/core/realm/d2gs_packets.cpp` exports s110 pins). Here the CLIENT only has to show
  // it received them, in order and whole; s125 already proves the bytes the server composes. Walking
  // each variable-length body is a later slice (the player-in-game length is its own u16).
  const burstBytes = [...response];
  let at = 0;
  const opcodes = expectedBurst.map((op) => { const i = response.indexOf(op, at); if (i >= 0) at = i + 1; return i; });
  const missing = expectedBurst.filter((_, i) => opcodes[i] < 0);
  if (missing.length) throw new Error(`game-entry burst missing opcodes ${missing.map((o) => '0x' + o.toString(16))} (first bytes ${burstBytes.slice(0, 12).map((b) => '0x' + b.toString(16))})`);

  return { greeting: { size: greetingSize, mode }, burstBytes, opcodes, bytes: response.length, gamelogon: gamelogon.length };
}

/**
 * Bind the `protocol` primitives to the engine's realm exports. Every message BODY is composed by
 * `src/core/realm` through `Module.ccall` -- the same sources the server and realmd compile -- so the
 * client never re-implements a wire layout. Common to the test runner scenario and the page, so neither
 * can drift from the other. Call `protocol.free()` when done.
 */
export function bindEngineProtocol(Module, scratch = 65536) {
  const P = Module._malloc(scratch), O = Module._malloc(scratch), T = Module._malloc(4096), E = Module._malloc(1);
  const call = (n, rt, a, v) => Module.ccall(n, rt, a, v);
  const rd = (p, n) => Array.from(Module.HEAPU8.subarray(Number(p), Number(p) + n));
  const str = (bytes) => String.fromCharCode(...bytes.filter((b) => b !== 0));
  const protocol = {
    bncsSelector: () => [0x01],
    clientToken: () => 0xcafebabe,
    bncsFrame: (id, body) => { call('d2_realm_bncs_header', 'number', ['number', 'number', 'bigint'], [id, body.length, BigInt(T)]); return [...rd(T, 4), ...body]; },
    bncsAuthInfo: (product) => { const n = call('d2_realm_bncs_auth_info_request', 'number', ['string', 'bigint'], [product, BigInt(T)]); return rd(T, n); },
    bncsAuthCheck: ({ clientToken, challenge, product }) => {
      const challengeStr = str(challenge);
      const revLen = call('d2_realm_bncs_checkrev', 'number', ['string', 'string', 'number', 'bigint'], [challengeStr, '1.13.2.0', 0, BigInt(O)]);
      const checkrev = str(rd(O, revLen));
      const n = call('d2_realm_bncs_auth_check_request', 'number', ['number', 'number', 'number', 'number', 'bigint', 'number', 'string', 'string', 'bigint'], [clientToken, 0x0e, 0, 0, BigInt(E), 0, product + ';' + checkrev, 'd2-web', BigInt(T)]);
      return rd(T, n);
    },
    bncsLogon: ({ clientToken, serverToken, account, password }) => { const n = call('d2_realm_bncs_logonresponse2_request', 'number', ['number', 'number', 'string', 'string', 'bigint'], [clientToken, serverToken, account, password, BigInt(T)]); return rd(T, n); },
    bncsRealmEx: ({ clientToken, serverToken, realm, password }) => { const n = call('d2_realm_bncs_logonrealmex_request', 'number', ['number', 'number', 'string', 'string', 'bigint'], [clientToken, serverToken, password, realm, BigInt(T)]); return rd(T, n); },
    greetingParse: (bytes) => { Module.HEAPU8.set(bytes, Number(P)); return call('d2_realm_greeting_parse', 'number', ['bigint', 'number'], [BigInt(P), bytes.length]); },
    d2gsFrameSize: (bytes) => { Module.HEAPU8.set(bytes, Number(P)); return call('d2_realm_d2gs_frame_size', 'number', ['bigint', 'number'], [BigInt(P), bytes.length]); },
    d2gsGamelogon: ({ character, charClass, gameToken, gameHash }) => { const n = call('d2_realm_d2gs_gamelogon', 'number', ['number', 'number', 'number', 'number', 'string', 'bigint'], [gameHash, gameToken, charClass, 0x0e, character, BigInt(T)]); return rd(T, n); },
    d2gsDecompress: (bytes) => { Module.HEAPU8.set(bytes, Number(P)); const n = call('d2_realm_huffman_decompress', 'number', ['bigint', 'number', 'bigint', 'number'], [BigInt(P), bytes.length, BigInt(O), scratch]); if (n < 0) throw new Error('huffman decompress failed'); return rd(O, n); },
    free: () => { Module._free(P); Module._free(O); Module._free(T); Module._free(E); },
  };
  return protocol;
}

/** Read one server->client D2GS frame: length header, then the Huffman-compressed body. */
export async function readD2gsFrame(reader, protocol, timeoutMs = CONNECT_TIMEOUT_MS) {
  await reader.want(2, timeoutMs);
  let size = protocol.d2gsFrameSize(reader.buffered());
  if (!size) throw new Error('D2GS frame header present but the frame size is 0');
  const frame = await reader.want(size, timeoutMs);
  reader.skip(size);
  return protocol.d2gsDecompress(frame);
}

// --- transports ---------------------------------------------------------------------------------

/**
 * The browser's only usable transport: a WebSocket whose bytes the ws-bridge copies to the realm.
 * Binary frames both ways; a byte is a byte.
 */
export function webSocketTransport(url, WebSocketImpl = globalThis.WebSocket) {
  const ws = new WebSocketImpl(url);
  ws.binaryType = 'arraybuffer';
  const dataCbs = [], closeCbs = [], pending = [];
  ws.onmessage = (e) => {
    const d = new Uint8Array(e.data);
    if (!dataCbs.length) { pending.push(d); return; }
    for (const cb of dataCbs) cb(d);
  };
  ws.onclose = () => { for (const cb of closeCbs) cb(); };
  ws.onerror = () => { for (const cb of closeCbs) cb(); };
  return {
    // A plain JS array must become bytes: `ws.send([1])` stringifies to "1" (0x31 on the wire), which
    // desyncs the frame and is exactly the sort of quiet corruption this transport cannot afford.
    send: (bytes) => { if (ws.readyState === 1) ws.send(bytes instanceof Uint8Array ? bytes : Uint8Array.from(bytes)); },
    onData: (cb) => { dataCbs.push(cb); while (pending.length) cb(pending.shift()); },
    onClose: (cb) => closeCbs.push(cb),
    close: () => { try { ws.close(); } catch { /* already closing */ } },
    ready: () => new Promise((resolve, reject) => { ws.onopen = () => resolve(); ws.onerror = () => reject(new Error('websocket failed to open')); }),
  };
}

/**
 * The same pipe over an in-process server session -- no socket, no mock: `feed` is the engine's own
 * server (`d2_realm_*_session_feed`), so the client's bytes meet the real server behaviour. `serverSay`
 * injects what a server sends unprompted (the D2GS greeting).
 */
export function loopbackTransport(feed) {
  const dataCbs = [], closeCbs = [];
  // Buffer anything the server says before the client subscribes -- the D2GS greeting is emitted the
  // instant the socket opens, which on a real socket is before the client's own reader is attached.
  const pending = [];
  const emit = (bytes) => {
    if (!bytes || !bytes.length) return;
    const u = bytes instanceof Uint8Array ? bytes : Uint8Array.from(bytes);
    if (!dataCbs.length) { pending.push(u); return; }
    for (const cb of dataCbs) cb(u);
  };
  return {
    send: (bytes) => emit(feed(bytes instanceof Uint8Array ? bytes : Uint8Array.from(bytes))),
    onData: (cb) => { dataCbs.push(cb); while (pending.length) cb(pending.shift()); },
    onClose: (cb) => closeCbs.push(cb),
    close: () => { for (const cb of closeCbs) cb(); },
    serverSay: emit,
    ready: async () => {},
  };
}
