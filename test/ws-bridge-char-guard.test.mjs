import test from 'node:test';
import assert from 'node:assert/strict';
import net from 'node:net';
import http from 'node:http';
import { once } from 'node:events';
import { createWsBridge } from '../tools/realmd/ws-bridge.mjs';
import { makeMcpFramer, refusalReply } from '../tools/realmd/char-guard.mjs';

// A scripted realm: records what the bridge forwards on /mcp and /d2gs, and answers CHARLOGON with success.
async function realm() {
  const seen = { mcp: [], d2gs: [], d2gsSockets: [] };
  const mcp = net.createServer((c) => {
    const f = makeMcpFramer({ selector: true });
    c.on('data', (d) => { for (const fr of f.push(d)) { seen.mcp.push(fr); if (fr[2] === 0x07) c.write(Buffer.from([7, 0, 0x07, 0, 0, 0, 0])); } });
  });
  const d2gs = net.createServer((c) => { seen.d2gsSockets.push(c); c.on('data', (d) => seen.d2gs.push(d)); });
  for (const s of [mcp, d2gs]) { s.listen(0, '127.0.0.1'); await once(s, 'listening'); }
  return { seen, mcp, d2gs, close() { mcp.close(); d2gs.close(); seen.d2gsSockets.forEach((c) => c.destroy()); } };
}

const socks = [];
function ws(port, path) {
  return new Promise((resolve) => {
    const req = http.request({ host: '127.0.0.1', port, path, headers: {
      Connection: 'Upgrade', Upgrade: 'websocket', 'Sec-WebSocket-Version': '13',
      'Sec-WebSocket-Key': Buffer.from('0123456789abcdef').toString('base64') } });
    req.on('upgrade', (res, sock) => {
      const got = []; socks.push(sock);
      const rx = makeServerParser();
      sock.on('data', (c) => { for (const f of rx(c)) got.push(f); });
      resolve({ sock, got });
    });
    req.end();
  });
}
// server->client frames are unmasked
function makeServerParser() {
  let buf = Buffer.alloc(0);
  return (c) => {
    buf = Buffer.concat([buf, c]); const out = [];
    for (;;) {
      if (buf.length < 2) break;
      let len = buf[1] & 0x7f, off = 2;
      if (len === 126) { if (buf.length < 4) break; len = buf.readUInt16BE(2); off = 4; }
      if (buf.length < off + len) break;
      out.push({ opcode: buf[0] & 0xf, data: buf.subarray(off, off + len) }); buf = buf.subarray(off + len);
    }
    return out;
  };
}
function send(sock, data, opcode = 0x2) {
  const mask = Buffer.from([1, 2, 3, 4]), h = Buffer.from([0x80 | opcode, 0x80 | data.length]);
  const masked = Buffer.from(data.map((b, i) => b ^ mask[i & 3]));
  sock.write(Buffer.concat([h, mask, masked]));
}
const mcpFrame = (op, body) => { const f = Buffer.alloc(3 + body.length); f.writeUInt16LE(f.length, 0); f[2] = op; body.copy(f, 3); return f; };
const logon = (name) => mcpFrame(0x07, Buffer.from(name + '\0'));
const join = (id) => mcpFrame(0x04, Buffer.concat([Buffer.from([id, 0]), Buffer.from('g\0p\0')]));
const create = (id) => mcpFrame(0x03, Buffer.concat([Buffer.from([id, 0]), Buffer.from('g\0p\0d\0')]));
const gamelogon = (name) => { const b = Buffer.alloc(37); b[0] = 0x68; b.write(name, 21, 'latin1'); return b; };
const tick = (ms) => new Promise((r) => setTimeout(r, ms));

async function start(opts = {}) {
  const r = await realm();
  const bridge = createWsBridge({ listen: 0, targets: {
    mcp: { host: '127.0.0.1', port: r.mcp.address().port }, d2gs: { host: '127.0.0.1', port: r.d2gs.address().port } },
  graceSec: 0.05, pingSec: 0, ...opts });
  await once(bridge, 'listening');
  return { port: bridge.address().port, r, done() { socks.splice(0).forEach((x) => x.destroy()); bridge.close(); bridge.closeAllConnections?.(); r.close(); } };
}
async function login(port, name) {
  const m = await ws(port, '/mcp');
  send(m.sock, Buffer.concat([Buffer.from([1]), logon(name)]));
  await tick(60);
  return m;
}

test('a join for a character held by a live game gets the failure reply, not a forward', async () => {
  const t = await start();
  const a = await ws(t.port, '/d2gs');
  send(a.sock, gamelogon('PalTest'));
  const c = await login(t.port, 'paltest');
  const before = t.r.seen.mcp.length;
  send(c.sock, join(2));
  await tick(60);
  assert.equal(t.r.seen.mcp.length, before, 'the doomed join never reaches the realm');
  const reply = c.got.find((f) => f.data[2] === 0x04);
  assert.ok(reply, 'the client got a JOINGAME answer');
  assert.deepEqual([...reply.data], [...refusalReply(join(2))]);
  assert.equal(reply.data.readUInt32LE(3 + 14), 1, 'result is a failure');
  assert.equal(reply.data.readUInt32LE(3 + 6), 0, 'no game server address');
  assert.equal(reply.data.readUInt16LE(3), 2, 'request id echoed');
  send(c.sock, create(3));
  await tick(60);
  const cr = c.got.find((f) => f.data[2] === 0x03);
  assert.equal(cr.data.readUInt32LE(3 + 6), 1);
  assert.equal(t.r.seen.mcp.filter((f) => f[2] === 0x03 || f[2] === 0x04).length, 0);
  t.done();
});

test('after the game connection closes, the grace passes and the join is forwarded', async () => {
  const t = await start();
  const a = await ws(t.port, '/d2gs');
  send(a.sock, gamelogon('PalTest'));
  await tick(40);
  const c = await login(t.port, 'PalTest');
  a.sock.destroy();
  await tick(250);
  send(c.sock, join(2));
  await tick(60);
  assert.equal(t.r.seen.mcp.filter((f) => f[2] === 0x04).length, 1);
  assert.equal(c.got.filter((f) => f.data[2] === 0x04).length, 0);
  t.done();
});

test('a different character, and a session that never logged on, are untouched', async () => {
  const t = await start();
  const a = await ws(t.port, '/d2gs');
  send(a.sock, gamelogon('PalTest'));
  await tick(40);
  const c = await login(t.port, 'PalTwo');
  const d = await ws(t.port, '/mcp');
  send(d.sock, Buffer.concat([Buffer.from([1]), join(5)]));
  send(c.sock, join(6));
  await tick(80);
  assert.equal(t.r.seen.mcp.filter((f) => f[2] === 0x04).length, 2);
  t.done();
});

test('--no-char-guard forwards everything', async () => {
  const t = await start({ charGuard: false });
  const a = await ws(t.port, '/d2gs');
  send(a.sock, gamelogon('PalTest'));
  const c = await login(t.port, 'PalTest');
  send(c.sock, join(2));
  await tick(60);
  assert.equal(t.r.seen.mcp.filter((f) => f[2] === 0x04).length, 1);
  t.done();
});

test('a browser that goes silent is dropped even while the realm keeps talking', async () => {
  const t = await start({ pingSec: 0.04 });
  const a = await ws(t.port, '/d2gs');          // never answers the pings
  await tick(30);
  const server = t.r.seen.d2gsSockets[0];
  const feed = setInterval(() => server.write(Buffer.from([0xaf])), 10);   // realm output would reset an idle timer
  const closed = once(server, 'close');
  await Promise.race([closed, tick(1000).then(() => { throw new Error('game connection outlived its silent browser'); })]);
  clearInterval(feed);
  assert.ok(a.got.some((f) => f.opcode === 0x9), 'pings were sent');
  t.done();
});

test('a browser that answers pings is kept', async () => {
  const t = await start({ pingSec: 0.04 });
  const a = await ws(t.port, '/d2gs');
  a.sock.on('data', () => { for (const f of a.got.splice(0)) if (f.opcode === 0x9) send(a.sock, f.data, 0xa); });
  const iv = setInterval(() => { for (const f of a.got.splice(0)) if (f.opcode === 0x9) send(a.sock, Buffer.alloc(0), 0xa); }, 10);
  await tick(400);
  clearInterval(iv);
  assert.equal(t.r.seen.d2gsSockets[0].destroyed, false);
  t.done();
});
