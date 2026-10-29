import { parseRealmMissiles } from './realm-missiles.mjs';
import { parseRealmMissileSoundEvents } from './realm-missile-sounds.mjs';
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
const fourcc = (value) => { let s = ''; for (let i = 0; i < 4; i++) { const c = (value >>> (i * 8)) & 0xff; if (c) s += String.fromCharCode(c); } return s.trim(); };

/**
 * A reader over a transport that ACCUMULATES. The obvious per-call reader is wrong on a socket: one
 * read can deliver a whole reply, so a caller that reads the header then starts over waiting for the
 * body blocks on bytes already in hand. `want` returns the first n bytes WITHOUT consuming, so a
 * caller can read a frame header and then the whole frame it describes, then `skip` it.
 */
function makeReader(transport) {
  let buf = new Uint8Array(0), closed = false, cancelled = false;
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
      if (ms !== null) w.t = setTimeout(() => { const i = waiters.indexOf(w); if (i >= 0) waiters.splice(i, 1); reject(new Error(`timed out after ${buf.length}/${n} bytes`)); }, ms);
      waiters.push(w);
      pump();
    }),
    skip: (n) => { buf = buf.subarray(n); },
    buffered: () => buf,
    get cancelled() { return cancelled; },
    cancel: () => {
      if (cancelled) return;
      cancelled = true;
      const error = new Error('D2GS reader cancelled');
      while (waiters.length) {
        const w = waiters.pop(); clearTimeout(w.t); w.reject(error);
      }
    },
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

/** One MCP frame: `[total:u16 LE][op:u8][body]`; total includes its own 3-byte header. */
async function readMcpFrame(reader, timeoutMs) {
  const head = await reader.want(3, timeoutMs);
  const total = head[0] | (head[1] << 8);
  if (total < 3) throw new Error(`MCP frame length ${total} is shorter than its header`);
  if (total > 8192) throw new Error(`MCP frame length ${total} exceeds the client buffer`);
  const frame = await reader.want(total, timeoutMs);
  reader.skip(total);
  return { op: frame[2], body: frame.subarray(3) };
}

function decodeMcpCharacters(body) {
  if (body.length < 8) throw new Error(`MCP CHARLIST2 body is short (${body.length} bytes)`);
  const total = u32(body, 2), returned = body[6] | (body[7] << 8);
  if (returned > total) throw new Error(`MCP CHARLIST2 returned ${returned} entries but declares ${total} total`);
  const characters = [];
  let at = 8;
  const readCString = (field) => {
    const end = body.indexOf(0, at);
    if (end < 0) throw new Error(`MCP CHARLIST2 ${field} is not terminated`);
    const value = body.subarray(at, end);
    at = end + 1;
    return value;
  };
  for (let i = 0; i < returned; i++) {
    if (at + 4 > body.length) throw new Error(`MCP CHARLIST2 entry ${i} has no expiry field`);
    const expiry = u32(body, at); at += 4;
    const rawName = readCString(`entry ${i} name`);
    const statstring = readCString(`entry ${i} statstring`);
    if (statstring.length < 26) throw new Error(`MCP CHARLIST2 entry ${i} statstring is short (${statstring.length} bytes)`);
    if (statstring[13] < 1 || statstring[13] > 7) throw new Error(`MCP CHARLIST2 entry ${i} has invalid class byte ${statstring[13]}`);
    characters.push({ name: new TextDecoder().decode(rawName), expiry,
      charClass: statstring[13] - 1, level: statstring[25] });
  }
  return { total, returned, characters };
}

/**
 * The MCP realm leg: selector -> STARTUP -> CHARLIST2 -> CHARLOGON -> CREATEGAME -> JOINGAME.
 * The startup bytes are the exact 64-byte block BNCS hands back. Wire bodies are composed through
 * the engine exports, as with BNCS and D2GS; this function owns only the sequence and read offsets.
 */
export async function mcpCreateAndJoin(transport, protocol, { realmReply, character, gameName, gamePassword = '',
  difficulty = 0, maxPlayers = 8, description = '', characterCount = 8, session = null, timeoutMs = CONNECT_TIMEOUT_MS } = {}) {
  if (!Array.isArray(realmReply) && !(realmReply instanceof Uint8Array)) throw new Error('MCP requires the BNCS realm handoff bytes');
  if (!character || !gameName) throw new Error('MCP requires a character and game name');
  const reader = session?.reader || makeReader(transport), sent = [], received = session ? [...session.received] : [];
  const send = (op, body) => {
    const frame = protocol.mcpFrame(op, body); sent.push({ op, bytes: frame.length }); transport.send(frame);
  };
  let list;
  if (session) {
    if (session.transport !== transport || session.protocol !== protocol || session.joined) throw new Error('MCP character session is not reusable');
    list = session.list;
  } else {
    transport.send(protocol.mcpSelector());
    send(0x01, protocol.mcpStartup(realmReply));
    const startup = await readMcpFrame(reader, timeoutMs); received.push(startup.op);
    if (startup.op !== 0x01) throw new Error(`expected MCP STARTUP reply 0x01, got 0x${startup.op.toString(16)}`);
    if (startup.body.length < 4 || u32(startup.body, 0) !== 0) throw new Error('MCP STARTUP was rejected');
    send(0x19, protocol.mcpCharlist(characterCount));
    const listReply = await readMcpFrame(reader, timeoutMs); received.push(listReply.op);
    if (listReply.op !== 0x19) throw new Error(`expected MCP CHARLIST2 reply 0x19, got 0x${listReply.op.toString(16)}`);
    list = decodeMcpCharacters(listReply.body);
  }
  const selected = list.characters.find((c) => c.name.toLowerCase() === character.toLowerCase());
  if (!selected) throw new Error(`character "${character}" is not in the MCP character list`);
  if (session?.characterLoggedOn && session.characterLoggedOn.toLowerCase() !== selected.name.toLowerCase())
    throw new Error(`MCP session is already logged onto "${session.characterLoggedOn}"; log in again to change characters`);

  if (!session?.characterLoggedOn) {
    send(0x07, protocol.mcpCharlogon(selected.name));
    const charlogon = await readMcpFrame(reader, timeoutMs); received.push(charlogon.op);
    if (charlogon.op !== 0x07) throw new Error(`expected MCP CHARLOGON reply 0x07, got 0x${charlogon.op.toString(16)}`);
    if (charlogon.body.length < 4 || u32(charlogon.body, 0) !== 0) throw new Error('MCP CHARLOGON was rejected');
    if (session) session.characterLoggedOn = selected.name;
  }

  const requestState = session || { nextRequestId: 2 };
  const createId = requestState.nextRequestId; requestState.nextRequestId = (createId + 1) & 0xffff;
  send(0x03, protocol.mcpCreateGame({ requestId: createId, difficulty, maxPlayers,
    name: titleCaseGameText(gameName), password: titleCaseGameText(gamePassword), description }));
  const created = await readMcpFrame(reader, timeoutMs); received.push(created.op);
  if (created.op !== 0x03) throw new Error(`expected MCP CREATEGAME reply 0x03, got 0x${created.op.toString(16)}`);
  if (created.body.length < 10 || u16(created.body, 0) !== createId) throw new Error(`MCP CREATEGAME reply id ${u16(created.body, 0)} does not match request ${createId}`);
  if (u32(created.body, 6) !== 0) throw new Error('MCP CREATEGAME was rejected');
  const gameToken = created.body[2] | (created.body[3] << 8);

  const joinId = requestState.nextRequestId; requestState.nextRequestId = (joinId + 1) & 0xffff;
  send(0x04, protocol.mcpJoinGame({ requestId: joinId, name: titleCaseGameText(gameName), password: titleCaseGameText(gamePassword) }));
  const joined = await readMcpFrame(reader, timeoutMs); received.push(joined.op);
  if (joined.op !== 0x04) throw new Error(`expected MCP JOINGAME reply 0x04, got 0x${joined.op.toString(16)}`);
  if (joined.body.length < 18 || u16(joined.body, 0) !== joinId) throw new Error(`MCP JOINGAME reply id ${u16(joined.body, 0)} does not match request ${joinId}`);
  if (u32(joined.body, 14) !== 0) throw new Error('MCP JOINGAME was rejected');
  const token = joined.body[2] | (joined.body[3] << 8);
  if (token !== gameToken) throw new Error(`MCP JOINGAME token ${token} does not match CREATEGAME token ${gameToken}`);
  const ip = [...joined.body.subarray(6, 10)].join('.');
  const hash = u32(joined.body, 10);
  if (session) session.joined = true;
  return { character: selected, game: { name: titleCaseGameText(gameName), createToken: gameToken, difficulty }, ticket: { ip, token, hash }, sent, received };
}

const u16 = (b, o) => (b[o] | (b[o + 1] << 8)) >>> 0;
function titleCaseGameText(value) {
  return String(value).replace(/(^| )([A-Za-z])([A-Za-z]*)/g,
    (_, space, first, rest) => space + first.toUpperCase() + rest.toLowerCase());
}

/** Open the MCP login leg far enough to populate the character picker. */
export async function mcpListCharacters(transport, protocol, { realmReply, characterCount = 8, timeoutMs = CONNECT_TIMEOUT_MS } = {}) {
  if (!Array.isArray(realmReply) && !(realmReply instanceof Uint8Array)) throw new Error('MCP requires the BNCS realm handoff bytes');
  const reader = makeReader(transport), received = [];
  const send = (op, body) => transport.send(protocol.mcpFrame(op, body));
  transport.send(protocol.mcpSelector());
  send(0x01, protocol.mcpStartup(realmReply));
  const startup = await readMcpFrame(reader, timeoutMs); received.push(startup.op);
  if (startup.op !== 0x01 || startup.body.length < 4 || u32(startup.body, 0) !== 0) throw new Error('MCP STARTUP was rejected');
  send(0x19, protocol.mcpCharlist(characterCount));
  const reply = await readMcpFrame(reader, timeoutMs); received.push(reply.op);
  if (reply.op !== 0x19) throw new Error(`expected MCP CHARLIST2 reply 0x19, got 0x${reply.op.toString(16)}`);
  const list = decodeMcpCharacters(reply.body);
  return { ...list, received, session: { reader, transport, protocol, list, nextRequestId: 2,
    characterLoggedOn: false, joined: false,
    received: [...received] } };
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
    realmReply: [...realmEx.body],
    // MCP_STARTUP forwards bytes [0,16) then [24,72) of this reply -- the block the realm hands over.
    // A plain array, so the value survives the CDP hop a scenario result crosses (a typed array does not).
    startup: [...cat(realmEx.body.subarray(0, 16), realmEx.body.subarray(24, 72))] };

  return { clientToken, serverToken, mpq: new TextDecoder().decode(mpq), challenge, versionResult, logonResult, realmList, handoff, sent, received };
}

/**
 * The D2GS direct-connect join -- the TCP/IP HOST/JOIN leg. The server greets UNPROMPTED (`AF 01`)
 * before the client says anything; the client then sends GAMELOGON and reads the game-entry burst,
 * which arrives Huffman-compressed and length-framed. The engine's 1.13c packet table frames the
 * decoded stream; this routine waits until a complete player assignment is available.
 */
export async function d2gsJoin(transport, protocol, { character, charClass, gameToken, gameHash, difficulty = 0, timeoutMs = CONNECT_TIMEOUT_MS } = {}) {
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

  const decoded = [];
  let frameCount = 0, assignOffset = -1, handshakeEnd = -1, playerAssignment = null;
  let receiptScanAt = 0;
  const joinReassignments = [];
  if (typeof protocol.d2gsFindPlayerAssign !== 'function') throw new Error('engine D2GS packet framing is unavailable');
  if (typeof protocol.d2gsClientPlayerFeed !== 'function') throw new Error('engine D2GS client state is unavailable');
  protocol.d2gsClientPlayerReset?.();
  protocol.d2gsClientSetDifficulty?.(difficulty);
  while (handshakeEnd < 0) {
    const frame = await readD2gsFrame(reader, protocol, timeoutMs);
    decoded.push(...frame);
    frameCount++;
    if (decoded.length > 65536 || frameCount > 32) throw new Error('D2GS join stream exceeded the player selection scan limit');
    const response = Uint8Array.from(decoded);
    // Observe complete packets as soon as the decoded receive stream contains them. The engine
    // roster is installed only after the matching 0x0B below, so a selected-player 0x15 during
    // join cannot be inferred later from the already-updated roster.
    while (receiptScanAt < response.length) {
      const packet = response.subarray(receiptScanAt);
      const packetLength = protocol.d2gsPacketSize(packet);
      if (packetLength < 0) throw new Error('D2GS join stream contains an invalid 1.13c server packet');
      if (packetLength === 0) break;
      if (packet[0] === 0x15) {
        const reassignment = protocol.d2gsPlayerReassign?.(packet.subarray(0, packetLength));
        if (reassignment) joinReassignments.push({ offset: receiptScanAt, update: reassignment });
      }
      receiptScanAt += packetLength;
    }
    const scan = protocol.d2gsFindPlayerAssign(response);
    if (scan.status === -2) throw new Error('D2GS join stream has an unknown or invalid 1.13c server packet');
    if (scan.status !== 1) continue;
    assignOffset = scan.offset;
    if (assignOffset + 26 > response.length) throw new Error('D2GS assignment scan returned a truncated packet');
    playerAssignment = protocol.d2gsPlayerAssign?.(response.subarray(assignOffset, assignOffset + 26)) ?? null;
    if (!playerAssignment) throw new Error('D2GS player assignment 0x59 failed engine validation');
    if (playerAssignment.name.toLowerCase() !== String(character).toLowerCase())
      throw new Error(`D2GS assigned player "${playerAssignment.name}" but MCP selected "${character}"`);
    if (playerAssignment.classId !== charClass)
      throw new Error(`D2GS assigned class ${playerAssignment.classId} but MCP selected class ${charClass}`);

    // The native join sender constructs the player's 0x59 CUnit, then selects that already
    // created unit with the type/GUID 0x0B Handshake. Walk complete engine-framed packets until
    // that exact handshake is present; it may arrive in a later Huffman frame than the assignment.
    for (let at = 0; at < response.length;) {
      const packetLength = protocol.d2gsPacketSize(response.subarray(at));
      if (packetLength < 0) throw new Error('D2GS join stream contains an invalid 1.13c server packet');
      if (packetLength === 0) break;
      if (at > assignOffset && response[at] === 0x0b && packetLength === 6 && response[at + 1] === 0) {
        const guid = (response[at + 2] | (response[at + 3] << 8) | (response[at + 4] << 16) | (response[at + 5] << 24)) >>> 0;
        if (guid === playerAssignment.guid) {
          handshakeEnd = at + packetLength;
          break;
        }
      }
      at += packetLength;
    }
  }
  const response = Uint8Array.from(decoded);
  // Keep packet offsets from the engine table for transport evidence, without assuming any order.
  const opcodes = [];
  for (let at = 0; at < decoded.length;) {
    const size = protocol.d2gsPacketSize(response.subarray(at));
    if (size < 0) throw new Error('D2GS join stream contains an invalid 1.13c server packet');
    if (size === 0) break;
    opcodes.push(at);
    at += size;
  }
  const applied = protocol.d2gsClientPlayerFeed(response.subarray(0, handshakeEnd));
  if (applied < 1) throw new Error(`D2GS engine did not parse the assigned player before selection (applied ${applied})`);
  const playerState = protocol.d2gsClientPlayerState?.();
  if (!playerState?.active || playerState.guid !== playerAssignment.guid)
    throw new Error('D2GS engine did not select the assigned player after the 0x0B Handshake');
  const worldState = protocol.d2gsClientWorldState?.() ?? null;
  const transitionFade = joinReassignments.some(({ offset, update }) => offset < handshakeEnd &&
    update.unitType === 0 && update.guid === playerAssignment.guid && (update.value & 0xff) !== 0);

  const result = { greeting: { size: greetingSize, mode }, burstBytes: [...response], opcodes, bytes: response.length,
    playerAssignment, playerState, worldState, transitionFade, gamelogon: gamelogon.length, gamelogonBytes: Array.from(gamelogon) };
  const tail = response.subarray(handshakeEnd);
  const pending = tail.length ? [tail] : [];
  const readFrame = async () => pending.length ? pending.shift() : readD2gsFrame(reader, protocol, null);
  readFrame.cancel = () => reader.cancel();
  // D2GS remains a continuous stream after the entry burst; reuse this reader for later frames.
  // Keep the method non-enumerable so browser-harness evidence stays plain JSON data.
  Object.defineProperties(result, {
    readFrame: { value: readFrame },
  });
  return result;
}

/** Retail's 13-byte D2GS keep-alive, sent every five seconds while the game socket is retained. */
export function startD2gsKeepAlive(transport, protocol, { intervalMs = 5000, now = () => Date.now(),
  setIntervalFn = setInterval, clearIntervalFn = clearInterval, onSent = () => {} } = {}) {
  const started = now();
  let active = true, sent = 0;
  let timer;
  const stop = () => {
    if (!active) return;
    active = false;
    clearIntervalFn(timer);
  };
  timer = setIntervalFn(() => {
    if (!active) return;
    const tick = (now() - started) >>> 0;
    transport.send(protocol.d2gsPing(tick));
    sent++;
    onSent(sent, tick);
  }, intervalMs);
  transport.onClose?.(stop);
  return { stop, get active() { return active; }, get sent() { return sent; } };
}

/** Decode later server frames continuously so an idle socket cannot grow the reader buffer. */
export function startD2gsReceiveLoop(readFrame, { onFrame = () => {}, onClose = () => {} } = {}) {
  let active = true, frames = 0;
  const done = (async () => {
    while (active) {
      try {
        const rawPayload = await readFrame();
        if (!active) break;
        const payload = rawPayload instanceof Uint8Array ? rawPayload : Uint8Array.from(rawPayload);
        frames++;
        onFrame({ count: frames, byteLength: payload.length, prefix: Array.from(payload.subarray(0, 8)) }, payload);
      } catch (error) {
        if (active) { active = false; onClose(error); }
        break;
      }
    }
  })();
  return { stop() { if (!active) return; active = false; readFrame.cancel?.(); }, done, get active() { return active; }, get frames() { return frames; } };
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
  const readPlayerPointer = (ptr) => {
    if (!ptr) return null;
    const p = Number(ptr), view = new DataView(Module.HEAPU8.buffer, p, 92);
    if (view.getUint32(0, true) !== 5 || view.getUint32(4, true) !== 92) return null;
    const rawName = Module.HEAPU8.slice(p + 32, p + 48), end = rawName.indexOf(0);
    return { active: Boolean(view.getUint32(8, true)), guid: view.getUint32(12, true),
      x: view.getUint32(16, true), y: view.getUint32(20, true), classId: view.getUint8(24),
      reassignValue: view.getUint8(25), reassignments: view.getUint32(28, true),
      name: new TextDecoder().decode(rawName.subarray(0, end < 0 ? rawName.length : end)),
      clientRoomActive: Boolean(view.getUint32(48, true)), clientRoomLevel: view.getUint32(52, true),
      clientRoomTileX: view.getUint32(56, true), clientRoomTileY: view.getUint32(60, true),
      cunitInitSeed: view.getUint32(64, true), cunitInitSeedValid: Boolean(view.getUint32(68, true)),
      cunitRngLow: view.getUint32(72, true), cunitRngHigh: view.getUint32(76, true),
      cunitRngValid: Boolean(view.getUint32(80, true)), initialMode: view.getUint32(84, true),
      initialDirection: view.getUint32(88, true) };
  };
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
    mcpSelector: () => { const n = call('d2_realm_mcp_protocol_selector', 'number', ['bigint'], [BigInt(T)]); return rd(T, n); },
    mcpFrame: (op, body) => { const n = call('d2_realm_mcp_header', 'number', ['number', 'number', 'bigint'], [op, body.length, BigInt(T)]); if (n < 3) throw new Error('MCP frame composer rejected the body'); return [...rd(T, 3), ...body]; },
    mcpStartup: (realmReply) => { Module.HEAPU8.set(realmReply, Number(P)); const n = call('d2_realm_mcp_startup_request', 'number', ['bigint', 'number', 'bigint'], [BigInt(P), realmReply.length, BigInt(T)]); return rd(T, n); },
    mcpCharlist: (count) => { const n = call('d2_realm_mcp_charlist2_request', 'number', ['number', 'bigint'], [count, BigInt(T)]); return rd(T, n); },
    mcpCharlogon: (name) => { const n = call('d2_realm_mcp_charlogon_request', 'number', ['string', 'bigint'], [name, BigInt(T)]); return rd(T, n); },
    mcpCreateGame: ({ requestId, difficulty, maxPlayers, name, password, description }) => { const n = call('d2_realm_mcp_creategame_request', 'number', ['number', 'number', 'number', 'string', 'string', 'string', 'bigint'], [requestId, difficulty, maxPlayers, name, password, description, BigInt(T)]); return rd(T, n); },
    mcpJoinGame: ({ requestId, name, password }) => { const n = call('d2_realm_mcp_joingame_request', 'number', ['number', 'string', 'string', 'bigint'], [requestId, name, password, BigInt(T)]); return rd(T, n); },
    greetingParse: (bytes) => { Module.HEAPU8.set(bytes, Number(P)); return call('d2_realm_greeting_parse', 'number', ['bigint', 'number'], [BigInt(P), bytes.length]); },
    d2gsFrameSize: (bytes) => { Module.HEAPU8.set(bytes, Number(P)); return call('d2_realm_d2gs_frame_size', 'number', ['bigint', 'number'], [BigInt(P), bytes.length]); },
    d2gsPacketSize: (bytes) => { Module.HEAPU8.set(bytes, Number(P)); return call('d2_realm_d2gs_packet_size', 'number', ['bigint', 'number'], [BigInt(P), bytes.length]); },
    d2gsFindPlayerAssign: (bytes) => {
      Module.HEAPU8.set(bytes, Number(P));
      const status = call('d2_realm_d2gs_find_player_assign', 'number', ['bigint', 'number', 'bigint'], [BigInt(P), bytes.length, BigInt(O)]);
      return { status, offset: status === 1 ? Module.HEAP32[Number(O) >> 2] : -1 };
    },
    d2gsPlayerAssign: (bytes) => {
      Module.HEAPU8.set(bytes, Number(P));
      const n = call('d2_realm_d2gs_parse_player_assign', 'number', ['bigint', 'number', 'bigint'], [BigInt(P), bytes.length, BigInt(O)]);
      if (n !== 26) return null;
      const record = rd(O, n), rawName = record.slice(5, 21), end = rawName.indexOf(0);
      return { guid: (record[0] | (record[1] << 8) | (record[2] << 16) | (record[3] << 24)) >>> 0,
        classId: record[4], name: new TextDecoder().decode(Uint8Array.from(rawName.slice(0, end < 0 ? rawName.length : end))),
        x: record[22] | (record[23] << 8), y: record[24] | (record[25] << 8) };
    },
    d2gsPlayerReassign: (bytes) => {
      Module.HEAPU8.set(bytes, Number(P));
      const n = call('d2_realm_d2gs_parse_player_reassign', 'number', ['bigint', 'number', 'bigint'], [BigInt(P), bytes.length, BigInt(O)]);
      if (n !== 11) return null;
      const record = rd(O, 12);
      return { guid: (record[0] | (record[1] << 8) | (record[2] << 16) | (record[3] << 24)) >>> 0,
        x: record[4] | (record[5] << 8), y: record[6] | (record[7] << 8),
        unitType: record[8], value: record[9] };
    },
    d2gsClientPlayerReset: () => call('d2_realm_client_player_reset', null, [], []),
    d2gsClientSetDifficulty: (difficulty) => call('d2_realm_client_set_difficulty', null, ['number'], [difficulty]),
    d2gsClientPlayerFeed: (bytes) => {
      Module.HEAPU8.set(bytes, Number(P));
      return call('d2_realm_client_player_feed', 'number', ['bigint', 'number'], [BigInt(P), bytes.length]);
    },
    d2gsClientPlayerState: () => {
      const ptr = call('d2_realm_client_player_state', 'bigint', [], []);
      const player = readPlayerPointer(ptr);
      // The selected player is one of the roster entries. The 92-byte assignment view stays as it
      // is, but the same live motion the roster exposes must travel with it so a server mode change
      // reaches the selected player's draw instead of the assignment's fixed initialMode.
      if (player?.active) {
        const count = call('d2_realm_client_player_count', 'number', [], []);
        for (let index = 0; index < count; index++) {
          const other = readPlayerPointer(call('d2_realm_client_player_at', 'bigint', ['number'], [index]));
          if (other?.guid === player.guid) { player.motion = protocol.d2gsClientPlayerMotionAt(index); break; }
        }
      }
      return player;
    },
    d2gsClientGameChat: () => {
      const ptr = call('d2_realm_client_game_chat', 'bigint', [], []);
      if (!ptr) return null;
      const at = Number(ptr), view = new DataView(Module.HEAPU8.buffer, at, 312);
      if (view.getUint32(0, true) !== 1 || view.getUint32(4, true) !== 312) return null;
      const nameLength = view.getUint32(32, true), messageLength = view.getUint32(36, true);
      if (nameLength > 15 || messageLength > 255) return null;
      return { messageType: view.getUint32(8, true), unknown2: view.getUint32(12, true),
        subType: view.getUint32(16, true), unitGuid: view.getUint32(20, true),
        unknown8: view.getUint32(24, true), color: view.getUint32(28, true),
        name: Array.from(Module.HEAPU8.subarray(at + 40, at + 40 + nameLength)),
        message: Array.from(Module.HEAPU8.subarray(at + 56, at + 56 + messageLength)) };
    },
    d2gsClientGameChatCount: () => call('d2_realm_client_game_chat_count', 'number', [], []),
    d2gsClientPlayerCount: () => call('d2_realm_client_player_count', 'number', [], []),
    d2gsClientPlayerAt: (index) => {
      const ptr = call('d2_realm_client_player_at', 'bigint', ['number'], [index]);
      const player = readPlayerPointer(ptr);
      if (player?.active) player.motion = protocol.d2gsClientPlayerMotionAt(index);
      return player;
    },
    d2gsClientPlayerMotionAt: (index) => {
      const ptr = call('d2_realm_client_player_motion_at', 'bigint', ['number'], [index]);
      if (!ptr) return null;
      const view = new DataView(Module.HEAPU8.buffer, Number(ptr), 48);
      if (view.getUint32(0, true) !== 1 || view.getUint32(4, true) !== 48) return null;
      return { active: Boolean(view.getUint32(8, true)), mode: view.getUint32(12, true),
        direction: view.getUint32(16, true), framePrecise: view.getUint32(20, true),
        frameCountPrecise: view.getUint32(24, true), animSpeed: view.getUint32(28, true),
        positionHintX: view.getUint32(32, true), positionHintY: view.getUint32(36, true),
        seqMode: view.getUint32(40, true), seqFrame: view.getUint32(44, true),
        usedSkill: call('d2_realm_client_player_used_skill_at', 'number', ['number'], [index]) };
    },
    d2gsClientPlayers: () => {
      const count = call('d2_realm_client_player_count', 'number', [], []);
      const players = [];
      for (let index = 0; index < count; index++) {
        const ptr = call('d2_realm_client_player_at', 'bigint', ['number'], [index]);
        const player = readPlayerPointer(ptr);
        if (player?.active) {
          player.motion = protocol.d2gsClientPlayerMotionAt(index);
          players.push(player);
        }
      }
      return players;
    },
    d2gsClientWorldState: () => {
      const ptr = call('d2_realm_client_world_state', 'bigint', [], []);
      if (!ptr) return null;
      const view = new DataView(Module.HEAPU8.buffer, Number(ptr), 72);
      if (view.getUint32(0, true) !== 2 || view.getUint32(4, true) !== 72) return null;
      return { hasLoadAct: Boolean(view.getUint32(8, true)), act: view.getUint32(12, true),
        mapSeed: view.getUint32(16, true), area: view.getUint32(20, true),
        rawLoadActTail: view.getUint32(24, true), loadCompleteEvent: Boolean(view.getUint32(28, true)),
        loadActCount: view.getUint32(32, true), unloadCompleteEvent: Boolean(view.getUint32(36, true)),
        hasMapReveal: Boolean(view.getUint32(40, true)), mapLevel: view.getUint32(44, true),
        mapX: view.getUint32(48, true), mapY: view.getUint32(52, true), mapRevealCount: view.getUint32(56, true),
        difficulty: view.getUint32(60, true), mapHideCount: view.getUint32(64, true), mapRevision: view.getUint32(68, true) };
    },
    d2gsClientMonsters: () => {
      const count = call('d2_realm_client_monster_count', 'number', [], []);
      let modeEvents = [];
      if (Module._d2_realm_client_monster_death_mode_event_count && Module._d2_realm_client_monster_death_mode_events) {
        const eventCount = call('d2_realm_client_monster_death_mode_event_count', 'number', [], []);
        if (eventCount) {
          const eventPtr = Number(call('d2_realm_client_monster_death_mode_events', 'bigint', [], []));
          if (!eventPtr) throw new Error('engine returned no monster death-mode events');
          const eventView = new DataView(Module.HEAPU8.buffer, eventPtr, eventCount * 32);
          modeEvents = Array.from({ length: eventCount }, (_, row) => {
            const at = row * 32;
            if (eventView.getUint32(at, true) !== 1 || eventView.getUint32(at + 4, true) !== 32)
              throw new Error(`invalid monster death-mode event ${row}`);
            return { sequence: eventView.getUint32(at + 8, true), guid: eventView.getUint32(at + 12, true),
              packetOpcode: eventView.getUint32(at + 16, true), wireState: eventView.getUint32(at + 20, true),
              fromMode: eventView.getUint32(at + 24, true), toMode: eventView.getUint32(at + 28, true) };
          });
        }
      }
      if (!count) return [];
      const ptr = call('d2_realm_client_monsters', 'bigint', [], []);
      if (!ptr) throw new Error('engine returned no remote-monster records');
      const p = Number(ptr), view = new DataView(Module.HEAPU8.buffer, p, count * 116);
      const eventsByGuid = new Map();
      for (const event of modeEvents) {
        const events = eventsByGuid.get(event.guid) ?? [];
        events.push(event);
        eventsByGuid.set(event.guid, events);
      }
      return Array.from({ length: count }, (_, row) => {
        const at = row * 116;
        if (view.getUint32(at, true) !== 1 || view.getUint32(at + 4, true) !== 116)
          throw new Error(`invalid remote-monster record ${row}`);
        return { guid: view.getUint32(at + 8, true), classId: view.getUint32(at + 12, true),
          code: fourcc(view.getUint32(at + 16, true)), mode: view.getUint32(at + 20, true),
          framePrecise: view.getUint32(at + 24, true), frameCountPrecise: view.getUint32(at + 28, true),
          components: Array.from({ length: 16 }, (_, i) => fourcc(view.getUint32(at + 32 + i * 4, true))),
          animSpeed: view.getUint16(at + 96, true), direction: view.getUint8(at + 98),
          animationFlags: view.getUint8(at + 99), modeEvents: eventsByGuid.get(view.getUint32(at + 8, true)) ?? [],
          x: view.getUint32(at + 100, true), y: view.getUint32(at + 104, true),
          hpPct: view.getUint32(at + 108, true), typeFlags: view.getUint32(at + 112, true) };
      });
    },
    d2gsClientMissiles: () => {
      const count = call('d2_realm_client_missile_count', 'number', [], []);
      if (!count) return [];
      const ptr = Number(call('d2_realm_client_missiles', 'bigint', [], []));
      if (!ptr) throw new Error('engine returned no client missile records');
      return parseRealmMissiles(Module.HEAPU8.subarray(ptr, ptr + count * 76), count);
    },
    d2gsClientPredictRightCastOnMonster: (guid) => call('d2_realm_client_player_predict_right_cast_on_monster', 'number', ['number'], [guid]),
    d2gsClientTakeMissileSounds: () => {
      const cap = 128;
      const count = call('d2_realm_client_take_missile_sounds', 'number', ['bigint','number'], [BigInt(T),cap]);
      if (count < 0 || count > cap) throw new Error(`invalid client missile sound count ${count}`);
      return parseRealmMissileSoundEvents(Module.HEAPU8.subarray(T, T + count * 24), count);
    },
    d2gsClientProjectileSupported: (skillId) => call('d2_realm_client_projectile_supported', 'number', ['number'], [skillId]),
    d2gsClientPredictRightCast: (x,y) => call('d2_realm_client_player_predict_right_cast', 'number', ['number','number'], [x,y]),
    d2gsClientMotionTick: () => call('d2_realm_client_motion_tick', 'number', [], []),
    d2gsGamelogon: ({ character, charClass, gameToken, gameHash }) => { const n = call('d2_realm_d2gs_gamelogon', 'number', ['number', 'number', 'number', 'number', 'string', 'bigint'], [gameHash, gameToken, charClass, 0x0e, character, BigInt(T)]); return rd(T, n); },
    d2gsPing: (tick) => { const n = call('d2_realm_d2gs_ping', 'number', ['number', 'bigint'], [tick, BigInt(T)]); if (n !== 13) throw new Error(`D2GS ping composer returned ${n} bytes`); return rd(T, n); },
    d2gsSelectedSkills: () => {
      const p = Number(call('d2_realm_client_selected_skills', 'bigint', [], []));
      if (!p) throw new Error('engine returned no selected skill record');
      const view = new DataView(Module.HEAPU8.buffer, p, 16);
      return {left:view.getInt32(0,true), right:view.getInt32(4,true),
        leftOwner:view.getInt32(8,true), rightOwner:view.getInt32(12,true)};
    },
    d2gsSkillChoices: (guid, cap = 128) => {
      if (!Number.isInteger(guid) || guid < 0 || guid > 0xffffffff) throw new RangeError('invalid D2GS player GUID');
      if (!Number.isInteger(cap) || cap < 0 || cap > 128) throw new RangeError('invalid D2GS skill choice capacity');
      const count = call('d2_realm_client_skill_choices', 'number', ['number','bigint','number'], [guid >>> 0, BigInt(T), cap]);
      if (!Number.isInteger(count) || count < 0 || count > cap) throw new Error(`engine returned invalid D2GS skill choice count ${count}`);
      return Uint8Array.from(rd(T, count * 8));
    },
    d2gsSelectSkill: (skill, left, owner = 0xffffffff) => {
      if (!Number.isInteger(skill) || skill < 0 || skill > 0xffff || typeof left !== 'boolean' ||
          !Number.isInteger(owner) || owner < -1 || owner > 0xffffffff)
        throw new RangeError('invalid D2GS skill selection');
      const n = call('d2_realm_d2gs_select_skill', 'number', ['number','number','number','bigint'], [skill,left?1:0,owner>>>0,BigInt(T)]);
      if (n !== 9) throw new Error(`D2GS skill selection composer returned ${n} bytes`);
      return Uint8Array.from(rd(T,n));
    },
    d2gsMoveTo: (opcode, x, y) => {
      if (!Number.isInteger(x) || !Number.isInteger(y) || x < 0 || y < 0 || x > 0xffff || y > 0xffff)
        throw new RangeError('D2GS walk destination must fit unsigned 16-bit subtiles');
      const n = call('d2_realm_d2gs_move_to', 'number', ['number', 'number', 'number', 'bigint'], [opcode, x, y, BigInt(T)]);
      if (n !== 5) throw new Error(`D2GS movement composer rejected opcode ${opcode}`);
      return Uint8Array.from(rd(T, n));
    },
    d2gsRightSkillAt: (held, x, y) => {
      if (!Number.isInteger(x) || !Number.isInteger(y) || x < 0 || y < 0 || x > 0xffff || y > 0xffff)
        throw new RangeError('D2GS skill destination must fit unsigned 16-bit subtiles');
      const opcode = held ? 0x0f : 0x0c;
      const n = call('d2_realm_d2gs_right_skill_at', 'number', ['number', 'number', 'number', 'bigint'], [opcode, x, y, BigInt(T)]);
      if (n !== 5) throw new Error(`D2GS skill composer rejected opcode ${opcode}`);
      return Uint8Array.from(rd(T, n));
    },
    d2gsRightSkillOnEntity: (held, unitType, guid) => {
      if (unitType !== 1 || !Number.isInteger(guid) || guid < 0 || guid > 0xffffffff)
        throw new RangeError('D2GS unit-target skill requires a monster GUID');
      const opcode = held ? 0x10 : 0x0d;
      const n = call('d2_realm_d2gs_right_skill_on_entity', 'number',
        ['number', 'number', 'number', 'bigint'], [opcode, unitType, guid, BigInt(T)]);
      if (n !== 9) throw new Error(`D2GS unit-target skill composer rejected opcode ${opcode}`);
      return Uint8Array.from(rd(T, n));
    },
    d2gsDecompress: (bytes) => { Module.HEAPU8.set(bytes, Number(P)); const n = call('d2_realm_huffman_decompress', 'number', ['bigint', 'number', 'bigint', 'number'], [BigInt(P), bytes.length, BigInt(O), scratch]); if (n < 0) throw new Error('huffman decompress failed'); return rd(O, n); },
    free: () => { Module._free(P); Module._free(O); Module._free(T); Module._free(E); },
  };
  return protocol;
}

/** Read one server->client D2GS frame: length header, then the Huffman-compressed body. */
export async function readD2gsFrame(reader, protocol, timeoutMs = CONNECT_TIMEOUT_MS) {
  await reader.want(2, timeoutMs);
  if (reader.cancelled) throw new Error('D2GS reader cancelled');
  let size = protocol.d2gsFrameSize(reader.buffered());
  if (!size) throw new Error('D2GS frame header present but the frame size is 0');
  const frame = await reader.want(size, timeoutMs);
  if (reader.cancelled) throw new Error('D2GS reader cancelled');
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
    send: (bytes) => { if (ws.readyState !== 1) return false; ws.send(bytes instanceof Uint8Array ? bytes : Uint8Array.from(bytes)); return true; },
    get isOpen() { return ws.readyState === 1; },
    onData: (cb) => { dataCbs.push(cb); while (pending.length) cb(pending.shift()); },
    onClose: (cb) => closeCbs.push(cb),
    close: () => { try { ws.close(); } catch { /* already closing */ } },
    ready: () => new Promise((resolve, reject) => {
      // A TCP refusal is commonly delivered as `close` without a useful `error` event. Observe all
      // terminal pre-open paths and remove these temporary listeners once the socket settles.
      const cleanup = () => {
        ws.removeEventListener('open', opened);
        ws.removeEventListener('error', failed);
        ws.removeEventListener('close', closed);
      };
      const opened = () => { cleanup(); resolve(); };
      const failed = () => { cleanup(); reject(new Error('websocket failed to open')); };
      const closed = () => { cleanup(); reject(new Error('websocket closed before opening')); };
      ws.addEventListener('open', opened);
      ws.addEventListener('error', failed);
      ws.addEventListener('close', closed);
      // Handle already-open/already-closed sockets too (for injected implementations and races).
      if (ws.readyState === 1) opened();
      else if (ws.readyState >= 2) closed();
    }),
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
