const TERMINAL = new Set([2, 3, 4]); // native collision, target hit, lifetime expiry

// Engine records are {version,tick,missileId,classId,event,soundRow}, six u32s.
export function parseRealmMissileSoundEvents(bytes, count) {
  const view = bytes instanceof DataView
    ? bytes
    : new DataView(bytes.buffer, bytes.byteOffset || 0, bytes.byteLength);
  if (!Number.isInteger(count) || count < 0 || view.byteLength < count * 24)
    throw new RangeError('truncated missile sound records');
  return Array.from({ length: count }, (_, i) => {
    const at = i * 24;
    const event = { version: view.getUint32(at, true), tick: view.getUint32(at + 4, true),
      missileId: view.getUint32(at + 8, true), classId: view.getUint32(at + 12, true),
      event: view.getUint32(at + 16, true), soundRow: view.getUint32(at + 20, true) };
    if (event.version !== 1 || event.classId !== 62 || !TERMINAL.has(event.event) || event.soundRow === 0xffffffff)
      throw new Error(`invalid Fireball impact sound record ${i}`);
    return event;
  });
}
