// F238: the per-sprite identity metrics -- an FNV-1a hash of the indexed plane and the count of
// opaque pixels -- are evidence a row reads back (item-renderer/missile-renderer carry both as
// `pixelHash`/`opaque`; unit-renderer/npc-speech-alert carry the opaque count). Each is a pure
// function of the plane, and each renderer is handed the SAME memoised plane object every draw, so
// the pair is memoised once per plane here instead of recomputed on every blit -- the same guard the
// monster pass already carries (F198). The values are byte-identical to the inline loops they replace,
// so no row that reads them back changes.
//
// `indices` is optional: a caller that only needs the opaque count (unit-renderer, npc-speech-alert)
// passes null and only that count is computed -- this must never do MORE work than the loop it
// replaces, so the hash runs only where the old code ran it.
const metrics = new WeakMap();

export function spriteMetrics(key, indices, opaque) {
  let m = metrics.get(key);
  if (!m) { m = {}; metrics.set(key, m); }
  if (m.opaque === undefined) { let count = 0; for (let i = 0; i < opaque.length; i++) count += opaque[i]; m.opaque = count; }
  if (m.pixelHash === undefined && indices) {
    let hash = 0x811c9dc5;
    for (let i = 0; i < indices.length; i++) { hash ^= indices[i]; hash = Math.imul(hash, 0x01000193); }
    m.pixelHash = hash >>> 0;
  }
  return m;
}
