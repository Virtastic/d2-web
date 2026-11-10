// A joined game's objects as the scene reads them. The engine's object unit is the one model: the server's object
// packets go to the engine only (src/core/realm/d2gs_client_object.cpp), and each frame reads the units back in
// one call. Nothing here keeps an object, parses a server packet or looks a class up in a table.
import { parseObjectPresentations } from './object-renderer.mjs';

/**
 * One unit of d2_realm_client_object_units: the 44-byte object record a local game's objects are read in (id,
 * class, art token, mode, position, cel clock, facing, pick flag, portal level, lock, room), then seven int32:
 * the overlay count, three overlay rows and the frames each overlay has looped.
 */
export const OBJECT_UNIT_BYTES = 72;
export function parseRealmObjectUnits(bytes, count) {
  if (!Number.isInteger(count) || count < 0 || bytes.byteLength < count * OBJECT_UNIT_BYTES) throw new RangeError('truncated object unit records');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  return parseObjectPresentations(view, count, OBJECT_UNIT_BYTES).map((object, row) => {
    const at = row * OBJECT_UNIT_BYTES + 44, n = view.getInt32(at, true);
    if (n < 0 || n > 3) throw new RangeError(`object ${object.id} reports ${n} overlays`);
    const words = (from) => Array.from({ length: n }, (_, i) => view.getInt32(at + from + i * 4, true));
    return { ...object, overlays: words(4), overlayAges: words(16) };
  });
}
/** The engine's pending object activations (d2_realm_client_take_object_events): 6 int32 words each. */
export function parseRealmObjectEvents(words, count) {
  const events = [];
  for (let i = 0; i < count; i++) {
    if (words[i * 6] !== 1) throw new Error(`object event version ${words[i * 6]}`);
    events.push({ objectId: words[i * 6 + 1] >>> 0, playerId: words[i * 6 + 2] >>> 0, shrine: words[i * 6 + 3], soundRow: words[i * 6 + 4], effect: words[i * 6 + 5] });
  }
  return events;
}
