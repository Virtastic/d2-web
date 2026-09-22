// Shared plumbing for the tests that drive realmd over a real socket.
// Lives under test/helpers/ so `node --test test/*.test.mjs` does not try to run it as a suite.
import { spawn, spawnSync } from 'node:child_process';
import net from 'node:net';
import { existsSync } from 'node:fs';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

export const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
// Build realmd and start it on the given ports. Returns null when the box cannot build it, so the
// caller can skip rather than fail.
//
// `tag` gives each test file its OWN binary. node --test runs files in parallel, so a shared output
// path means one file's g++ rewrites the binary another file is currently executing -- which fails
// as ETXTBSY, intermittently and confusingly.
export async function startRealmd(ports, tag = 'test') {
  const BIN = join(ROOT, 'build-wasm', `realmd-${tag}`);
  if (spawnSync('g++', ['--version']).status !== 0) return null;
  if (spawnSync('bash', [join(ROOT, 'ci', 'build-realmd.sh'), BIN]).status !== 0) return null;
  if (!existsSync(BIN)) return null;
  const proc = spawn(BIN, ['--bncs', String(ports.bncs), '--mcp', String(ports.mcp), '--d2gs', String(ports.d2gs)],
    { stdio: ['ignore', 'pipe', 'pipe'] });
  // realmd announces readiness on stdout, so wait for that rather than sleeping.
  const ok = await new Promise((resolve) => {
    const t = setTimeout(() => resolve(false), 8000);
    proc.stdout.on('data', (d) => {
      if (d.toString().includes('realmd listening')) { clearTimeout(t); resolve(true); }
    });
    proc.once('exit', () => { clearTimeout(t); resolve(false); });
  });
  if (!ok) { proc.kill('SIGKILL'); return null; }
  return proc;
}

export const connect = (port) => new Promise((resolve, reject) => {
  const s = net.createConnection({ host: '127.0.0.1', port }, () => resolve(s));
  s.once('error', reject);
});

// A socket reader that ACCUMULATES. The obvious per-call reader is wrong here and fails in a way
// that looks like a server bug: one read can deliver a whole reply, so a helper that waits for the
// header and then starts over waiting for the body blocks forever on bytes already in hand.
export const reader = (sock) => {
  let buf = Buffer.alloc(0);
  let closed = false;
  const waiters = [];
  const pump = () => {
    for (let i = waiters.length - 1; i >= 0; i--) {
      const w = waiters[i];
      if (buf.length >= w.n) { clearTimeout(w.t); waiters.splice(i, 1); w.resolve(buf.subarray(0, w.n)); }
      else if (closed) { clearTimeout(w.t); waiters.splice(i, 1); w.reject(new Error(`closed after ${buf.length}/${w.n} bytes`)); }
    }
  };
  sock.on('data', (d) => { buf = Buffer.concat([buf, d]); pump(); });
  sock.on('close', () => { closed = true; pump(); });
  return {
    // Wait until at least n bytes have arrived; returns the first n WITHOUT consuming, so a caller
    // can read a header and then the whole frame it describes.
    want: (n, ms = 4000) => new Promise((resolve, reject) => {
      const w = { n, resolve, reject, t: null };
      w.t = setTimeout(() => {
        const i = waiters.indexOf(w);
        if (i >= 0) waiters.splice(i, 1);
        reject(new Error(`timed out after ${buf.length}/${n} bytes`));
      }, ms);
      waiters.push(w);
      pump();
    }),
    // Drop n bytes off the front. `want` deliberately does NOT consume, so a caller doing repeated
    // request/reply round trips on one socket must skip each frame once it has read it -- otherwise
    // every later read returns the FIRST reply again, which looks like a protocol disagreement.
    skip: (n) => { buf = buf.subarray(n); },
  };
};

export const u32le = (v) => { const b = Buffer.alloc(4); b.writeUInt32LE(v >>> 0); return b; };
export const fourcc = (s) => ((s.charCodeAt(0) << 24) | (s.charCodeAt(1) << 16) | (s.charCodeAt(2) << 8) | s.charCodeAt(3)) >>> 0;
// BNCS frame: [0xFF][id][len u16 incl header].
export const bncsFrame = (id, body) => {
  const h = Buffer.alloc(4);
  h[0] = 0xff; h[1] = id; h.writeUInt16LE(body.length + 4, 2);
  return Buffer.concat([h, body]);
};
// MCP frame: [len u16 counting itself and the op][op].
export const mcpFrame = (op, body) => {
  const h = Buffer.alloc(3);
  h.writeUInt16LE(body.length + 3, 0); h[2] = op;
  return Buffer.concat([h, body]);
};
