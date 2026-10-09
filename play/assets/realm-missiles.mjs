// Read-only canonical client CUnits. These are local skill effects, not D2GS assigned objects.
export function parseRealmMissiles(bytes, count) {
  if (!Number.isInteger(count) || count < 0 || bytes.byteLength !== count * 76)
    throw new RangeError('invalid client missile record span');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  return Array.from({ length: count }, (_, row) => {
    const at = row * 76, read = (word) => view.getUint32(at + word * 4, true);
    if (read(0) !== 1 || read(1) !== 76) throw new Error(`invalid client missile record ${row}`);
    return { id: read(2), classId: read(3), mode: read(4), framePrecise: read(5), frameCountPrecise: read(6),
      animSpeed: read(7), direction: read(8), trans: read(9), preciseX: read(10), preciseY: read(11),
      x: read(10) / 65536, y: read(11) / 65536, level: read(12), ownerGuid: read(13), skill: read(14), remaining: read(15), initSeed: read(16), seedLow: read(17), seedHigh: read(18) };
  });
}
