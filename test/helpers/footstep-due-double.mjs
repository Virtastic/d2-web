// A test double for the engine's footfall due test (_d2_client_footstep_due, D2Client 0x23C00), for tests that run
// without the engine build. realm-event-sounds.test.mjs checks it against the engine whenever the build is present.
const circ = (a, b, m) => { const ra = a % m, rb = b % m, d = rb - ra; return Math.min(Math.abs(d), Math.abs(d - m), Math.abs(ra - rb - m)); };
export function footstepDue(cur, total, step, n, off, now, last) {
  if (!step || !n) return 0;
  const tick = Math.trunc(total / (n * step));
  if (n > 1 && ((now - last) >>> 0) < Math.trunc((tick * 2) / 3)) return 0;
  const m = Math.trunc(total / n), o = off << 8;
  if (m <= 0) return 0;
  const d = circ(cur | 0, o, m);
  return +(d < circ((step + cur) | 0, o, m) && d < circ((cur - step) | 0, o, m));
}
