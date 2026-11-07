// Retail's unit shadow: a DESTINATION-ONLY darkening, applied once per COF layer, over a sheared
// silhouette.  Measured from retail's own frame pairs, not fitted:
//
//   * retail draws each COF layer of a unit a second time before its visible cel, at the SAME anchor,
//     under `shadow dcc:<path>` and the DCC's own path (the retail captures: all 9 Barbarian
//     layers, then the monsters' and the objects' -- 18 shadow draws, before the first wall).
//   * the transform is a half-height shear: a source row h rows above the anchor lands at
//     (x - h/2, y - h/2), taken as the INTEGER step of the destination, not a per-pixel rounding.
//     **F461:** retail's rasteriser has an unclipped row budget
//     of `height >> 1` destination rows -- it walks the cel's rows in storage order, samples every SECOND one and
//     DISCARDS the row between (never merging the pair), omitting an odd-height final row, and
//     stepping one row and one pixel left per destination row from a base of
//     `anchor + trunc(celYOffset/2)`. The ported rule below is that
//     exact walk; it reproduces retail's own shadow footprint cell-for-cell (s1510 PASS: 5,977/5,977
//     town-lit, 4,218/4,218 den-shadow-lit). The pass used to keep EVERY source row and round each
//     through `round(anchorY - 0.5h)`, which merged each pair onto one destination row and both over-
//     and under-covered retail's shadow at run ends (the whole of F402's "scene-dependence"). Over the
//     union of the 18 town-lit draws the old rule covered 4,043 pixels of the Rogue Encampment frame
//     against retail's 5,977-pixel footprint; retail's finished buffer differs from its own floors
//     buffer at 3,789 of them (91% darker) -- so the footprint is retail's shadow.
//   * the colour is `LightLevelVariations[7]` of the DESTINATION index: replaying the 18 draws in
//     retail's order from retail's own floors buffer reproduces 96.5% of the pixels retail changed
//     (town-lit: 3,659 of 3,789; den-shadow-lit: 1,868 of 1,938).  The DCC's own palette index does
//     not enter -- the ACT PL2's AlphaBlend[0][dst*256 + src] route, which this pass used before and
//     which the terrain shadow tiles do use, reproduces 0.3% of the footprint.
//   * once per COF LAYER, not once per composed silhouette: 130 wrong pixels against 305 on town-lit.
//
// Every act's pal.pl2 carries LightLevelVariations[7] at byte 1024 + 7*256 (32 light tables follow the
// 1024-byte base palette), and it is byte-identical to MultiplicativeBlend[190] in all five acts, so
// either name reaches the same table.  The base palette and pal.dat are identical in all five acts and
// their 256 colours are distinct, so a canvas pixel's index is recoverable exactly.
import { cofLayerRecord, composeCofFrame } from './unit-formats.mjs';
import { shadowMaskLayout, shadowVisibleSpans } from './unit-shadow-raster.mjs';
import { perfT0, perfT } from './frame-perf.mjs';

const ACT_PALETTE = ['ACT1', 'ACT2', 'ACT3', 'ACT4', 'ACT5'];
// PL2: 1024-byte base palette, then 32 LightLevelVariations tables, then InvColorVariations(16),
// SelectedUnitShift(1) and the blend tables (OpenDiablo2-pl2 pkg/decode.go).
const LIGHT_LEVEL_7 = 1024 + 7 * 256;

// PERF: the single-layer plane is a pure function of (cof, direction, frame, component, dcc, that component's DCC
// direction); composeCofFrame already memoises it, but only after a per-call Map and template-string key. A numeric
// key skips both every frame; the plane object (and so its mask layout) is the very same one.
const planeMemo = new WeakMap();
export function singlePlane(cof, direction, frame, component, dcc, dccDirection) {
  const req = dccDirection instanceof Map ? dccDirection.get(component) ?? 0 : dccDirection;
  if (!Number.isInteger(req) || req < 0 || req >= 64 || frame >= 4096 || direction >= 1024 || typeof dcc !== 'object') return composeCofFrame(cof, direction, frame, new Map([[component, dcc]]), dccDirection);
  let byDcc = planeMemo.get(cof); if (!byDcc) planeMemo.set(cof, byDcc = new WeakMap());
  let byKey = byDcc.get(dcc); if (!byKey) byDcc.set(dcc, byKey = new Map());
  const key = ((component * 1024 + direction) * 64 + req) * 4096 + frame;
  let plane = byKey.get(key);
  if (!plane) byKey.set(key, plane = composeCofFrame(cof, direction, frame, new Map([[component, dcc]]), dccDirection));
  return plane;
}

export function createUnitShadowRenderer(read) {
  const acts = new Map();
  const act0Of = (act) => Math.min(4, Math.max(0, Math.trunc(Number(act) || 1) - 1));
  const tables = (act) => {
    const act0 = act0Of(act);
    let value = acts.get(act0);
    if (!value) {
      value = null;
      try {
        const pl2 = read(`data\\global\\palette\\${ACT_PALETTE[act0]}\\pal.pl2`);
        if (pl2.length >= LIGHT_LEVEL_7 + 256) {
          const shadow = pl2.subarray(LIGHT_LEVEL_7, LIGHT_LEVEL_7 + 256), index = indexOfPalette(pl2);
          value = { pl2, shadow, index, shade: shadeOf(pl2, shadow, index) };
        }
      } catch { value = null; }
      acts.set(act0, value);
    }
    return value;
  };
  return {
    /**
     * Draw one unit's shadow under its own sprite.  `x, y` is the unit's anchor, the point its cel is
     * drawn at; `animation` is the COF plus the decoded component DCCs the sprite pass selected.
     * Returns the blit's identity (evidence), or null when the unit or the act's PL2 cannot supply one.
     */
    draw(ctx, x, y, animation, identity = {}) {
      const { cof, components, direction, dccDirection = direction, frame } = animation || {};
      if (!cof || !components || !(direction >= 0) || !(frame >= 0)) return null;
      if (direction >= cof.directions || frame >= cof.framesPerDirection) return null;
      const table = tables(identity.act);
      if (!table) return null;
      const width = ctx.canvas.width, height = ctx.canvas.height;
      const pShadow = perfT0();
      // One plane per COF layer, in the COF's own priority order: retail blends each layer's shadow
      // separately, so a pixel two layers cover is darkened twice.
      const layers = [];
      const eligibleComponents = [];
      let minX = width, minY = height, maxX = -1, maxY = -1;
      const pFootprint = perfT0();
      for (const component of cof.priority[direction][frame]) {
        const dcc = components.get ? components.get(component) : components[component];
        if (!dcc) continue;
        // Shadow ownership is animation data, read off the layer's own bytes:
        //   * Shadow clear -> no shadow: 8 distinct object layers, 53 cel draws, 0 shadow draws.
        //   * Shadow set and not transparent -> shadow (every torch, banner, rock, waypoint body).
        //   * Shadow set, transparent, drawEffect 3 (modulate) -> shadow: 48 retail object shadow
        //     draws across `ss`, `98\S1`, `bm\S1`, `JT\S1`, `vg\S1`, `wn\S1`, `wp\S1`, `yg\S1`.  So the
        //     transparency byte does NOT gate a shadow, and the gate this pass used to carry was wrong.
        //   * Shadow set, transparent, drawEffect 0 (the 25% family) -> NO shadow: `R1`/`R2`/`R3`, 21
        //     cel draws, 0 shadow draws.  Narrowest condition consistent with every measured draw; the
        //     same three objects carry Objects.txt `Trans = 7` where the shadowed ones carry 0, but the
        //     renderer is handed no Objects.txt row, so the COF-only form is what runs.
        // Tyrael's LA/RA are the only transparent layers on a unit, and both have Shadow clear.
        const layer = cofLayerRecord(cof, component);
        if (layer && (!layer.shadow || (layer.transparent && layer.drawEffect === 0))) continue;
        let plane;
        try { plane = singlePlane(cof, direction, frame, component, dcc, dccDirection); } catch { continue; }
        if (!plane.width || !plane.height) continue;
        eligibleComponents.push(component);
        // Retail's rasteriser: emit height >> 1 rows, walking the
        // cel's rows in storage order and keeping every SECOND one (the row between is DISCARDED,
        // never merged into a destination row); an odd-height final unpaired row is omitted.
        // Destination row k (k = 0 at the anchor) is that kept row placed at
        // (x0 - k, y0 - k), with the base x0 = anchorX + celXOffset + trunc(celYOffset / 2) and
        // y0 = anchorY + trunc(celYOffset / 2), C-truncating. The whole source row lands on one
        // destination row: the run's own bytes are skipped, and every pixel takes the destination
        // darkening (this pass's LightLevelVariations[7]). `plane.yOffset` is the composed cel's top,
        // so the DCC frame header's own `yOffset` is recovered from it and the cel's bottomUp bit.
        const yOff = plane.bottomUp ? plane.yOffset : plane.yOffset + plane.height - 1;
        const half = Math.trunc(yOff / 2);
        const baseX = Math.round(x + plane.xOffset) + half, baseY = Math.round(y) + half;
        // The footprint is a pure function of the composed plane: a kept source pixel (sx, sy) lands at
        // a DESTINATION offset (sx - k, -k) from the base cell, wherever that is drawn. The offsets are
        // cached per plane (the composer memoises planes, so the object is stable across frames for the
        // same cel/frame) and the per-frame cost is the clipped copy below, not the whole plane rescan.
        // Kept source rows have unique k values, so their destination y offsets do not overlap; pixels
        // within each row also have unique x offsets. The mask builder can append directly without a Set.
        const layout = shadowMaskLayout(plane), mask = layout.offsets;
        // Per-row binary search returns the exact in-canvas pixel range of every destination row, with
        // work proportional to cel height; the raster pass below walks only those ranges.
        const spans = shadowVisibleSpans(layout, baseX, baseY, width, height);
        if (spans.length) {
          for (let s = 0; s < spans.length; s += 3) {
            const py = spans[s], left = baseX + mask[spans[s + 1] * 2], right = baseX + mask[(spans[s + 2] - 1) * 2];
            if (left < minX) minX = left;
            if (right > maxX) maxX = right;
            if (py < minY) minY = py;
            if (py > maxY) maxY = py;
          }
          layers.push({ component, mask, baseX, spans });
        }
      }
      perfT('unit-shadow-footprint', pFootprint);
      if (!layers.length) {
        if (!eligibleComponents.length) return null;
        // A native shadow call can have no raster output (for example, height 1 gives height>>1 = 0).
        // Keep its attempted draw identity distinct from a COF-gated/no-shadow result.
        const blit = { ...identity, x: Math.round(x), y: Math.round(y), layers: eligibleComponents.length,
          components: eligibleComponents, pixels: 0, darkened: 0, method: 'pl2-lightlevel-7',
          projection: 'x=-0.5h,y=-0.5h', act: act0Of(identity.act) + 1, rect: null, rasterized: false };
        // `__d2UnitShadows` is the visible raster output list; keep this call identity only on the
        // return value so zero-pixel calls cannot masquerade as visible shadows in pixel probes.
        return blit;
      }
      // PERF (handheld): each band is one getImageData + one putImageData, each allocating; a short shadow (a torch, a
      // banner) has no diagonal worth banding, so it gets one band per 8 rows (1..4). The destination colour feeds the
      // darkening, so a readback of the covered pixels is inherent; this only cuts the calls, never a pixel.
      // PERF (a retail recording): the shadow is a thin diagonal, so its bounding rect is mostly untouched pixels and
      // getImageData is linear in them. Read it in BANDS rows tall, each only as wide as its own rows' extent.
      // Each pixel is still read, shadowed and written once, layers in the same order: identical output.
      const rows = maxY - minY + 1, BANDS = Math.min(4, Math.max(1, rows >> 3)), rowLo = new Int32Array(rows).fill(maxX + 1), rowHi = new Int32Array(rows).fill(minX - 1);
      for (const { mask, baseX, spans } of layers) for (let s = 0; s < spans.length; s += 3) {
        const r = spans[s] - minY, left = baseX + mask[spans[s + 1] * 2], right = baseX + mask[(spans[s + 2] - 1) * 2];
        if (left < rowLo[r]) rowLo[r] = left; if (right > rowHi[r]) rowHi[r] = right;
      }
      // PERF (throne wave, 38 shadowed units): the bands are disjoint row ranges, so every in-rect mask pixel
      // lies in exactly one band. Read the bands first, then walk the masks ONCE in layer order (the per-band
      // walk re-scanned every mask BANDS times), darkening through a per-colour memo of the destination's
      // shadow colour, then write the bands back. Same pixels, same order per pixel, same counts.
      const images = [], views = [], rowBand = new Uint8Array(rows), bandX = new Int32Array(BANDS), bandY = new Int32Array(BANDS), bandW = new Int32Array(BANDS);
      for (let band = 0; band < BANDS; band++) {
        const r0 = Math.floor(rows * band / BANDS), r1 = Math.floor(rows * (band + 1) / BANDS) - 1;
        let bx0 = maxX + 1, bx1 = minX - 1;
        for (let r = r0; r <= r1; r++) { if (rowLo[r] < bx0) bx0 = rowLo[r]; if (rowHi[r] > bx1) bx1 = rowHi[r]; rowBand[r] = band; }
        if (bx1 < bx0) { images.push(null); views.push(null); continue; }
        const image = ctx.getImageData(bx0, minY + r0, bx1 - bx0 + 1, r1 - r0 + 1);
        images.push(image); views.push(new Uint32Array(image.data.buffer, image.data.byteOffset, image.data.length >>> 2));
        bandX[band] = bx0; bandY[band] = minY + r0; bandW[band] = image.width;
      }
      const shade = table.shade;
      let pixels = 0, darkened = 0;
      for (const { mask, baseX, spans } of layers) for (let s = 0; s < spans.length; s += 3) {
        const py = spans[s], band = rowBand[py - minY], d = views[band];
        const k0 = (py - bandY[band]) * bandW[band] + baseX - bandX[band], end = spans[s + 2] * 2;
        for (let i = spans[s + 1] * 2; i < end; i += 2) {
          const k = k0 + mask[i], v = shade(d[k]);
          if (v < 0) darkened++;
          d[k] = v | 0xff000000;
        }
        pixels += spans[s + 2] - spans[s + 1];
      }
      for (let band = 0; band < BANDS; band++) if (images[band]) ctx.putImageData(images[band], bandX[band], bandY[band]);
      perfT('unit-shadow-total', pShadow);
      const blit = { ...identity, x: Math.round(x), y: Math.round(y), layers: layers.length, components: layers.map((l) => l.component), pixels, darkened,
        method: 'pl2-lightlevel-7', projection: 'x=-0.5h,y=-0.5h', act: act0Of(identity.act) + 1, rasterized: true,
        rect: [minX, minY, maxX - minX + 1, maxY - minY + 1] };
      if (Array.isArray(globalThis.__d2UnitShadows)) globalThis.__d2UnitShadows.push(blit);
      return blit;
    },
  };
}

// One destination pixel (a little-endian RGBA word) through the shadow: LightLevelVariations[7] of its palette
// index, as the PL2 colour r | g << 8 | b << 16, with bit 31 set when it is darker (r+g+b) than the pixel was.
// A pure function of the pixel's RGB, so a direct-mapped memo in front of indexOfPalette returns its own answer.
function shadeOf(pl2, shadow, index) {
  const ck = new Int32Array(4096).fill(-1), cv = new Int32Array(4096);
  return (pixel) => {
    const key = pixel & 0xffffff, h = Math.imul(key, 0x9E3779B1) >>> 20;
    if (ck[h] === key) return cv[h];
    const r = key & 0xff, g = (key >>> 8) & 0xff, b = key >>> 16, out = shadow[index(r, g, b)] * 4;
    const R = pl2[out], G = pl2[out + 1], B = pl2[out + 2];
    const v = (R | (G << 8) | (B << 16) | (R + G + B < r + g + b ? 0x80000000 : 0)) | 0;
    ck[h] = key; cv[h] = v;
    return v;
  };
}

// The PL2's base palette (the same 256 colours as the act's pal.dat, byte for byte in all five acts)
// with distinct colours, so a canvas pixel recovers its index exactly; nearest is the fallback for a
// pixel something else has already blended.
function indexOfPalette(pl2) {
  const map = new Map();
  for (let i = 255; i >= 0; i--) map.set((pl2[i * 4] << 16) | (pl2[i * 4 + 1] << 8) | pl2[i * 4 + 2], i);
  return (r, g, b) => {
    const key = (r << 16) | (g << 8) | b;
    const hit = map.get(key);
    if (hit !== undefined) return hit;
    let best = 0, distance = Infinity;
    for (let i = 0; i < 256; i++) {
      const dr = pl2[i * 4] - r, dg = pl2[i * 4 + 1] - g, db = pl2[i * 4 + 2] - b, d = dr * dr + dg * dg + db * db;
      if (d < distance) { distance = d; best = i; }
    }
    map.set(key, best);
    return best;
  };
}
