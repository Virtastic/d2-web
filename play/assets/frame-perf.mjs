// The client's frame-cost instrument (perf work, 2026-09-29).
//
// Reported: "the game is very low fps and slow and not optimized." An impression is not a row; this is
// the number. It answers two questions the shell could not before: how long a frame takes as a
// DISTRIBUTION (p50 / p95 / max -- a steady cost and one stall share a mean and are different
// defects), and WHERE that time goes, attributed to a named pass.
//
// OFF BY DEFAULT, and free while off. Nothing is read and nothing is stored unless the page sets
// `window.__d2Perf = true` (or loads with `?perf=1`); every pass is a `perfT0()` / `perfT(name, t0)`
// pair that returns immediately while off, so the shipped frame draws byte-identically and pays one
// boolean per frame, not a timestamp per blit. This is deliberate: the score is `s000a..s000e` at 0
// differing records and every rendering row compares against retail, so the instrument must not be
// able to change a pixel.
//
// RAW SAMPLES, never a running mean. `perfReport()` sorts each pass's samples and reports
// p50/p95/max/mean with the sample count, so a caller reads the shape rather than a summary that
// hides it.
//
// A pass name is a plain string; names nest by convention (`world` contains `terrain`, `hud`, ...).
// The reader subtracts what it needs; nothing here assumes a tree.

const MAX_SAMPLES = 4000;

export const perfState = { on: false, passes: Object.create(null), frames: 0, resets: 0 };

// Refresh `on` from the global. Called once per frame by the shell (and once per draw by the terrain
// pass), so a page can turn the instrument on AFTER boot -- a scenario sets `window.__d2Perf = true`
// and drives frames, without a reload.
export function perfSync() { perfState.on = globalThis.__d2Perf === true; return perfState.on; }
export function perfEnabled() { return perfState.on; }

// A timestamp for the start of a pass, or 0 while off (so the matching perfT is a no-op and no
// `performance.now()` was spent).
export function perfT0() { return perfState.on ? performance.now() : 0; }

// Close a pass opened with perfT0(). Ignores a 0 start (off) or a non-finite delta.
export function perfT(name, t0) {
  if (!perfState.on || !t0) return;
  const ms = performance.now() - t0;
  if (Number.isFinite(ms)) perfSample(name, ms);
}

// Record an already-measured duration (a per-pixel loop that accumulates its own time, or a value
// measured across a span the pass pairs cannot express).
export function perfSample(name, ms) {
  if (!perfState.on || !Number.isFinite(ms)) return;
  let a = perfState.passes[name];
  if (!a) a = perfState.passes[name] = [];
  if (a.length < MAX_SAMPLES) a.push(ms);
}

export function perfCountFrame() { if (perfState.on) perfState.frames++; }

export function perfReset() { perfState.passes = Object.create(null); perfState.frames = 0; perfState.resets++; }

// Nearest-rank percentile on an ascending array (no interpolation: every reported number is a sample
// some frame actually took).
export function percentile(sorted, p) {
  if (!sorted.length) return 0;
  const i = Math.min(sorted.length - 1, Math.max(0, Math.ceil((p / 100) * sorted.length) - 1));
  return sorted[i];
}

// p50/p95/max/mean per pass, plus the frame count. Plain data (returnByValue-safe: it crosses a CDP
// evaluate boundary).
export function perfReport() {
  const passes = {};
  for (const name of Object.keys(perfState.passes)) {
    const a = perfState.passes[name];
    if (!a.length) continue;
    const s = a.slice().sort((x, y) => x - y);
    const sum = s.reduce((x, y) => x + y, 0);
    passes[name] = { n: s.length,
      p50: +percentile(s, 50).toFixed(3), p95: +percentile(s, 95).toFixed(3),
      max: +s[s.length - 1].toFixed(3), mean: +(sum / s.length).toFixed(3) };
  }
  return { frames: perfState.frames, on: perfState.on, resets: perfState.resets, passes };
}
