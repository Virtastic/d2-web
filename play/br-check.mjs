// Does <file>.br really hold the brotli of <file>?
//
// One question, asked in the three places that used to answer it with a file's mtime:
//   - wasm-build/make_br.sh   -- "is the .br already fresh?"  (skip recompressing)
//   - ci/stage.sh             -- "is this .br safe to stage?" (it goes beside the wasm in e/<ver>/)
//   - play/serve.mjs          -- "may I answer with the .br?" (Content-Encoding: br)
//
// mtime is a proxy for content and it is NOT one. An engine put into play/ by hand -- `cp`, `rsync
// -a`, a docker cp, anything that preserves the source's mtime -- leaves a wasm whose mtime is OLDER
// than the .br already sitting beside it. All three tests then read "fresh"/"newer"/"not older", the
// stale .br is staged into the content-addressed `e/<ver>/` (the version hashes the *plain* files)
// and the browser is handed the OLD engine's bytes behind a `max-age=31536000, immutable` header, on
// a URL that names the NEW build. Nothing throws: the page boots, on the wrong engine. It cost an
// afternoon of measurements on 2026-09-26 before anyone noticed.
//
// So the association is read from the bytes: the .br must decompress to the file it sits beside,
// exactly. That is the whole check. `brMatches` is it; `brMayAnswer` is the same question for
// serve.mjs, which asks it on every request and memoizes the verdict -- by CONTENT, for the same
// reason (a key made of timestamps and sizes reuses a stale verdict; see brMayAnswer).
//
// CLI: node play/br-check.mjs <plain> <br>   -> exit 0 if it matches, 1 if it does not, 2 on usage.
import { readFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { brotliDecompressSync } from 'node:zlib';
import { pathToFileURL } from 'node:url';

/** True only when `br` brotli-decompresses to exactly the bytes of `plain`. */
export function brMatches(plain, br) {
  let plainBytes, brBytes;
  try {
    plainBytes = readFileSync(plain);
    brBytes = readFileSync(br);
  } catch {
    return false;                       // no .br (or an unreadable one) is never a match
  }
  let out;
  try {
    out = brotliDecompressSync(brBytes);
  } catch {
    return false;                       // not brotli at all, or truncated: not this file's .br
  }
  return out.length === plainBytes.length && out.equals(plainBytes);
}

/** The CONTENT identity of one or more files: sha256 over their raw bytes, or null if one cannot be
 *  read. Never a size, never an mtime -- two files share those by accident, which is the whole trap. */
export function contentKey(...paths) {
  const h = createHash('sha256');
  for (const p of paths) {
    let bytes;
    try { bytes = readFileSync(p); } catch { return null; }
    h.update(`${bytes.length}\n`); h.update(bytes);     // length-framed, so two files cannot line up
  }
  return h.digest('hex');
}

/** `brMatches` for a caller that asks once per request: decompressing 1.8 MB of brotli every time is
 *  the cost this avoids, so the answer is memoized -- per CONTENT, never per file version. The memo's
 *  key is the content identity of BOTH files, because the verdict is a pure function of those two byte
 *  strings and of nothing else. A timestamp key only looks cheaper: a same-size rewrite that put the
 *  mtime back (a `cp`, an rsync, anything that writes in place) reused the cached "yes" for a `.br`
 *  that no longer decompressed to the file beside it, and the server went on handing out the OLD build
 *  behind `immutable`. Every memo hit still reads and hashes both files (3.3 ms on d2.wasm +
 *  d2.wasm.br here, against 9.7 ms for the decompression it saves), so a changed byte can never reuse
 *  a stale verdict. */
const verdicts = new Map();             // br path -> { key, ok }
export function brMayAnswer(plain, br) {
  const key = contentKey(plain, br);
  if (key === null) return false;       // unreadable is not a match, exactly as brMatches says
  const hit = verdicts.get(br);
  if (hit && hit.key === key) return hit.ok;
  const ok = brMatches(plain, br);
  verdicts.set(br, { key, ok });
  if (!ok) console.warn(`serve.mjs: ${br} does not decompress to ${plain} -- serving the plain file (play/br-check.mjs)`);
  return ok;
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  const [plain, br] = process.argv.slice(2);
  if (!plain || !br) {
    console.error('usage: node play/br-check.mjs <plain> <br>   (exit 0 = the .br is that file)');
    process.exit(2);
  }
  process.exit(brMatches(plain, br) ? 0 : 1);
}
