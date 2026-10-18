// A level entrance -- a cave mouth, crypt, sewer, town exit or portal -- is a UNIT_TILE warp, not an
// Objects.txt object unit, so the object-label path never sees it and it had no hover name. Retail's
// tile unit carries the destination level's name, and the client resolves that from the engine's own
// Levels.txt row (d2_level_name -> d2_lang_string), never from a name table of its own.
//
// Both halves are pure so a test can drive the hit test and the record it produces together.

// The exit under the pointer: retail hit-tests the warp's LvlWarp selection box in SCREEN space
// (SelectX/Y/DX/DY px, y down, from the warp's anchor = tile*5 + Offset - 2 subtiles; Diablerie
// Warp.cs). The smallest box containing the pointer wins. `project` maps a world subtile to a canvas
// pixel (the shell's own static-grid projection).
export function warpAtPoint(warps, px, py, project, currentLevel) {
  let best = null, bestArea = Infinity;
  for (const warp of warps || []) {
    if (!warp.enabled || warp.located === false || !warp.select || warp.destinationLevel === currentLevel) continue;
    const [ax, ay] = project(warp.originX, warp.originY), b = warp.select;
    // F6 MEASURED (retail, hover-select-f14-f6.json): edges are INCLUSIVE, the box is (DX+1)x(DY+1) px.
    if (px < ax + b.x || px > ax + b.x + b.dx || py < ay + b.y || py > ay + b.y + b.dy) continue;
    if (b.dx * b.dy < bestArea) { best = warp; bestArea = b.dx * b.dy; }
  }
  return best;
}

// The label record for a hit warp and the destination's resolved level name. No name -> no label
// (retail never invents one), which is also the negative control the test pins.
export function areaEntranceLabel(warp, name) {
  const label = String(name || '').trim();
  if (!warp || !label) return null;
  return { kind: 'warp', id: warp.slot, warpId: warp.warpId, destinationLevel: warp.destinationLevel,
    label, x: warp.x, y: warp.y };
}
