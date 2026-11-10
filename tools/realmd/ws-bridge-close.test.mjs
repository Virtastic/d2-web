import test from 'node:test';
import assert from 'node:assert/strict';
import net from 'node:net';
import http from 'node:http';
import { once } from 'node:events';
import { createWsBridge } from './ws-bridge.mjs';

const tick = (ms) => new Promise((r) => setTimeout(r, ms));
const gamelogon = (name) => { const b = Buffer.alloc(37); b[0] = 0x68; b.write(name, 21, 'latin1'); return b; };
function send(sock, data, opcode = 0x2) {
  const mask = Buffer.from([1, 2, 3, 4]), h = Buffer.from([0x80 | opcode, 0x80 | data.length]);
  sock.write(Buffer.concat([h, mask, Buffer.from(data.map((x, i) => x ^ mask[i & 3]))]));
}
function ws(port, path) {
  return new Promise((resolve) => {
    const req = http.request({ host: '127.0.0.1', port, path, headers: { Connection: 'Upgrade', Upgrade: 'websocket',
      'Sec-WebSocket-Version': '13', 'Sec-WebSocket-Key': Buffer.from('0123456789abcdef').toString('base64') } });
    req.on('upgrade', (res, sock) => resolve(sock));
    req.end();
  });
}
async function start() {
  const server = { sockets: [], ended: false, reset: false };
  const d2gs = net.createServer({ allowHalfOpen: true }, (c) => {
    server.sockets.push(c);
    c.on('end', () => { server.ended = true; });
    c.on('error', (e) => { if (e.code === 'ECONNRESET') server.reset = true; });
    c.on('data', () => {});
  });
  const mcp = net.createServer((c) => c.on('data', () => {}));
  for (const s of [mcp, d2gs]) { s.listen(0, '127.0.0.1'); await once(s, 'listening'); }
  const bridge = createWsBridge({ listen: 0, targets: { mcp: { host: '127.0.0.1', port: mcp.address().port },
    d2gs: { host: '127.0.0.1', port: d2gs.address().port } }, graceSec: 0.05, pingSec: 0 });
  await once(bridge, 'listening');
  return { port: bridge.address().port, server, done() { bridge.close(); bridge.closeAllConnections?.(); mcp.close(); d2gs.close(); server.sockets.forEach((c) => c.destroy()); } };
}

test('the browser leaving ends the game connection with a FIN, so the game server can finish its save', async () => {
  const t = await start();
  const sock = await ws(t.port, '/d2gs');
  send(sock, gamelogon('Bob')); await tick(100);
  send(sock, Buffer.alloc(0), 0x8);
  await tick(300);
  assert.equal(t.server.ended, true, 'the realm saw an orderly end of stream');
  assert.equal(t.server.reset, false, 'and no reset');
  sock.destroy(); t.done();
});
