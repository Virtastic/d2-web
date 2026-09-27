// Retail's unit shadow: a DESTINATION-ONLY darkening, applied once per COF layer, over a sheared
// silhouette.  Measured from retail's own frame pairs, not fitted:
//
//   * retail draws each COF layer of a unit a second time before its visible cel, at the SAME anchor,
//     under `shadow dcc:<path>` and the DCC's own path (the retail captures: all 9 Barbarian
//     layers, then the monsters' and the objects' -- 18 shadow draws, before the first wall).
//   * the transform is a half-height shear: a source row h rows above the anchor lands at
//     (x - h/2, y - h/2).  Over the union of the 18 draws it covers 4,043 pixels of the Rogue
//     Encampment frame, and retail's own finished buffer differs from its own floors buffer at 3,789
//     of them (91% darker) -- so the footprint is retail's shadow.
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
import { composeCofFrame } from './unit-formats.mjs';

const ACT_PALETTE = ['ACT1', 'ACT2', 'ACT3', 'ACT4', 'ACT5'];
// PL2: 1024-byte base palette, then 32 LightLevelVariations tables, then InvColorVariations(16),
// SelectedUnitShift(1) and the blend tables (OpenDiablo2-pl2 pkg/decode.go).
const LIGHT_LEVEL_7 = 1024 + 7 * 256;

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
        if (pl2.length >= LIGHT_LEVEL_7 + 256) value = { pl2, shadow: pl2.subarray(LIGHT_LEVEL_7, LIGHT_LEVEL_7 + 256), index: indexOfPalette(pl2) };
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
      const { cof, components, direction, frame } = animation || {};
      if (!cof || !components || !(direction >= 0) || !(frame >= 0)) return null;
      if (direction >= cof.directions || frame >= cof.framesPerDirection) return null;
      const table = tables(identity.act);
      if (!table) return null;
      const width = ctx.canvas.width, height = ctx.canvas.height;
      // One plane per COF layer, in the COF's own priority order: retail blends each layer's shadow
      // separately, so a pixel two layers cover is darkened twice.
      const layers = [];
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
        const layer = cof.layerRecords?.find((l) => l.component === component);
        if (layer && (!layer.shadow || (layer.transparent && layer.drawEffect === 0))) continue;
        let plane;
        try { plane = composeCofFrame(cof, direction, frame, new Map([[component, dcc]])); } catch { continue; }
        if (!plane.width || !plane.height) continue;
        const left = Math.round(x + plane.xOffset), top = Math.round(y + plane.yOffset);
        const pixels = new Set();
        for (let sy = 0; sy < plane.height; sy++) for (let sx = 0; sx < plane.width; sx++) {
          if (!plane.opaque[sy * plane.width + sx]) continue;
          const h = y - (top + sy);                       // the source row's height above the anchor
          const px = Math.round(left + sx - 0.5 * h), py = Math.round(y - 0.5 * h);
          if (px < 0 || py < 0 || px >= width || py >= height) continue;
          pixels.add(py * width + px);
        }
        if (pixels.size) layers.push({ component, pixels });
      }
      if (!layers.length) return null;
      let minX = width, minY = height, maxX = -1, maxY = -1;
      for (const layer of layers) for (const at of layer.pixels) {
        const px = at % width, py = (at - px) / width;
        if (px < minX) minX = px; if (px > maxX) maxX = px;
        if (py < minY) minY = py; if (py > maxY) maxY = py;
      }
      const image = ctx.getImageData(minX, minY, maxX - minX + 1, maxY - minY + 1);
      const data = image.data, iw = image.width, pal = table.pl2;
      let pixels = 0, darkened = 0;
      for (const layer of layers) for (const at of layer.pixels) {
        const px = at % width, py = (at - px) / width;
        if (px < minX || py < minY || px > maxX || py > maxY) continue;
        const p = ((py - minY) * iw + px - minX) * 4;
        const out = table.shadow[table.index(data[p], data[p + 1], data[p + 2])];
        const r = pal[out * 4], g = pal[out * 4 + 1], b = pal[out * 4 + 2];
        if (r + g + b < data[p] + data[p + 1] + data[p + 2]) darkened++;
        data[p] = r; data[p + 1] = g; data[p + 2] = b; data[p + 3] = 255;
        pixels++;
      }
      ctx.putImageData(image, minX, minY);
      const blit = { ...identity, x: Math.round(x), y: Math.round(y), layers: layers.length, components: layers.map((l) => l.component), pixels, darkened,
        method: 'pl2-lightlevel-7', projection: 'x=-0.5h,y=-0.5h', act: act0Of(identity.act) + 1,
        rect: [minX, minY, maxX - minX + 1, maxY - minY + 1] };
      if (Array.isArray(globalThis.__d2UnitShadows)) globalThis.__d2UnitShadows.push(blit);
      return blit;
    },
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
