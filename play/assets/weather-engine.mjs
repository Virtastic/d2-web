// The engine's rain drops for the frame: D2Client 1.13c's weather pass (0x81340 step, 0x7FE80 draw) as
// src/core/d2client/weather_lines.cpp exports it. Null when the build has no export (the caller falls back).
export function readEngineRainDrops(Module, width, height, mode = 0) {
  if (!Module?._d2_client_weather_lines) return null;
  const n = Number(Module._d2_client_weather_lines(width, height, mode));
  const base = Number(Module._d2_client_weather_lines_ptr()) >> 2;
  const heap = Module.HEAP32, out = [];
  for (let i = 0; i < n; i++) {
    const o = base + i * 6;
    out.push({ x0: heap[o], y0: heap[o + 1], x1: heap[o + 2], y1: heap[o + 3], colour: heap[o + 4] & 255, alpha: heap[o + 5] & 255 });
  }
  return out;
}
