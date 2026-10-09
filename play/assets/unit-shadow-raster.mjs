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
  return shadowMaskLayout(plane).offsets;
}

// Keep each destination row's range into the legacy flat mask.
// Repeated draws can then find exact clipped bounds by scanning rows rather
// than walking every shadow pixel a first time before the raster pass, without
// duplicating the X offsets in a second per-pixel array.
export function shadowMaskLayout(plane) {
  let layout = masks.get(plane);
  if (layout) return layout;
  const offsets = [], rows = [];
  for (let sy = 0; sy < plane.height; sy++) {
    const k = shadowDestinationRow(plane.height, sy);
    if (k < 0) continue;
    const row = sy * plane.width, start = offsets.length >>> 1;
    for (let sx = 0; sx < plane.width; sx++) {
      if (!plane.opaque[row + sx]) continue;
      // Every retained source row has a unique k and therefore a unique destination y (-k).
      // Within that row sx is unique, so no two source pixels can map to the same offset.
      const dx = sx - k;
      offsets.push(dx, -k);
    }
    const end = offsets.length >>> 1;
    if (start !== end) rows.push({ dy: -k, start, end });
  }
  layout = { offsets: Int32Array.from(offsets), rows };
  masks.set(plane, layout);
  return layout;
}

// Write the exact pixel-bounded rectangle of the translated mask into `out` as
// [minX, minY, maxX, maxY]. Sorted row values make clipping two binary searches
// per destination row instead of another pass over every opaque pixel.
export function shadowVisibleBounds(layout, baseX, baseY, width, height, out) {
  let minX = width, minY = height, maxX = -1, maxY = -1;
  const minOffsetX = -baseX, maxOffsetX = width - 1 - baseX;
  const offsets = layout.offsets;
  for (const row of layout.rows) {
    const y = baseY + row.dy;
    if (y < 0 || y >= height) continue;
    let low = row.start, high = row.end;
    while (low < high) {
      const middle = (low + high) >>> 1;
      if (offsets[middle * 2] < minOffsetX) low = middle + 1;
      else high = middle;
    }
    const first = low;
    low = first; high = row.end;
    while (low < high) {
      const middle = (low + high) >>> 1;
      if (offsets[middle * 2] <= maxOffsetX) low = middle + 1;
      else high = middle;
    }
    const afterLast = low;
    if (first === afterLast) continue;
    const left = baseX + offsets[first * 2], right = baseX + offsets[(afterLast - 1) * 2];
    if (left < minX) minX = left;
    if (right > maxX) maxX = right;
    if (y < minY) minY = y;
    if (y > maxY) maxY = y;
  }
  if (maxX < 0) return false;
  out[0] = minX; out[1] = minY; out[2] = maxX; out[3] = maxY;
  return true;
}
