// A new game's map seed, derived the way retail's game create derives dwInitSeed (D2Game, game
// create with no seed forced): the performance counter's low word goes through SEED_GetRandomValue,
// becomes the low seed of a fresh game seed {low, 666}, one roll of that seed (low 32 bits of
// 0x6AC690C5 * low + high) goes through SEED_GetRandomValue again, and that is dwInitSeed.
// SEED_GetRandomValue(n) = (0x2F490A95 * (time + n + tickCount) - 0x2E330917) & 0x7FFFFFFF, in
// 32-bit arithmetic. The clock inputs are explicit so a test can pin them; clockInputs() reads them.
export function seedGetRandomValue(n, timeSec, tickMs) {
  return (Math.imul(0x2F490A95, (timeSec + n + tickMs) | 0) - 0x2E330917 & 0x7FFFFFFF) >>> 0;
}

export function gameInitSeed({ qpcLow, timeSec, tickMs }) {
  const low = seedGetRandomValue(qpcLow | 0, timeSec, tickMs);
  const roll = (Number((0x6AC690C5n * BigInt(low) + 666n) & 0xFFFFFFFFn)) | 0;
  return seedGetRandomValue(roll, timeSec, tickMs);
}

// The three clocks retail reads: QueryPerformanceCounter (low word), time(NULL), GetTickCount.
export function clockInputs() {
  const perf = typeof performance !== 'undefined' ? performance.now() : 0;
  return {
    qpcLow: Math.floor(perf * 1000) >>> 0,
    timeSec: Math.floor(Date.now() / 1000) >>> 0,
    tickMs: Math.floor(perf) >>> 0,
  };
}
