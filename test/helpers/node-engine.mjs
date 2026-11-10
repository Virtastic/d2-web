// Loads the staged engine (play/d2.js + play/d2.wasm, a browser build) inside node, for tests that drive the
// engine's exported functions with no page. The engine script only needs enough of a page to believe it is
// in one; its worker pool is answered by a stand-in that reports "loaded" and never runs a thread.
import { readFileSync, openSync, readSync, fstatSync, existsSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const REPO = join(dirname(fileURLToPath(import.meta.url)), '..', '..');
export const ENGINE_DIR = process.env.D2_ENGINE_DIR || join(REPO, 'play');
export const ARCHIVE_DIR = process.env.D2_ARCHIVE_DIR || join(REPO, 'build-wasm', 'retail');
export const ARCHIVES = ['patch_d2.mpq', 'd2exp.mpq', 'd2data.mpq', 'd2char.mpq'];
export const haveEngine = () => existsSync(join(ENGINE_DIR, 'd2.js')) && existsSync(join(ENGINE_DIR, 'd2.wasm'));
export const haveArchives = () => ARCHIVES.every((f) => existsSync(join(ARCHIVE_DIR, f)));

// A host file exposed at `path` in the engine's filesystem, read on demand (the archives are too big to copy in).
function mountFile(Module, path, hostPath) {
  const FS = Module.__FS, fd = openSync(hostPath, 'r'), size = fstatSync(fd).size;
  const name = path.slice(path.lastIndexOf('/') + 1), dir = path.slice(0, path.lastIndexOf('/')) || '/';
  try { FS.mkdir(dir); } catch { /* exists */ }
  const node = FS.createFile(dir, name, {}, true, false);
  node.usedBytes = size;
  const getattr = node.node_ops.getattr;
  node.node_ops = { ...node.node_ops, getattr(n) { const a = getattr(n); a.size = size; return a; } };
  node.stream_ops = { ...node.stream_ops,
    llseek(stream, off, whence) { return whence === 1 ? off + stream.position : whence === 2 ? off + size : off; },
    read(stream, buffer, offset, length, position) {
      const n = Math.max(0, Math.min(length, size - position)), tmp = Buffer.alloc(n), got = readSync(fd, tmp, 0, n, position);
      buffer.set(tmp.subarray(0, got), offset);
      return got;
    },
    mmap() { throw new FS.ErrnoError(52); } };
}

// A cold engine. `archives: true` mounts the game data the way the page does before a game starts.
// `log` receives every line the engine prints (an assertion's file and line arrive there before the abort).
export async function loadEngine({ archives = false, log = () => {} } = {}) {
  const js = readFileSync(join(ENGINE_DIR, 'd2.js'), 'utf8'), wasm = readFileSync(join(ENGINE_DIR, 'd2.wasm'));
  const href = pathToFileURL(join(ENGINE_DIR, 'd2.js')).href;
  class Worker {
    onmessage = null; onerror = null;
    postMessage(m) { if (m && m.cmd === 1) queueMicrotask(() => this.onmessage?.({ data: { cmd: 3 } })); }   // load, answered loaded
    terminate() {} addEventListener() {} removeEventListener() {}
  }
  const Module = await new Promise((resolve, reject) => {
    const Module = { print: log, printErr: log, noInitialRun: true, locateFile: (p) => join(ENGINE_DIR, p),
      instantiateWasm: (info, done) => { WebAssembly.instantiate(wasm, info).then((r) => done(r.instance, r.module)).catch(reject); return {}; },
      onRuntimeInitialized() { resolve(Module); }, onAbort: (what) => reject(new Error('engine aborted while loading: ' + what)) };
    const window = { Module, location: { href, search: '' } };
    const document = { currentScript: { src: href }, addEventListener() {}, createElement: () => ({ getContext: () => null, style: {} }) };
    const run = new Function('Module', 'window', 'document', 'Worker', 'self', 'navigator', 'process', js + '\n;Module.__FS = FS; return Module;');
    const saved = globalThis.window;
    globalThis.window = window;   // the engine script decides it is on a page from globalThis.window
    try { run(Module, window, document, Worker, window, { hardwareConcurrency: 2, userAgent: 'node' }, undefined); }
    catch (e) { reject(e); }
    finally { if (saved === undefined) delete globalThis.window; else globalThis.window = saved; }
  });
  if (archives) for (const f of ARCHIVES) {
    mountFile(Module, '/retail/' + f, join(ARCHIVE_DIR, f));
    if (!Module.ccall('d2_vfs_mount', 'number', ['string'], ['/retail/' + f])) throw new Error('engine refused archive ' + f);
  }
  return Module;
}
