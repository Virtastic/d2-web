/** Create a copying reader for the engine's layered retail MPQ VFS. */
export function createMpqAssetReader(Module) {
  if (!Module?.ccall || !Module?.HEAPU8) throw new TypeError('an initialized d2 wasm Module is required');
  return function readMpqAsset(path) {
    if (typeof path !== 'string' || path.length === 0) throw new TypeError('asset path must be a non-empty string');
    const ok = Module.ccall('d2_asset_read', 'number', ['string'], [path]);
    if (!ok) throw new Error(Module.ccall('d2_asset_error', 'string', [], []) || `asset read failed: ${path}`);
    const length = Module.ccall('d2_asset_len', 'number', [], []) >>> 0;
    const pointer = Number(Module.ccall('d2_asset_ptr', 'bigint', [], []));
    if (length && (!Number.isSafeInteger(pointer) || pointer <= 0 || pointer > Module.HEAPU8.length - length)) {
      throw new RangeError(`asset buffer outside wasm memory: ${path}`);
    }
    return length ? Module.HEAPU8.slice(pointer, pointer + length) : new Uint8Array();
  };
}
