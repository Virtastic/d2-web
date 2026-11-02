// Dev/harness static server for play/. The three things the engine needs from any origin, and
// nothing else (openmw-web's server.py grew MP proxying and S3; none of that is wanted here):
//   1. COOP/COEP so the page is crossOriginIsolated (SharedArrayBuffer -> pthreads, streamfs)
//   2. HTTP Range so streamfs can fetch chunks
//   3. a precompressed .br sibling served when the client accepts it
// node play/serve.mjs [port] [dir]   (default 8920, this dir; 127.0.0.1 only unless D2_BIND=0.0.0.0 -- no auth)
// D2_RETAIL_DIR=<dir with the retail *.mpq> additionally serves them under /retail/ so the page
// can streamfs-mount them (local/harness use; the deployed site never hosts retail data).
// ci/build.sh stages a versioned copy in build-wasm/site/ and serves THAT, so
// version-engine.sh never rewrites the tracked play/index.html.
import { createServer as createHttp } from 'node:http';
import { createServer as createHttps } from 'node:https';
import { createServer as createNet } from 'node:net';
import { readFileSync } from 'node:fs';
// D2_TLS_DIR=<dir with key.pem + cert.pem> serves HTTPS (a secure context, so the game can be cross-origin isolated from a LAN address).
const TLS = process.env.D2_TLS_DIR ? { key: readFileSync(join(process.env.D2_TLS_DIR, 'key.pem')), cert: readFileSync(join(process.env.D2_TLS_DIR, 'cert.pem')) } : null;
// With TLS, a plain-http request on the same port (an address typed without https://) gets a redirect
// to https instead of an empty reply, which phone browsers report as a connection failure.
const createServer = (h) => {
  if (!TLS) return createHttp(h);
  const secure = createHttps(TLS, h);
  const plain = createHttp((req, res) => { res.writeHead(301, { Location: `https://${req.headers.host || 'localhost'}${req.url}` }); res.end(); });
  return createNet((s) => s.once('readable', () => {
    const b = s.read(1);
    if (!b) return s.destroy();
    s.unshift(b);
    (b[0] === 0x16 ? secure : plain).emit('connection', s);
  }));
};
import { createReadStream, statSync, existsSync } from 'node:fs';
import { join, extname, normalize, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';
import { brMayAnswer } from './br-check.mjs';

// May this .br answer for `path`? By CONTENT, never by mtime -- and the verdict memo is keyed by
// content too, in the same module as the check (play/br-check.mjs): the key that used to sit here was
// `brMtimeMs:brSize:fileMtimeMs:fileSize`, which a same-size rewrite with its mtime put back
// reproduces exactly, so the cached "yes" answered for a .br that no longer decompressed to the file
// beside it and the OLD build went out behind e/<ver>/'s `max-age=31536000, immutable`. A key that
// cannot collide is the fix; dropping the memo is not (a 1.8 MB decompression per request is what it
// saves). `brMayAnswer` reads and hashes both files per request -- never a stale verdict.

const ROOT = process.argv[3] ? resolve(process.argv[3]) + sep : fileURLToPath(new URL('.', import.meta.url));
const PORT = Number(process.argv[2] || process.env.PORT || 8920);
// A bring-your-own-files site (site-config.json {"assets":"byo"}) serves no game data, whatever D2_RETAIL_DIR says.
const BYO = (() => { try { return JSON.parse(readFileSync(join(ROOT, 'site-config.json'), 'utf8')).assets === 'byo'; } catch { return false; } })();
const RETAIL = process.env.D2_RETAIL_DIR && !BYO ? resolve(process.env.D2_RETAIL_DIR) + sep : null;
const TYPES = { '.html': 'text/html', '.js': 'text/javascript', '.mjs': 'text/javascript', '.wasm': 'application/wasm',
  '.json': 'application/json', '.mpq': 'application/octet-stream', '.txt': 'text/plain',
  '.webmanifest': 'application/manifest+json', '.svg': 'image/svg+xml', '.png': 'image/png' };

createServer((req, res) => {
  const url = new URL(req.url, 'http://x');
  // Which site this server is serving. A harness reuses a server already on its port only when it
  // serves THAT test runner's staged site; two worktrees that stage the same engine build have
  // byte-identical index.html (version-engine.sh hashes only wasm/loader/streamfs), so comparing
  // index.html cannot tell them apart and the test runner would silently measure the other worktree's
  // client JS. The root is the answer that cannot be forged by a coincidence of build hashes.
  // Divergence from openmw-web's serve.mjs (which has no such probe); see d2-harness.mjs ensurePlayServer.
  if (url.pathname === '/__site') {
    res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8', 'Cache-Control': 'no-store' });
    return res.end(ROOT);
  }
  // Crawler files, built from the request's own host so the image serves any domain without a rebuild.
  if (url.pathname === '/robots.txt' || url.pathname === '/sitemap.xml') {
    const origin = `${req.headers['x-forwarded-proto'] || (TLS ? 'https' : 'http')}://${req.headers.host}`;
    const xml = url.pathname === '/sitemap.xml';
    res.writeHead(200, { 'Content-Type': xml ? 'application/xml' : 'text/plain; charset=utf-8', 'Cache-Control': 'public, max-age=86400' });
    return res.end(xml ? `<?xml version="1.0" encoding="UTF-8"?>\n<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9"><url><loc>${origin}/</loc></url></urlset>\n`
      : `User-agent: *\nAllow: /\n\nSitemap: ${origin}/sitemap.xml\n`);
  }
  let rel = normalize(decodeURIComponent(url.pathname)).replace(/^([/\\])+/, '');
  if (rel === '' || rel.endsWith('/') || rel.endsWith('\\')) rel += 'index.html';
  let base = ROOT;
  if (RETAIL && /^retail[\/\\]/.test(rel)) { base = RETAIL; rel = rel.slice(7); }
  const path = join(base, rel);
  if (!path.startsWith(base) || !existsSync(path) || !statSync(path).isFile()) { res.writeHead(404); return res.end('404'); }
  const h = {
    'Cross-Origin-Opener-Policy': 'same-origin',
    'Cross-Origin-Embedder-Policy': 'require-corp',
    'Cross-Origin-Resource-Policy': 'cross-origin',
    'Content-Type': TYPES[extname(rel).toLowerCase()] || 'application/octet-stream',
    // e/<hash>/ is content-addressed -> immutable; everything else (index.html) must revalidate.
    'Cache-Control': rel.startsWith('e/') || rel.startsWith('e\\') ? 'public, max-age=31536000, immutable' : 'no-cache',
    'Accept-Ranges': 'bytes',
  };
  let file = path;
  const br = path + '.br';
  if (!req.headers.range && /\bbr\b/.test(req.headers['accept-encoding'] || '') && existsSync(br)
      && brMayAnswer(path, br)) { file = br; h['Content-Encoding'] = 'br'; }
  const st = statSync(file), size = st.size;
  // Validators, so a revalidation (the browser's, or the CDN's after its edge TTL) is a 304, not the whole file.
  h['ETag'] = `W/"${size.toString(36)}-${Math.floor(st.mtimeMs).toString(36)}"`;
  h['Last-Modified'] = st.mtime.toUTCString();
  if (req.headers['if-none-match'] ? req.headers['if-none-match'].split(/\s*,\s*/).includes(h['ETag'])
      : req.headers['if-modified-since'] && Date.parse(req.headers['if-modified-since']) >= Math.floor(st.mtimeMs / 1000) * 1000) {
    for (const k of ['Content-Type', 'Accept-Ranges']) delete h[k];
    res.writeHead(304, h); return res.end();
  }
  const m = /^bytes=(\d*)-(\d*)$/.exec(req.headers.range || '');
  if (m && file === path) {
    const start = m[1] ? Number(m[1]) : Math.max(0, size - Number(m[2]));
    const end = m[1] && m[2] ? Math.min(Number(m[2]), size - 1) : size - 1;
    if (start > end || start >= size) { res.writeHead(416, { 'Content-Range': `bytes */${size}` }); return res.end(); }
    h['Content-Range'] = `bytes ${start}-${end}/${size}`; h['Content-Length'] = end - start + 1;
    res.writeHead(206, h);
    return createReadStream(file, { start, end }).pipe(res);
  }
  h['Content-Length'] = size;
  res.writeHead(200, h);
  if (req.method === 'HEAD') return res.end();
  createReadStream(file).pipe(res);
}).listen(PORT, process.env.D2_BIND || '127.0.0.1', () => console.log(`play/ on ${TLS ? 'https' : 'http'}://${process.env.D2_BIND || '127.0.0.1'}:${PORT}/`));
