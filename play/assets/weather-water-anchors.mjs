import { worldToScreen } from './session-terrain.mjs';

// F11b's retail captures place Rain3/Rain4 ground ripples on water. A DT1 subtile collision byte is
// not a water label: +0x28 marks walk/collision properties and also equals 1 around dry town props
// such as the Rogue Encampment firepit. The material flags at +0x06 carry TILE_FLAGS_WATER (0x2),
// cross-checked against D2MOO D2CMP.h / MonsterSpawn.cpp and measured against rip-ford-full/dry.
// Keep eligible positions in world-screen coordinates so camera motion cannot turn a ripple into a
// screen-space mark.
export function weatherWaterAnchors(floors, { width = 800, height = 600, offsetX = width / 2, offsetY = height / 2 - 8 } = {}) {
  const anchors = [], seen = new Set();
  for (const floor of floors || []) {
    const tile = floor?.tile, dt1 = floor?.dt1Tile;
    if (!tile || !dt1 || !((dt1.materialFlags ?? ((dt1.soundIndex ?? 0) | ((dt1.animated ?? 0) << 8))) & 0x2)) continue;
    const flags = dt1.subtileFlags;
    if (!flags || flags.length !== 25) continue;
    for (let i = 0; i < 25; i++) {
      if (flags[i] !== 1) continue;
      const x = tile.worldX * 5 + i % 5, y = tile.worldY * 5 + Math.floor(i / 5), key = `${x},${y}`;
      if (seen.has(key)) continue;
      seen.add(key);
      const p = worldToScreen(x + 0.5, y + 0.5);
      const screenX = p.x + offsetX, screenY = p.y + offsetY;
      // Retail puts these effects in the world pass before the HUD. Do not seed particles where the
      // canvas cannot show them; the cells remain world anchored and are reselected at the next wrap.
      if (screenX < 0 || screenX >= width || screenY < 0 || screenY >= height - 120) continue;
      anchors.push({ x, y, sx: p.x, sy: p.y });
    }
  }
  return anchors;
}
