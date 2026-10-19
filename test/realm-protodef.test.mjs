// realmd's real wire output, decoded by an INDEPENDENT schema.
//
// Everything else validates our bytes against our own reading of the protocol. This decodes what
// realmd actually puts on a socket using the protodef packet definitions from
// reference/diablo2-protocol/data/1.13 -- someone else's reading of the same version, written from
// their own sources. If our composer and their schema disagree about a field's order, width or
// endianness, the decode lands on the wrong bytes and the strict length check fails.
//
// It is the strongest interoperability signal available here short of a stock 1.13c client.
import { test, before, after } from 'node:test';
import assert from 'node:assert/strict';
import { startRealmd, connect, reader, u32le, fourcc, bncsFrame, mcpFrame } from './helpers/realmd.mjs';
import { loadSchema, decode } from './helpers/protodef.mjs';

const PORTS = { bncs: 0, mcp: 0, d2gs: 0 };
let proc = null;
const sid = loadSchema('sid');
const mcp = loadSchema('mcp');

// Needs BOTH a buildable realmd and the vendored schemas; either may be absent (CI has no
// reference/), and an absent input is a skip, not a failure.
before(async () => { if (sid && mcp) proc = await startRealmd(PORTS, 'protodef'); });
after(() => { if (proc && !proc.killed) proc.kill('SIGKILL'); });

// Send a BNCS frame and return the next reply's body (header stripped).
async function bncsRoundTrip(sock, rx, id, body) {
  sock.write(bncsFrame(id, body));
  const head = await rx.want(4);
  assert.equal(head[0], 0xff, 'BNCS reply must be a 0xFF frame');
  const total = head.readUInt16LE(2);
  const full = await rx.want(total);
  const replyBody = Buffer.from(full.subarray(4, total));
  rx.skip(total);
  return { id: head[1], body: replyBody };
}

async function mcpRoundTrip(sock, rx, op, body) {
  sock.write(mcpFrame(op, body));
  const head = await rx.want(3);
  const total = head.readUInt16LE(0);
  const full = await rx.want(total);
  const replyBody = Buffer.from(full.subarray(3, total));
  rx.skip(total);
  return { op: head[2], body: replyBody };
}

test('BNCS replies decode cleanly under the independent 1.13 SID schema', async (t) => {
  if (!sid || !mcp) return t.skip('reference/diablo2-protocol schemas not present (not committed)');
  if (!proc) return t.skip('realmd unavailable');
  const s = await connect(PORTS.bncs);
  const rx = reader(s);
  try {
    s.write(Buffer.from([0x01]));

    const authInfoBody = Buffer.concat([
      u32le(0), u32le(fourcc('IX86')), u32le(fourcc('D2XP')), u32le(0x0e),
      u32le(0), u32le(0), u32le(0), u32le(0), u32le(0),
      Buffer.from('USA\0United States\0', 'latin1'),
    ]);
    const ai = await bncsRoundTrip(s, rx, 0x50, authInfoBody);
    assert.equal(ai.id, 0x50);
    const info = decode(sid, 'toClient', 'SID_AUTH_INFO', ai.body);
    assert.equal(info.mpqFilename, 'ver-IX86-5.mpq');
    assert.equal(info.serverToken >>> 0, 0x1234abcd, 'serverToken the login hash is computed over');
    assert.ok(info.valuestring.length > 0, 'CheckRevision challenge must be present');

    const ac = await bncsRoundTrip(s, rx, 0x51, Buffer.concat([u32le(0xcafebabe), u32le(0x0e), u32le(0), u32le(0), u32le(0), Buffer.from('D2\0probe\0', 'latin1')]));
    assert.equal(decode(sid, 'toClient', 'SID_AUTH_CHECK', ac.body).result, 0, 'version check passed');

    const lr = await bncsRoundTrip(s, rx, 0x3a, Buffer.concat([u32le(0xcafebabe), u32le(0x1234abcd), Buffer.alloc(20), Buffer.from('bob\0', 'latin1')]));
    assert.equal(decode(sid, 'toClient', 'SID_LOGONRESPONSE2', lr.body).status, 0, 'login accepted');

    const qr = await bncsRoundTrip(s, rx, 0x40, Buffer.alloc(0));
    const realms = decode(sid, 'toClient', 'SID_QUERYREALMS2', qr.body);
    assert.equal(realms.realms.length, 1);
    assert.equal(realms.realms[0].realmTitle, 'd2-web');

    // The handoff -- the richest layout, and the one with the endianness traps.
    const rex = await bncsRoundTrip(s, rx, 0x3e, Buffer.concat([u32le(0xcafebabe), Buffer.alloc(20), Buffer.from('d2-web\0', 'latin1')]));
    const h = decode(sid, 'toClient', 'SID_LOGONREALMEX', rex.body);
    assert.equal(h.MCPCookie >>> 0, 0xcafebabe, 'cookie must echo the client token');
    assert.equal(h.MCPStatus, 1);
    assert.deepEqual([...h.IP], [127, 0, 0, 1], 'IP is a byte array, i.e. network order');
    // The schema types this as big-endian u16 followed by a zero u16; our composer writes
    // (port << 16) big-endian, which only agrees if both readings are right.
    assert.equal(h.port, 6113, 'MCP port must decode as a big-endian u16');
    assert.equal(h.zero, 0);
    assert.equal(h.MCPChunk1.length, 2);
    assert.equal(h.MCPChunk2.length, 12, 'the 48-byte block MCP_STARTUP forwards');
    assert.equal(h.battleNetUniqueName, 'player');
  } finally { s.destroy(); }
});

test('MCP replies decode cleanly under the independent 1.13 MCP schema', async (t) => {
  if (!sid || !mcp) return t.skip('reference/diablo2-protocol schemas not present (not committed)');
  if (!proc) return t.skip('realmd unavailable');
  const s = await connect(PORTS.mcp);
  const rx = reader(s);
  try {
    s.write(Buffer.from([0x01]));

    const st = await mcpRoundTrip(s, rx, 0x01, Buffer.alloc(64));
    assert.equal(decode(mcp, 'toClient', 'MCP_STARTUP', st.body).result, 0, 'session accepted');

    const cl = await mcpRoundTrip(s, rx, 0x19, u32le(8));
    const list = decode(mcp, 'toClient', 'MCP_CHARLIST2', cl.body);
    assert.equal(list.numbersOfCharactersInAccount, 1);
    assert.equal(list.characters.length, 1);
    assert.equal(list.characters[0].characterName, 'player');
    // Class and level live in the statstring, which the schema treats as an opaque string -- so
    // this also proves it is null-terminated where the client expects it to end.
    assert.ok(list.characters[0].characterStatstring.length >= 26, 'statstring must reach the level byte at 25');

    const lo = await mcpRoundTrip(s, rx, 0x07, Buffer.from('player\0', 'latin1'));
    assert.equal(decode(mcp, 'toClient', 'MCP_CHARLOGON', lo.body).result, 0, 'logged onto the character');

    const cg = await mcpRoundTrip(s, rx, 0x03, Buffer.concat([Buffer.from([1, 0]), u32le(0), Buffer.from([1, 0, 8]), Buffer.from('g\0\0\0', 'latin1')]));
    const created = decode(mcp, 'toClient', 'MCP_CREATEGAME', cg.body);
    assert.equal(created.result, 0, 'game created');
    assert.equal(created.gameToken, 0x1234);

    const jg = await mcpRoundTrip(s, rx, 0x04, Buffer.concat([Buffer.from([2, 0]), Buffer.from('g\0\0', 'latin1')]));
    const joined = decode(mcp, 'toClient', 'MCP_JOINGAME', jg.body);
    assert.equal(joined.result, 0, 'join ok');
    assert.equal(joined.gameToken, 0x1234);
    assert.deepEqual([...joined.IPOfD2GSServer], [127, 0, 0, 1], 'the game server the client will dial');
    assert.equal(joined.gameHash >>> 0, 0xdeadbeef, 'the hash GAMELOGON carries to D2GS');
  } finally { s.destroy(); }
});
