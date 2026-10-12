// Return the destination row for a source row sampled by D2CMP's shadow blitter, or -1 when
// the row is discarded. D2CMP emits exactly floor(height / 2) destination rows, so an odd
// cel's final unpaired source row is omitted as well as the alternating row in each pair.
export function shadowDestinationRow(height, sourceY) {
  if (!Number.isInteger(height) || !Number.isInteger(sourceY) || height <= 0 || sourceY < 0 || sourceY >= height) return -1;
  const fromBottom = height - 1 - sourceY;
  const k = fromBottom >> 1;
  return (fromBottom & 1) || k >= (height >> 1) ? -1 : k;
}

// Cache the deduplicated shear mask for a stable opaque plane. Values are
// anchor-independent offsets, so callers can clip them against any canvas.
const masks = new WeakMap();

export function shadowMaskOffsets(plane) {
  let mask = masks.get(plane);
  if (mask) return mask;
  const offsets = [];
  for (let sy = 0; sy < plane.height; sy++) {
    const k = shadowDestinationRow(plane.height, sy);
    if (k < 0) continue;
    const row = sy * plane.width;
    for (let sx = 0; sx < plane.width; sx++) {
      if (!plane.opaque[row + sx]) continue;
      // Every retained source row has a unique k and therefore a unique destination y (-k).
      // Within that row sx is unique, so no two source pixels can map to the same offset.
      offsets.push(sx - k, -k);
    }
  }
  mask = Int32Array.from(offsets);
  masks.set(plane, mask);
  return mask;
}
