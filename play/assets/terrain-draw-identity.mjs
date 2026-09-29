// The draw-list vocabulary for a terrain blit.  This is deliberately about the
// engine's placed-tile list, not the visible art: a comparison needs to retain
// the renderer entry that selected that blit even when the same DT1 identity is
// matched across wall-like entries (F65).
export function terrainDrawOp(tile) {
  if (tile.orientation === 0 || tile.orientation === 15) return 'floor';
  // Shadow orientation takes precedence: pRoofTiles is populated by shadow
  // tiles too, but retail dispatches orientation 13 through slot 41.
  if (tile.orientation === 13) return 'shadowtile';
  return tile.layer === 2 ? 'roof' : 'wall';
}
