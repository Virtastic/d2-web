// Optional widescreen frontend backgrounds (local, retail-derived, never committed: <site>/widebg/).
// <site>/widebg/manifest.json = { "<dc6 basename, lower case>": { file, w, h, s, ox, oy } }, where the
// image is registered to retail's 800x600 frame: retail pixel (0,0) sits at image pixel (ox,oy) and
// one retail pixel is `s` image pixels wide and `sy` (default s) tall (the painted images are a hair anisotropic). A screen asks wideBg(path); null means draw the MPQ DC6 as before.
// The image is used WHOLE. Overlays stay in 800x600 and are drawn exactly where retail puts them.
const entries = {};
const base = new URL('../widebg/', import.meta.url);
export const widebgReady = (async () => {
  try {
    const m = await (await fetch(new URL('manifest.json', base))).json();
    for (const [k, e] of Object.entries(m)) {
      const img = await createImageBitmap(await (await fetch(new URL(e.file, base))).blob());
      entries[k] = { ...e, img, bg800: null };
    }
  } catch { /* no local images: retail DC6 */ }
})();

export const wideBg = (dc6Path) => entries[dc6Path.split('\\').pop().replace(/\.dc6$/i, '').toLowerCase()] || null;

/** The image resampled into the 800x600 scene (what blends read back and overlay diffs compare to). */
export function sceneBg(e, ctx) {
  if (!e.bg800) {
    const c = ctx.canvas.ownerDocument.createElement('canvas'); c.width = 800; c.height = 600;
    const g = c.getContext('2d', { willReadFrequently: true }); g.imageSmoothingQuality = 'high';
    g.drawImage(e.img, -e.ox / e.s, -e.oy / (e.sy ?? e.s), e.w / e.s, e.h / (e.sy ?? e.s));
    const data = g.getImageData(0, 0, 800, 600).data;
    e.bg800 = { c, data, data32: new Uint32Array(data.buffer) };
  }
  ctx.drawImage(e.bg800.c, 0, 0);
}

let layer = null;
/** Present: the whole image full-res under the view, then only what the scene drew over the 800x600 background. */
export function presentWide(e, out, scene, v) {
  const k = v.scale / e.s, ky = v.scale / (e.sy ?? e.s);
  out.save();
  // The scaled backdrop (edge repeats + the high-quality resample) never changes for a window size: draw it once into a cache canvas.
  const OW = out.canvas.width, OH = out.canvas.height, key = `${OW},${OH},${v.ox},${v.oy},${v.scale}`;
  if (e.back?.key !== key) {
    const c = e.back?.c ?? out.canvas.ownerDocument.createElement('canvas'); c.width = OW; c.height = OH;
    const o = c.getContext('2d'); o.imageSmoothingEnabled = true; o.imageSmoothingQuality = 'high';
    const L = v.ox - e.ox * k, T = v.oy - e.oy * ky, W = e.w * k, H = e.h * ky;
    // A registered image that falls a few px (<= 24) short of the window edge repeats its own edge row/column; a wider gap (portrait) stays black, a smear is worse.
    const N = 24;
    if (T > 0 && T <= N) o.drawImage(e.img, 0, 0, e.w, 1, L, 0, W, T + 1);
    if (T + H < OH && OH - T - H <= N) o.drawImage(e.img, 0, e.h - 1, e.w, 1, L, T + H - 1, W, OH - T - H + 1);
    if (L > 0 && L <= N) o.drawImage(e.img, 0, 0, 1, e.h, 0, T, L + 1, H);
    if (L + W < OW && OW - L - W <= N) o.drawImage(e.img, e.w - 1, 0, 1, e.h, L + W - 1, T, OW - L - W + 1, H);
    o.drawImage(e.img, L, T, W, H);
    e.back = { c, key };
  }
  out.drawImage(e.back.c, 0, 0);
  const g = scene.getContext('2d'), px = g.getImageData(0, 0, 800, 600), d = px.data, d32 = new Uint32Array(d.buffer), b32 = e.bg800.data32;
  for (let i = 0; i < d32.length; i++) if (((d32[i] ^ b32[i]) & 0xffffff) === 0) d[i * 4 + 3] = 0;
  // Sized once: re-setting width/height every frame reallocates and clears the store; putImageData writes every pixel anyway.
  if (!layer) { layer = scene.ownerDocument.createElement('canvas'); layer.width = 800; layer.height = 600; }
  layer.getContext('2d').putImageData(px, 0, 0);
  out.imageSmoothingEnabled = false;
  out.drawImage(layer, v.ox, v.oy, 800 * v.scale, 600 * v.scale);
  out.restore();
}

/** Scene background: the registered image when present, else the retail DC6's composed canvas. */
export function drawBg(ctx, path, canvas) { const e = wideBg(path); if (e) sceneBg(e, ctx); else ctx.drawImage(canvas, 0, 0); }
/** Put the finished 800x600 scene on the page. */
export function present(out, scene, v, path) {
  const e = wideBg(path);
  if (e) presentWide(e, out, scene, v);
  else { out.save(); out.imageSmoothingEnabled = false; out.drawImage(scene, v.ox, v.oy, 800 * v.scale, 600 * v.scale); out.restore(); }
}
