// Drives play/assets/realm-client.mjs from node against a native realmd, with no browser and no emcc:
// the client's Module is a heap image whose ccall runs the native composers (test/helpers/realm-ccall-host.cpp,
// built from the same src/core/realm sources the engine uses) and a transport that is a plain TCP socket.
import { spawnSync } from 'node:child_process';
import net from 'node:net';
import { writeFileSync, readFileSync, mkdtempSync, rmSync, existsSync, readdirSync } from 'node:fs';
import { join } from 'node:path';
import { tmpdir } from 'node:os';
import { ROOT } from './realmd.mjs';

let hostBin = null;
export function buildCcallHost() {
  if (hostBin) return hostBin;
  if (spawnSync('g++', ['--version']).error?.code === 'ENOENT') return null;
  const out = join(ROOT, 'build-wasm', `realm-ccall-host-${process.pid}`);
  // The client composers are the 1.13c BNClient / D2MCPClient ports the engine links; what they reach for in
  // the OS or the rest of the engine stays unresolved and is never called by a composer.
  const files = ['realm', 'bnclient', 'd2mcpclient'].flatMap((d) => readdirSync(join(ROOT, 'src/core', d))
    .filter((f) => f.endsWith('.cpp') && !/\.test\.cpp$|^d2gs_client|batch_(2558|5906|6265|1499)/.test(f)).map((f) => join(ROOT, 'src/core', d, f)));
  const b = spawnSync('g++', ['-O0', '-std=c++20', '-w', '-I' + join(ROOT, 'src'), '-I' + join(ROOT, 'tools/realmd/shim'),
    '-no-pie', '-Wl,--unresolved-symbols=ignore-all', join(ROOT, 'test/helpers/realm-ccall-host.cpp'), ...files, '-o', out], { encoding: 'utf8' });
  if (b.status !== 0) throw new Error(`ccall host build failed:\n${b.stderr}`);
  return (hostBin = out);
}

// An Emscripten-shaped Module: HEAPU8, _malloc/_free, and ccall through the native host.
export function nativeModule(bin = buildCcallHost()) {
  const heap = new Uint8Array(1 << 19);
  let next = 4096;
  const dir = mkdtempSync(join(tmpdir(), 'ccall-'));
  const mem = join(dir, 'heap');
  return {
    HEAPU8: heap,
    _malloc: (n) => { const p = next; next += (Number(n) + 15) & ~15; if (next > heap.length) throw new Error('test heap full'); return BigInt(p); },
    _free: () => {},
    ccall(name, _ret, types, args) {
      writeFileSync(mem, heap);
      const argv = args.map((a, i) => types[i] === 'string' ? 's:' + Buffer.from(String(a), 'utf8').toString('hex') : 'n:' + BigInt.asUintN(64, BigInt(a)));
      const r = spawnSync(bin, [mem, name, ...argv], { encoding: 'utf8' });
      if (r.status !== 0) throw new Error(`${name} failed (${r.status}): ${r.stderr}`);
      heap.set(readFileSync(mem));
      return Number(BigInt.asIntN(32, BigInt(r.stdout.trim())));
    },
    dispose() { rmSync(dir, { recursive: true, force: true }); if (existsSync(bin) && false) rmSync(bin); },
  };
}

// The client's transport interface over a TCP socket.
export function tcpTransport(port) {
  const sock = net.createConnection({ host: '127.0.0.1', port });
  const dataCbs = [], closeCbs = [], pending = []; let open = false;
  sock.on('connect', () => { open = true; });
  sock.on('data', (d) => { const u = new Uint8Array(d); if (!dataCbs.length) pending.push(u); else for (const cb of dataCbs) cb(u); });
  sock.on('close', () => { open = false; for (const cb of closeCbs) cb(); });
  sock.on('error', () => {});
  return {
    send: (bytes) => sock.write(Buffer.from(bytes)),
    get isOpen() { return open; },
    onData: (cb) => { dataCbs.push(cb); while (pending.length) cb(pending.shift()); },
    onClose: (cb) => closeCbs.push(cb),
    close: () => sock.destroy(),
    ready: () => new Promise((resolve, reject) => { if (open) resolve(); else { sock.once('connect', resolve); sock.once('error', reject); } }),
  };
}
