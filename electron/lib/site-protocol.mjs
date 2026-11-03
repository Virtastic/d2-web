// The app:// answer rules for the desktop shell, as a pure function of a request. No Electron here, so
// node:test runs the same code the shell runs (test/desktop-protocol.test.mjs).
//
// The page is served from app://d2/ and needs what play/serve.mjs gives a browser origin, and nothing else:
//   1. COOP/COEP/CORP, so the page is crossOriginIsolated (SharedArrayBuffer -> pthreads, streamfs)
//   2. HTTP Range (206), so streamfs can read a retail archive in chunks
//   3. a precompressed .br sibling, when the request accepts br and the .br is the file's own brotli
//      (the same content check serve.mjs makes, passed in as canUseBr)
// Retail archives are NOT under the site root: /retail/<name> is answered from the player's own folder
// (retailFiles, a Map of lower-case name -> absolute path), so the package never holds game data.
// door-art/ is cut from the player's archives at run time (lib/derive.mjs) and answered from userData/derived/
// (derivedDir), marked X-D2-Derived; the package never holds it. Absent there, the path 404s like any other.
// site-config.json is answered by the shell: hosted when a retail folder is set, byo otherwise.
import { existsSync, statSync } from 'node:fs';
import { extname, join, resolve, sep } from 'node:path';

export const SCHEME = 'app';
export const HOST = 'd2';
// Registered before the app is ready (protocol.registerSchemesAsPrivileged). fetch() and streams must work on it.
export const PRIVILEGES = { standard: true, secure: true, supportFetchAPI: true, stream: true };

const TYPES = { '.html': 'text/html', '.js': 'text/javascript', '.mjs': 'text/javascript', '.cjs': 'text/javascript',
  '.wasm': 'application/wasm', '.json': 'application/json', '.mpq': 'application/octet-stream', '.txt': 'text/plain',
  '.webmanifest': 'application/manifest+json', '.svg': 'image/svg+xml', '.png': 'image/png', '.jpg': 'image/jpeg',
  '.dc6': 'application/octet-stream', '.patch': 'text/plain', '.md': 'text/markdown' };

const SECURITY = {
  'Cross-Origin-Opener-Policy': 'same-origin',
  'Cross-Origin-Embedder-Policy': 'require-corp',
  'Cross-Origin-Resource-Policy': 'cross-origin',
};

/** The request URL's path as the site sees it, or null when it is not a path this shell answers. */
function parseRel(url) {
  let u;
  try { u = new URL(url); } catch { return { status: 400 }; }
  if (u.protocol !== `${SCHEME}:` || u.hostname !== HOST) return { status: 404 };
  let p;
  try { p = decodeURIComponent(u.pathname); } catch { return { status: 400 }; }
  if (p.includes('\0') || p.includes('\\')) return { status: 404 };
  let rel = p.replace(/^\/+/, '');
  if (rel === '' || rel.endsWith('/')) rel += 'index.html';
  const segs = rel.split('/');
  if (segs.some((s) => s === '' || s === '.' || s === '..')) return { status: 404 };
  return { rel, segs };
}

/** Range header -> [start, end] inclusive; 'bad' for a range the file cannot answer (416); null for none or one this server does not parse (the whole file, as serve.mjs does). */
function parseRange(header, size) {
  const m = /^bytes=(\d*)-(\d*)$/.exec(header || '');
  if (!m || (!m[1] && !m[2])) return null;
  const start = m[1] ? Number(m[1]) : Math.max(0, size - Number(m[2]));
  const end = m[1] && m[2] ? Math.min(Number(m[2]), size - 1) : size - 1;
  if (start > end || start >= size) return 'bad';
  return [start, end];
}

/**
 * createSiteHandler({ siteRoot, retailFiles, siteConfig, canUseBr }) -> plan(request)
 *   siteRoot    absolute path of the staged site (app-site in the package)
 *   derivedDir  () => absolute path of the folder holding the run-time art (door-art/...), or null
 *   retailFiles () => Map<lowercase name, absolute path> | null  (null: no retail folder chosen)
 *   siteConfig  () => object for /site-config.json
 *   canUseBr    (plain, br) => boolean, the content check (play/br-check.mjs brMayAnswer)
 * plan({ url, method, headers }) -> { status, headers, file?, start?, end? } where file/start/end say which
 * bytes to send (none for 304/416/HEAD/errors, and then body is absent). headers keys are lower case in the request.
 */
export function createSiteHandler({ siteRoot, retailFiles = () => null, derivedDir = () => null, siteConfig = () => ({ assets: 'byo' }), canUseBr = () => false }) {
  const root = resolve(siteRoot) + sep;
  const derived = (segs) => {
    const d = derivedDir();
    if (!d) return null;
    const dr = resolve(d) + sep, f = join(dr, ...segs);
    try { return f.startsWith(dr) && existsSync(f) && statSync(f).isFile() ? f : null; } catch { return null; }
  };
  return function plan(req) {
    const headers = Object.fromEntries(Object.entries(req.headers || {}).map(([k, v]) => [k.toLowerCase(), v]));
    const method = (req.method || 'GET').toUpperCase();
    if (method !== 'GET' && method !== 'HEAD') return { status: 405, headers: { Allow: 'GET, HEAD' } };
    const parsed = parseRel(req.url);
    if (parsed.status) return { status: parsed.status, headers: { 'Cache-Control': 'no-store' } };
    const { rel, segs } = parsed;
    const head = method === 'HEAD';

    if (rel === 'site-config.json') {
      const body = JSON.stringify(siteConfig());
      return { status: 200, headers: { 'Content-Type': TYPES['.json'], 'Cache-Control': 'no-cache', 'Content-Length': Buffer.byteLength(body), ...SECURITY }, text: head ? undefined : body };
    }

    let file, br = false;
    if (segs[0] === 'retail') {
      if (segs.length !== 2) return { status: 404 };
      const map = retailFiles();
      file = map && map.get(segs[1].toLowerCase());
      if (!file || !/\.mpq$/i.test(segs[1])) return { status: 404 };
    } else if (segs[0] === 'door-art' && derived(segs)) {
      file = derived(segs);
    } else {
      file = join(root, ...segs);
      if (!file.startsWith(root) || !existsSync(file) || !statSync(file).isFile()) return { status: 404 };
      const cand = file + '.br';
      if (!headers.range && /\bbr\b/.test(headers['accept-encoding'] || '') && existsSync(cand) && canUseBr(file, cand)) {
        file = cand; br = true;
      }
    }

    const st = statSync(file), size = st.size;
    const h = {
      ...(segs[0] === 'retail' ? {} : SECURITY),
      'Content-Type': TYPES[extname(segs[segs.length - 1]).toLowerCase()] || 'application/octet-stream',
      'Cache-Control': segs[0] === 'retail' ? 'no-cache' : (rel.startsWith('e/') ? 'public, max-age=31536000, immutable' : 'no-cache'),
      'Accept-Ranges': 'bytes',
      ETag: `W/"${size.toString(36)}-${Math.floor(st.mtimeMs).toString(36)}"`,
      'Last-Modified': st.mtime.toUTCString(),
    };
    if (segs[0] === 'door-art' && file !== join(root, ...segs)) h['X-D2-Derived'] = '1';
    if (segs[0] !== 'retail') h.Vary = 'Accept-Encoding';
    if (br) h['Content-Encoding'] = 'br';

    const inm = headers['if-none-match'];
    if (inm ? inm.split(/\s*,\s*/).includes(h.ETag)
        : headers['if-modified-since'] && Date.parse(headers['if-modified-since']) >= Math.floor(st.mtimeMs / 1000) * 1000) {
      delete h['Content-Type']; delete h['Accept-Ranges'];
      return { status: 304, headers: h };
    }

    const range = br ? null : parseRange(headers.range, size);
    if (range === 'bad') return { status: 416, headers: { 'Content-Range': `bytes */${size}` } };
    const [start, end] = range || [0, size - 1];
    h['Content-Length'] = end - start + 1;
    if (range) h['Content-Range'] = `bytes ${start}-${end}/${size}`;
    return { status: range ? 206 : 200, headers: h, file, start, end, head };
  };
}
