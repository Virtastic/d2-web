// Small per-renderer LRU for derived frames. Rendering can encounter many actor variants over a
// long session; keep the most recently used frames without retaining every one until reload.
export function createRenderCache(limit = 128, maxBytes = 64 * 1024 * 1024, byteSize = () => 0) {
  if (!Number.isInteger(limit) || limit < 1) throw new RangeError('render cache limit must be a positive integer');
  if (!Number.isSafeInteger(maxBytes) || maxBytes < 0 || typeof byteSize !== 'function') throw new RangeError('invalid render cache byte limit');
  const entries = new Map();
  let bytes = 0;
  const removeOldest = () => {
    const [key, entry] = entries.entries().next().value;
    entries.delete(key);
    bytes -= entry.bytes;
  };
  return {
    get(key) {
      const entry = entries.get(key);
      if (entry === undefined) return undefined;
      const value = entry.value;
      entries.delete(key);
      entries.set(key, entry);
      return value;
    },
    set(key, value) {
      if (entries.has(key)) {
        bytes -= entries.get(key).bytes;
        entries.delete(key);
      }
      const size = byteSize(value);
      if (!Number.isSafeInteger(size) || size < 0) throw new RangeError('invalid cached render value size');
      if (size > maxBytes) return value;
      entries.set(key, { value, bytes: size });
      bytes += size;
      while (entries.size > limit || bytes > maxBytes) removeOldest();
      return value;
    },
    get size() { return entries.size; },
    get retainedBytes() { return bytes; },
  };
}
