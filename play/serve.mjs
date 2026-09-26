// Dev/harness static server for play/. The three things the engine needs from any origin, and
// nothing else (openmw-web's server.py grew MP proxying and S3; none of that is wanted here):
//   1. COOP/COEP so the page is crossOriginIsolated (SharedArrayBuffer -> pthreads, streamfs)
//   2. HTTP Range so streamfs can fetch chunks
//   3. a precompressed .br sibling served when the client accepts it
// node play/serve.mjs [port] [dir]   (default 8920, this dir; 127.0.0.1 only -- no auth)
// D2_RETAIL_DIR=<dir with the retail *.mpq> additionally serves them under /retail/ so the page
// can streamfs-mount them (local/harness use; the deployed site never hosts retail data).
// ci/build.sh stages a versioned copy in build-wasm/site/ and serves THAT, so
// version-engine.sh never rewrites the tracked play/index.html.
import { createServer } from 'node:http';
import { createReadStream, statSync, existsSync } from 'node:fs';
import { join, extname, normalize, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = process.argv[3] ? resolve(process.argv[3]) + sep : fileURLToPath(new URL('.', import.meta.url));
const PORT = Number(process.argv[2] || process.env.PORT || 8920);
const RETAIL = process.env.D2_RETAIL_DIR ? resolve(process.env.D2_RETAIL_DIR) + sep : null;
const TYPES = { '.html': 'text/html', '.js': 'text/javascript', '.mjs': 'text/javascript', '.wasm': 'application/wasm',
  '.json': 'application/json', '.mpq': 'application/octet-stream', '.txt': 'text/plain',
  '.webmanifest': 'application/manifest+json', '.svg': 'image/svg+xml', '.png': 'image/png' };

createServer((req, res) => {
  const url = new URL(req.url, 'http://x');
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
      && statSync(br).mtimeMs >= statSync(path).mtimeMs) { file = br; h['Content-Encoding'] = 'br'; }
  const size = statSync(file).size;
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
}).listen(PORT, '127.0.0.1', () => console.log(`play/ on http://127.0.0.1:${PORT}/`));
