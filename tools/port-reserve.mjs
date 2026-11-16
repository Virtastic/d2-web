// Reserve a local TCP port across concurrent processes on one box.
//
// Asking the kernel for a free port (listen on 0, then close) is a probe, not a reservation: two processes that
// probe before either binds can be handed the same port, and a port inherited through the environment is shared by
// every child. A reservation here is a lock file <dir>/<port> created exclusively and holding the owner's pid; a
// file whose pid no longer runs is stale and is taken over. The port must also bind at the moment it is reserved.
//
//   const port = reservePort({ dir, preferred });   // released when the process exits
import { mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { spawnSync } from 'node:child_process';
import { join } from 'node:path';

const alive = (pid) => { try { process.kill(pid, 0); return true; } catch (e) { return e.code === 'EPERM'; } };

// 0 asks the kernel for any free port; returns the port bound (and released), or 0 when the bind failed.
export function probePort(port = 0, host = '127.0.0.1') {
  const r = spawnSync(process.execPath, ['-e', `const s=require('net').createServer();s.on('error',()=>process.exit(1));s.listen(${Number(port)},${JSON.stringify(host)},()=>{console.log(s.address().port);s.close()})`], { encoding: 'utf8' });
  return r.status === 0 ? Number(r.stdout.trim()) || 0 : 0;
}

// Take <dir>/<port> for `pid`. False when another live process holds it.
export function tryLock(dir, port, pid = process.pid) {
  mkdirSync(dir, { recursive: true });
  const f = join(dir, String(port));
  for (let i = 0; i < 2; i++) {
    try { writeFileSync(f, String(pid), { flag: 'wx' }); return true; } catch (e) { if (e.code !== 'EEXIST') throw e; }
    const owner = Number((() => { try { return readFileSync(f, 'utf8').trim(); } catch { return ''; } })());
    if (owner === pid) return true;
    if (owner > 0 && alive(owner)) return false;
    rmSync(f, { force: true });   // stale: its owner is gone
  }
  return false;
}

export function releasePort(dir, port, pid = process.pid) {
  const f = join(dir, String(port));
  try { if (Number(readFileSync(f, 'utf8').trim()) === pid) rmSync(f, { force: true }); } catch {}
}

// The preferred port when it is unreserved and binds, else a fresh kernel-chosen one. Throws after `tries` misses.
export function reservePort({ dir, preferred = 0, pid = process.pid, probe = probePort, tries = 20, release = true } = {}) {
  const take = (p) => {
    if (!p || !tryLock(dir, p, pid)) return 0;
    if (probe(p) === p) { if (release) process.once('exit', () => releasePort(dir, p, pid)); return p; }
    releasePort(dir, p, pid); return 0;
  };
  const want = Number(preferred) || 0;
  if (want && take(want)) return want;
  for (let i = 0; i < tries; i++) { const p = take(probe(0)); if (p) return p; }
  throw new Error(`port-reserve: no free port after ${tries} tries (${dir})`);
}
