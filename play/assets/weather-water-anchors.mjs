import { worldToScreen } from './session-terrain.mjs';

// F11b's retail captures place Rain3/Rain4 ground ripples on water. A DT1 subtile collision byte is
// not a water label: +0x28 marks walk/collision properties and also equals 1 around dry town props
// such as the Rogue Encampment firepit. The material flags at +0x06 carry TILE_FLAGS_WATER (0x2),
// cross-checked against D2MOO D2CMP.h / MonsterSpawn.cpp and measured against rip-ford-full/dry.
// Keep eligible positions in world-screen coordinates so camera motion cannot turn a ripple into a
// screen-space mark.
export function weatherWaterAnchors(floors) {
  const anchors = [];
  for (const floor of floors || []) {
    const tile = floor?.tile, dt1 = floor?.dt1Tile;
    // FUN_6FAC4050 requests floor arrays for selectors 1 then 2. A completed renderer list can
    // also contain other floor selector classes, so apply the native (dwFlags >> 14) & 7 gate.
    const selector = ((tile?.flags ?? 0) >>> 14) & 7;
    if (selector !== 1 && selector !== 2) continue;
    // D2Client 1.13c FUN_6FAC3CE0 reaches its water emission only after rejecting
    // D2DrlgTileDataStrc flags 0x408 and requiring D2TileLibraryEntryStrc.nType (+0x14) == 0.
    // `tile` is the former and `dt1Tile.orientation` is the latter. A renderer floor bucket alone
    // can still contain shadow/other-orientation entries, so preserve the native checks here.
    if (!tile || ((tile.flags ?? 0) & 0x408) !== 0 || (dt1?.orientation ?? tile.orientation) !== 0) continue;
    if (!dt1 || !((dt1.materialFlags ?? ((dt1.soundIndex ?? 0) | ((dt1.animated ?? 0) << 8))) & 0x2)) continue;
    // FUN_6FAC3CE0 submits once for each rendered water DT1 entry. It does not iterate the tile's
    // 25 collision bytes or deduplicate equal world coordinates; those affect movement, not this
    // draw callback. The world origin passed to the emitter is the tile's room origin plus nPosX/Y*5.
    const x = tile.worldX * 5, y = tile.worldY * 5;
    const p = worldToScreen(x, y);
    // Keep edge and lower-screen candidates from the visible floor list. The cel has its own
    // offsets and dimensions, so rejecting by anchor alone drops partially visible ripples. The
    // canvas clips the final cel rectangle, and terrain's floor pass has already culled floor tiles.
    anchors.push({ x, y, sx: p.x, sy: p.y });
  }
  return anchors;
}
