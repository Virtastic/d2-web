// The select screen's character widget animation (D2Win FUN_6F8EE5F0, 1.13c): each draw shows frame
// acc>>8 of the TN COF, then acc += COF speed (8.8 fixed point); once the new acc>>8 reaches
// framesPerDirection-1 the accumulator wraps to 0. The widget (A8C0 / D2Win #10172) starts at 0 when
// a card is selected. Retail draws at the 25 Hz front-end rate, the shell steps at 60 Hz.
export const SELECT_RETAIL_HZ = 25, SELECT_UPDATE_HZ = 60;

/** Frame shown on retail draw number `draws` (0 = first draw after the widget was reset). */
export function tnFrame(draws, speed, count) {
  if (count <= 1 || speed <= 0) return 0;
  const period = Math.ceil((count - 1) * 256 / speed);
  return (((draws % period) * speed) >> 8);
}

/** Retail draw number for a 60 Hz shell tick counted from `start`. */
export const drawsSince = (tick, start) => Math.floor((tick - start) * SELECT_RETAIL_HZ / SELECT_UPDATE_HZ);
