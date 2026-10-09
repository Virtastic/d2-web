// D2Client 1.13c wall payload +0x24 bit 0 selects the alternate row.
// DT1 +0 supplies direction; its transparency color does not select lighting.
// for producer and caller evidence.
const patterns = [
  [ [[2,5],[2,3],[2,1],[2,-1],[2,-3],[2,-5]], [[-2,5],[-2,3],[-2,1],[-2,-1],[-2,-3],[-2,-5]] ],
  [ [[-5,2],[-3,2],[-1,2],[1,2],[3,2],[5,2]], [[-5,-2],[-3,-2],[-1,-2],[1,-2],[3,-2],[5,-2]] ],
  [ [[2,5],[2,3],[2,2],[2,2],[3,2],[5,2]], [[-2,5],[-2,3],[-2,-2],[-2,-2],[3,-2],[5,-2]] ],
  [ [[-5,2],[-3,2],[-1,2],[2,-1],[2,-3],[2,-5]], [[-5,-2],[-3,-2],[-1,-2],[-2,-1],[-2,-3],[-2,-5]] ],
];

/** Native cells relative to the world-subtile tile corner; null for unrecovered metadata. */
export function nativeWallLightCell(dt1Entry, hidden, A, B, sample) {
  const d = dt1Entry?.direction;
  if (!Number.isInteger(d) || d < 1 || d > 9 || !Number.isInteger(sample) || sample < 0 || sample > 5) return null;
  if (d === 5) return [A, B];
  const [dx, dy] = patterns[(d - 1) % 5][hidden ? 1 : 0][sample];
  return [A + dx, B + dy];
}

/** Native six-cell wall sample set, or null when the DT1 direction has no recovered row. */
export function nativeWallLightCells(dt1Entry, hidden, A, B) {
  const cells = [];
  for (let sample = 0; sample < 6; sample++) {
    const cell = nativeWallLightCell(dt1Entry, hidden, A, B, sample);
    if (!cell) return null;
    cells.push(cell);
  }
  return cells;
}
