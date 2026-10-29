export function createMpqAssetReader(Module) {
  if (!Module?.ccall || !Module?.HEAPU8) throw new TypeError('an initialized d2 wasm Module is required');
  const heap = (path) => {
    const length = Module.ccall('d2_asset_len', 'number', [], []) >>> 0;
    const pointer = Number(Module.ccall('d2_asset_ptr', 'bigint', [], []));
    if (length && (!Number.isSafeInteger(pointer) || pointer <= 0 || pointer > Module.HEAPU8.length - length)) {
      throw new RangeError(`asset buffer outside wasm memory: ${path}`);
    }
    return length ? Module.HEAPU8.subarray(pointer, pointer + length) : new Uint8Array();
  };
  const fail = (path) => new Error(Module.ccall('d2_asset_error', 'string', [], []) || `asset read failed: ${path}`);
  function readMpqAsset(path) {
    if (typeof path !== 'string' || path.length === 0) throw new TypeError('asset path must be a non-empty string');
    const ok = Module.ccall('d2_asset_read', 'number', ['string'], [path]);
    if (!ok) throw fail(path);
    return heap(path).slice();
  }
  // The same bytes read in parts with a yield between them: StormLib decompresses a music track's sectors on
  // the main thread, and read whole a 6 MB track was a 1.1 s freeze. A build without the part exports reads whole.
  readMpqAsset.sliced = async function readMpqAssetSliced(path, part = 64 * 1024) {
    if (typeof Module._d2_asset_open !== 'function') return readMpqAsset(path);
    if (typeof path !== 'string' || path.length === 0) throw new TypeError('asset path must be a non-empty string');
    const h = Module.ccall('d2_asset_open', 'number', ['string'], [path]);
    if (!h) throw fail(path);
    try {
      const size = Module.ccall('d2_asset_size', 'number', ['number'], [h]) >>> 0;
      if (size === 0xFFFFFFFF) throw fail(path);
      const out = new Uint8Array(size);
      for (let pos = 0; pos < size; pos += part) {
        if (pos) await (globalThis.scheduler?.yield?.() ?? new Promise((r) => setTimeout(r, 0)));
        const n = Math.min(part, size - pos);
        if (!Module.ccall('d2_asset_part', 'number', ['number', 'number'], [h, n])) throw fail(path);
        out.set(heap(path), pos);
      }
      return out;
    } finally { Module.ccall('d2_asset_close', null, ['number'], [h]); }
  };
  return readMpqAsset;
}
