// Index only the two equality fields that lead DATATBLS_GetAutomapCellId's ordered
// predicate. Keep each bucket in AutoMap.txt order so overlapping rows still choose
// the same first match.
export function createAutomapRecordIndex(rows) {
  const byLevel = new Map();
  for (const row of rows) {
    let byTile = byLevel.get(row.levelName);
    if (!byTile) byLevel.set(row.levelName, byTile = new Map());
    let bucket = byTile.get(row.tileName);
    if (!bucket) byTile.set(row.tileName, bucket = []);
    bucket.push(row);
  }
  return byLevel;
}

export function selectAutomapRecord(rows, levelName, tileName, tile) {
  for (const row of rows) {
    if (row.levelName === levelName && row.tileName === tileName &&
      (row.style === -1 || row.style === tile.mainIndex) &&
      (row.startSequence === -1 || (row.startSequence <= tile.subIndex && tile.subIndex <= row.endSequence))) return row;
  }
  return null;
}

export function selectIndexedAutomapRecord(index, levelName, tileName, tile) {
  return selectAutomapRecord(index.get(levelName)?.get(tileName) ?? [], levelName, tileName, tile);
}
