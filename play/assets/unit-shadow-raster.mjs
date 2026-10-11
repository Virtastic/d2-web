// Return the destination row for a source row sampled by D2CMP's shadow blitter, or -1 when
// the row is discarded. D2CMP emits exactly floor(height / 2) destination rows, so an odd
// cel's final unpaired source row is omitted as well as the alternating row in each pair.
export function shadowDestinationRow(height, sourceY) {
  if (!Number.isInteger(height) || !Number.isInteger(sourceY) || height <= 0 || sourceY < 0 || sourceY >= height) return -1;
  const fromBottom = height - 1 - sourceY;
  const k = fromBottom >> 1;
  return (fromBottom & 1) || k >= (height >> 1) ? -1 : k;
}
