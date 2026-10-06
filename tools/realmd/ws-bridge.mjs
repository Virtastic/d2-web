// ws-bridge -- the browser-compatible transport to a project-configured realm endpoint.
//
// A browser cannot open a raw TCP socket, and the realm protocols (BNCS/MCP/D2GS) are raw TCP:
// BNCS frames are `[0xFF][id][len]`, MCP `[len][op]`, D2GS an unframed client stream with a
// length-framed Huffman server stream. So the browser reaches those protocols the only way it can:
// a WebSocket, whose payload bytes this bridge copies verbatim to and from a TCP socket. It adds no
// framing of its own -- a byte the client writes is a byte the realm reads -- so nothing about the
// protocol is reimplemented here and nothing can drift.
//
// This is the "explicit browser-compatible transport to a project-configured external endpoint"
// names as the missing half of the TCP/IP / gateway path: the page points at a
// configured endpoint (never hosting Battle.net itself), and this bridges its WebSocket to it.
//
// Usage:  node tools/realmd/ws-bridge.mjs --listen 7900 \
//           --bncs 127.0.0.1:6112 --mcp 127.0.0.1:6113 --d2gs 127.0.0.1:4000
// The WebSocket URL path selects the target: ws://host:7900/bncs, /mcp, /d2gs.
import http from 'node:http';
import net from 'node:net';
import crypto from 'node:crypto';

const GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';

/** The RFC 6455 accept token: base64(sha1(key ++ GUID)). */
export function acceptKey(key) {
  return crypto.createHash('sha1').update(key + GUID).digest('base64');
}

/** Encode one unmasked server->client frame. Length uses the 7/16/64-bit forms exactly as the RFC. */
export function encodeFrame(payload, opcode = 0x2) {
  const len = payload.length;
  let header;
  if (len < 126) header = Buffer.from([0x80 | opcode, len]);
  else if (len < 0x10000) { header = Buffer.alloc(4); header[0] = 0x80 | opcode; header[1] = 126; header.writeUInt16BE(len, 2); }
  else { header = Buffer.alloc(10); header[0] = 0x80 | opcode; header[1] = 127; header.writeBigUInt64BE(BigInt(len), 2); }
  return Buffer.concat([header, payload]);
}

/**
 * A WebSocket frame parser that ACCUMULATES. TCP and WebSocket both hand over arbitrary fragments,
 * so a frame can arrive split and several can arrive in one chunk; this returns whole frames only
 * and keeps the tail for the next call. Client frames are always masked, so each is unmasked here.
 */
export function makeWsParser() {
  let buf = Buffer.alloc(0);
  return (chunk) => {
    buf = Buffer.concat([buf, chunk]);
    const frames = [];
    for (;;) {
      if (buf.length < 2) break;
      const fin = (buf[0] & 0x80) !== 0, opcode = buf[0] & 0x0f, masked = (buf[1] & 0x80) !== 0;
      let len = buf[1] & 0x7f, off = 2;
      if (len === 126) { if (buf.length < 4) break; len = buf.readUInt16BE(2); off = 4; }
      else if (len === 127) { if (buf.length < 10) break; len = Number(buf.readBigUInt64BE(2)); off = 10; }
      if (!masked) throw new Error('a client frame must be masked (RFC 6455 5.1)');
      if (buf.length < off + 4 + len) break;
      const mask = buf.subarray(off, off + 4);
      const data = Buffer.allocUnsafe(len);
      for (let i = 0; i < len; i++) data[i] = buf[off + 4 + i] ^ mask[i & 3];
      buf = buf.subarray(off + 4 + len);
      frames.push({ fin, opcode, data });
    }
    return frames;
  };
}

/** Start the bridge. Returns the http.Server so a test can close it. */
export function createWsBridge({ listen, targets, onLog = () => {} }) {
  const server = http.createServer((req, res) => { res.writeHead(426); res.end('WebSocket only'); });
  server.on('upgrade', (req, sock) => {
    const key = req.headers['sec-websocket-key'];
    const name = String(req.url || '').replace(/^\/+/, '').split('?')[0];
    const target = targets[name];
    if (!key || !target) { sock.destroy(); return; }
    sock.write(
      'HTTP/1.1 101 Switching Protocols\r\n' +
      'Upgrade: websocket\r\nConnection: Upgrade\r\n' +
      `Sec-WebSocket-Accept: ${acceptKey(key)}\r\n\r\n`);
    sock.setNoDelay(true);

    const tcp = net.createConnection({ host: target.host, port: target.port });
    const closeBoth = () => { sock.destroy(); tcp.destroy(); };
    tcp.on('connect', () => onLog(`ws /${name} -> ${target.host}:${target.port}`));
    tcp.on('data', (d) => { try { sock.write(encodeFrame(d)); } catch { closeBoth(); } });
    tcp.on('error', closeBoth); tcp.on('close', closeBoth);

    const parse = makeWsParser();
    let fragment = null; // accumulate a fragmented message (browsers may split a large binary send)
    sock.on('data', (chunk) => {
      let frames;
      try { frames = parse(chunk); } catch { closeBoth(); return; }
      for (const f of frames) {
        if (f.opcode === 0x8) { closeBoth(); return; }                        // close
        if (f.opcode === 0x9) { sock.write(encodeFrame(f.data, 0xa)); continue; } // ping -> pong
        if (f.opcode === 0xa) continue;                                         // pong
        if (f.opcode === 0x0) { if (fragment) { fragment = Buffer.concat([fragment, f.data]); if (f.fin) { tcp.write(fragment); fragment = null; } } continue; }
        if (f.opcode === 0x1 || f.opcode === 0x2) {                             // text / binary
          if (f.fin) tcp.write(f.data); else fragment = f.data;
        }
      }
    });
    sock.on('error', closeBoth); sock.on('close', closeBoth);
  });
  server.listen(listen, '127.0.0.1', () => onLog(`ws-bridge listening on 127.0.0.1:${listen}`));
  return server;
}

const parseAddr = (s) => { const i = s.lastIndexOf(':'); return { host: s.slice(0, i), port: Number(s.slice(i + 1)) }; };

function main(argv) {
  const opt = {};
  for (let i = 0; i < argv.length; i += 2) if (argv[i].startsWith('--')) opt[argv[i].slice(2)] = argv[i + 1];
  const listen = Number(opt.listen || 7900);
  const targets = {};
  for (const k of ['bncs', 'mcp', 'd2gs']) if (opt[k]) targets[k] = parseAddr(opt[k]);
  if (!Object.keys(targets).length) { console.error('ws-bridge: no --bncs/--mcp/--d2gs targets given'); process.exit(2); }
  const log = (m) => { console.log(`ws-bridge: ${m}`); };
  createWsBridge({ listen, targets, onLog: log });
  // Announce readiness on stdout so a supervisor can wait rather than sleep.
  log(`listening ws://127.0.0.1:${listen} for ${Object.keys(targets).join(',')}`);
}

if (import.meta.url === `file://${process.argv[1]}`) main(process.argv.slice(2));
