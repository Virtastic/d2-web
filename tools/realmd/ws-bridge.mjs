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
//
// Hardening (all optional, all with safe defaults; a public deployment sits behind a reverse proxy):
//   --host 127.0.0.1        bind address (default loopback)
//   --trust-proxy           take the client address from CF-Connecting-IP / X-Forwarded-For
//   --origins a,b           accept only these Origin header values (default: any)
//   --max-per-ip 8          concurrent sockets per client address
//   --max-total 512         concurrent sockets overall
//   --max-message 65536     largest WebSocket message in bytes; a bigger one closes the socket
//   --rate 30               new connections per client address per minute
//   --idle 600              seconds without traffic in either direction before closing
//   --handshake 10          seconds to complete the upgrade and the TCP connect
//   --ping 20               seconds between WebSocket pings; a peer silent for 2.5 intervals is dropped, so
//                           a vanished browser cannot keep its game connection (and the character) alive
//   --grace 1.5             seconds a closed game connection still counts as holding its character
//   --no-char-guard         forward create/join even for a character that is still in a game
import http from 'node:http';
import net from 'node:net';
import crypto from 'node:crypto';
import { createCharGuard } from './char-guard.mjs';

const GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';

export const DEFAULTS = Object.freeze({
  host: '127.0.0.1', trustProxy: false, origins: null,
  maxPerIp: 8, maxTotal: 512, maxMessage: 65536, ratePerMin: 30, idleSec: 600, handshakeSec: 10,
  pingSec: 20, graceSec: 1.5, charGuard: true,
});

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
 * A frame whose declared length exceeds `maxFrame` throws BEFORE any of it is buffered, so a peer
 * cannot make the bridge allocate by announcing a huge frame.
 */
export function makeWsParser(maxFrame = Infinity) {
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
      if (len > maxFrame) throw new Error(`frame of ${len} bytes exceeds the ${maxFrame} byte cap`);
      if (opcode >= 0x8 && (len > 125 || !fin)) throw new Error('a control frame must be short and unfragmented (RFC 6455 5.5)');
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

/** The client address: the socket peer, or the proxy's header when --trust-proxy is on. */
function clientAddr(req, trustProxy) {
  if (trustProxy) {
    const cf = req.headers['cf-connecting-ip'];
    if (cf) return String(cf).trim();
    const xff = req.headers['x-forwarded-for'];
    if (xff) return String(xff).split(',')[0].trim();
  }
  return req.socket.remoteAddress || 'unknown';
}

/** Start the bridge. Returns the http.Server so a test can close it. */
export function createWsBridge({ listen, targets, onLog = () => {}, ...overrides }) {
  const cfg = { ...DEFAULTS, ...overrides };
  const perIp = new Map();      // address -> open sockets
  const recent = new Map();     // address -> connection timestamps within the last minute
  let total = 0;
  const guard = cfg.charGuard ? createCharGuard({ graceMs: cfg.graceSec * 1000, onLog }) : null;

  const admit = (addr) => {
    if (total >= cfg.maxTotal) return 'server full';
    if ((perIp.get(addr) || 0) >= cfg.maxPerIp) return 'too many connections from this address';
    const now = Date.now();
    const times = (recent.get(addr) || []).filter((t) => now - t < 60000);
    if (times.length >= cfg.ratePerMin) { recent.set(addr, times); return 'connecting too fast'; }
    times.push(now); recent.set(addr, times);
    return null;
  };
  // Forget addresses that went quiet, so the maps cannot grow without bound.
  const sweep = setInterval(() => {
    const now = Date.now();
    for (const [a, ts] of recent) if (!ts.some((t) => now - t < 60000)) recent.delete(a);
  }, 60000);
  sweep.unref();

  const server = http.createServer((req, res) => { res.writeHead(426); res.end('WebSocket only'); });
  server.on('close', () => clearInterval(sweep));
  server.on('upgrade', (req, sock) => {
    const reject = (code, why, addr) => { onLog(`refused ${addr || ''} ${why}`); try { sock.end(`HTTP/1.1 ${code}\r\nConnection: close\r\n\r\n`); } catch { /* gone */ } sock.destroy(); };
    const key = req.headers['sec-websocket-key'];
    const name = String(req.url || '').replace(/^\/+/, '').split('?')[0];
    const target = Object.hasOwn(targets, name) ? targets[name] : null;
    const addr = clientAddr(req, cfg.trustProxy);
    if (!key || !target) { reject('404 Not Found', `unknown path /${name}`, addr); return; }
    if (cfg.origins && !cfg.origins.includes(String(req.headers.origin || ''))) { reject('403 Forbidden', `origin ${req.headers.origin || '(none)'}`, addr); return; }
    const why = admit(addr);
    if (why) { reject('429 Too Many Requests', why, addr); return; }

    total++; perIp.set(addr, (perIp.get(addr) || 0) + 1);
    let released = false;
    const release = () => {
      if (released) return; released = true;
      total--; const n = (perIp.get(addr) || 1) - 1; if (n > 0) perIp.set(addr, n); else perIp.delete(addr);
    };

    sock.write(
      'HTTP/1.1 101 Switching Protocols\r\n' +
      'Upgrade: websocket\r\nConnection: Upgrade\r\n' +
      `Sec-WebSocket-Accept: ${acceptKey(key)}\r\n\r\n`);
    sock.setNoDelay(true);

    const tcp = net.createConnection({ host: target.host, port: target.port });
    tcp.setNoDelay(true);
    let timers = [];
    const opened = Date.now();
    const tap = guard && name === 'd2gs' ? guard.d2gsTap() : null;
    const mcp = guard && name === 'mcp' ? guard.mcpTap() : null;
    let lastRx = Date.now();
    const closeBoth = (why = 'closed') => {
      if (!released) onLog(`${why} ${addr} /${name} after ${((Date.now() - opened) / 1000).toFixed(1)}s`);
      release(); tap?.close(String(why).startsWith("realm")); timers.forEach(clearTimeout); timers = []; sock.destroy(); tcp.destroy();
    };
    if (cfg.pingSec > 0) {
      const ms = cfg.pingSec * 1000;
      const live = setInterval(() => {
        if (Date.now() - lastRx > ms * 2.5) { closeBoth('dead peer'); return; }
        try { sock.write(encodeFrame(Buffer.alloc(0), 0x9)); } catch { closeBoth(); }
      }, ms);
      timers.push(live);
    }
    let idle = null;
    const touch = () => { clearTimeout(idle); idle = setTimeout(() => { onLog(`idle close ${addr} /${name}`); closeBoth(); }, cfg.idleSec * 1000); timers.push(idle); };
    const connectTimer = setTimeout(() => { onLog(`connect timeout ${addr} /${name}`); closeBoth(); }, cfg.handshakeSec * 1000);
    timers.push(connectTimer);
    tcp.on('connect', () => { clearTimeout(connectTimer); touch(); onLog(`ws /${name} -> ${target.host}:${target.port} from ${addr}`); });
    tcp.on('data', (d) => {
      touch(); mcp?.fromRealm(d); tap?.fromRealm(d);
      try { sock.write(encodeFrame(d)); } catch { closeBoth(); return; }
      // Backpressure: a browser that stops reading must not make the bridge buffer the realm's output.
      if (sock.writableLength > cfg.maxMessage * 16) { onLog(`slow reader ${addr} /${name}`); closeBoth(); }
    });
    tcp.on('error', () => closeBoth('realm error')); tcp.on('close', () => closeBoth('realm closed'));

    // The one place client bytes reach the realm: the character guard may swap a doomed create/join
    // for the realm's own failure reply; every other byte goes through unchanged.
    let gated = null;                      // client bytes that arrived while a create/join waits for the unlock
    const toRealm = (data) => {
      if (gated) { gated.push(data); return; }
      tap?.fromClient(data);
      if (!mcp) { tcp.write(data); return; }
      const { forward, reply, delayMs, later } = mcp.fromClient(data);
      if (forward.length) tcp.write(forward);
      if (reply) sock.write(encodeFrame(reply));
      if (later) {
        gated = [];
        const t = setTimeout(() => { const q = gated; gated = null; for (const d of [later, ...q]) toRealm(d); }, delayMs);
        timers.push(t);
      }
    };
    const parse = makeWsParser(cfg.maxMessage);
    let fragment = null; // accumulate a fragmented message (browsers may split a large binary send)
    sock.on('data', (chunk) => {
      touch(); lastRx = Date.now();
      let frames;
      try { frames = parse(chunk); } catch (e) { onLog(`bad frame from ${addr}: ${e.message}`); closeBoth(); return; }
      for (const f of frames) {
        if (f.opcode === 0x8) { closeBoth(); return; }                        // close
        if (f.opcode === 0x9) { sock.write(encodeFrame(f.data, 0xa)); continue; } // ping -> pong
        if (f.opcode === 0xa) continue;                                         // pong
        if (f.opcode === 0x0) {
          if (!fragment) { closeBoth(); return; }                             // continuation with nothing to continue
          fragment = Buffer.concat([fragment, f.data]);
          if (fragment.length > cfg.maxMessage) { onLog(`message too large from ${addr}`); closeBoth(); return; }
          if (f.fin) { toRealm(fragment); fragment = null; }
          continue;
        }
        if (f.opcode === 0x1 || f.opcode === 0x2) {                             // text / binary
          if (f.fin) toRealm(f.data); else fragment = f.data;
        }
      }
    });
    sock.on('error', () => closeBoth('browser error')); sock.on('end', () => closeBoth('browser ended')); sock.on('close', () => closeBoth('browser closed'));
  });
  server.listen(listen, cfg.host, () => onLog(`ws-bridge listening on ${cfg.host}:${server.address().port}`));
  return server;
}

const parseAddr = (s) => { const i = s.lastIndexOf(':'); return { host: s.slice(0, i), port: Number(s.slice(i + 1)) }; };

function main(argv) {
  const opt = {};
  for (let i = 0; i < argv.length; i++) {
    if (!argv[i].startsWith('--')) continue;
    const k = argv[i].slice(2);
    if (k === 'trust-proxy' || k === 'no-char-guard') opt[k] = '1'; else opt[k] = argv[++i];
  }
  const listen = Number(opt.listen || 7900);
  const targets = {};
  for (const k of ['bncs', 'mcp', 'd2gs']) if (opt[k]) targets[k] = parseAddr(opt[k]);
  if (!Object.keys(targets).length) { console.error('ws-bridge: no --bncs/--mcp/--d2gs targets given'); process.exit(2); }
  const num = (k, d) => (opt[k] !== undefined ? Number(opt[k]) : d);
  const log = (m) => { console.log(`ws-bridge: ${m}`); };
  createWsBridge({
    listen, targets, onLog: log,
    host: opt.host || DEFAULTS.host,
    trustProxy: !!opt['trust-proxy'],
    origins: opt.origins ? opt.origins.split(',').map((s) => s.trim()).filter(Boolean) : null,
    maxPerIp: num('max-per-ip', DEFAULTS.maxPerIp),
    maxTotal: num('max-total', DEFAULTS.maxTotal),
    maxMessage: num('max-message', DEFAULTS.maxMessage),
    ratePerMin: num('rate', DEFAULTS.ratePerMin),
    idleSec: num('idle', DEFAULTS.idleSec),
    handshakeSec: num('handshake', DEFAULTS.handshakeSec),
    pingSec: num('ping', DEFAULTS.pingSec),
    graceSec: num('grace', DEFAULTS.graceSec),
    charGuard: !opt['no-char-guard'],
  });
}

if (import.meta.url === `file://${process.argv[1]}`) main(process.argv.slice(2));
