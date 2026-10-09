// D2Client 1.13c 0x6fb1474e clamps AutoMapMode to 0..2. Its art loader
// (0x6fb0fb50) uses small art only for 1; the Fade ramp (0x6fb10250)
// requires exactly 0. Preserve 2 even though it shares 0's art and divisor.
export function nativeAutomapMode(mode) {
  if (mode === 'mini') return 1;
  return mode === 1 || mode === 2 ? mode : 0;
}
