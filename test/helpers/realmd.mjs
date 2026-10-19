// Shared plumbing for the tests that drive realmd over a real socket.
// Lives under test/helpers/ so `node --test test/*.test.mjs` does not try to run it as a suite.
import { spawn, spawnSync } from 'node:child_process';
import net from 'node:net';
import { existsSync } from 'node:fs';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

export const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
// Build realmd and start it on the given ports (zero asks the OS to allocate a free port).
// Updates ports from readiness while the listeners remain bound, with no reservation race.
// Returns null only when the compiler is absent, so callers can skip that genuine prerequisite. Build and startup errors are test failures: in
// particular, a bind conflict must not turn protocol coverage into a green SKIP.
//
// `tag` and PID give each test file/process its OWN binary. node --test runs files in parallel, so a shared output
// path means one file's g++ rewrites the binary another file is currently executing -- which fails
// as ETXTBSY, intermittently and confusingly.
export async function startRealmd(ports, tag = 'test', extraArgs = []) {
  const BIN = join(ROOT, 'build-wasm', `realmd-${tag}-${process.pid}`);
  const compiler = spawnSync('g++', ['--version'], { encoding: 'utf8' });
  if (compiler.error?.code === 'ENOENT') return null;
  if (compiler.status !== 0) {
    throw new Error(`g++ --version failed (${compiler.status ?? compiler.error?.message}): ${compiler.stderr ?? ''}`);
  }
  const build = spawnSync('bash', [join(ROOT, 'ci', 'build-realmd.sh'), BIN], { encoding: 'utf8' });
  if (build.status !== 0) {
    throw new Error(`realmd build failed (${build.status ?? build.error?.message}):\n${build.stdout ?? ''}${build.stderr ?? ''}`);
  }
  if (!existsSync(BIN)) throw new Error(`realmd build succeeded but did not create ${BIN}`);
  const proc = spawn(BIN, ['--bncs', String(ports.bncs), '--mcp', String(ports.mcp), '--d2gs', String(ports.d2gs), ...extraArgs],
    { stdio: ['ignore', 'pipe', 'pipe'] });
  // realmd announces readiness on stdout, so wait for that rather than sleeping. A premature
  // exit or timeout is an unexpected server failure, not an unavailable prerequisite.
  let stdout = '';
  let stderr = '';
  const ok = await new Promise((resolve) => {
    const t = setTimeout(() => resolve(false), 8000);
    proc.stdout.on('data', (d) => {
      stdout += d.toString();
      const ready = stdout.match(/realmd listening bncs=(\d+) mcp=(\d+) d2gs=(\d+)\n/);
      if (ready) {
        Object.assign(ports, { bncs: Number(ready[1]), mcp: Number(ready[2]), d2gs: Number(ready[3]) });
        clearTimeout(t); resolve(true);
      }
    });
    proc.stderr.on('data', (d) => { stderr += d.toString(); });
    proc.once('error', (error) => { clearTimeout(t); resolve(error); });
    proc.once('exit', (code, signal) => { clearTimeout(t); resolve({ code, signal }); });
  });
  if (ok !== true) {
    if (proc.exitCode === null && !proc.killed) proc.kill('SIGKILL');
    const reason = ok instanceof Error ? ok.message
      : ok === false ? 'timed out waiting for readiness'
        : `exited with code ${ok.code}${ok.signal ? ` (signal ${ok.signal})` : ''}`;
    throw new Error(`realmd ${tag} failed to start: ${reason}\nstdout: ${stdout}\nstderr: ${stderr}`);
  }
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
