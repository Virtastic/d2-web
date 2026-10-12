// ABI mirror of D2WebWalkProbe in d2moo/include/Path/Path.h. Every slot is int32 so this
// diagnostic can be copied safely from wasm memory without exposing host pointers.
export const WALK_PROBE_WORDS = 29;
export function decodeWalkProbe(words) {
  if (!words || words.length !== WALK_PROBE_WORDS) throw new RangeError(`walk probe requires ${WALK_PROBE_WORDS} int32 words`);
  const v = Array.from(words, Number);
  return {
    sequence: v[0], command: [v[1], v[2]], commandStart: [v[3], v[4]],
    wrapperRoomFound: Boolean(v[5]), packetBuilt: Boolean(v[6]), packet: v.slice(7, 12),
    handlerResult: v[12], moveReason: v[13], pathPoints: v[14], pathIndex: v[15],
    resolver: { called: Boolean(v[16]), roomFound: Boolean(v[17]), level: v[18], pathType: v[19],
      start: [v[20], v[21]], target: [v[22], v[23]] },
    continuation: { ticks: v[24], x: v[25], y: v[26], level: v[27], pending: Boolean(v[28]) },
  };
}
